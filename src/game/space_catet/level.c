#include "core/core.h"
#include "ecs/ecs.h"
#include "log/log.h"
#include "gfx/gfx.h"
#include "txt/txt.h"
#include "gl/gl_util.h"
#include "space_catet.h"
#include "snd/snd.h"

#define WIN_W 640
#define WIN_H 480

#define SHIP_SIZE 64

/* Player stuff */
unsigned int txt;
unsigned int btxt;
static int ticks;
float x, y;

/* Enemy stuff */
unsigned int enemy_txt[1];
unsigned int enemy_btxt[1];

/* Components */
struct Enemy {
  float x;
  float y;
  int type;
  int state;
  int ticks;
  int hp;
};

struct Bullet {
  float x;
  float y;
  int w;
  int h;
  float xvel;
  float yvel;
  float xaccel;
  float yaccel;
  unsigned int txt;
  int type;
};

/* ECS Stuff */
static struct ECS_ctx ctx;

static struct ECS_q qe_update;
static struct ECS_q qe_draw;
static struct ECS_q qb_update;
static struct ECS_q qb_draw;

#define SLOWDOWN 16.0
static void update_player(void)
{
  int xin, yin; /* Need to match actual input with 'projected input' */
  xin = (float) CORE_input.mouse.x / CORE_foo.win_w * WIN_W;
  yin = (float) CORE_input.mouse.y / CORE_foo.win_h * WIN_H;

  x += ((xin - SHIP_SIZE / 2) - x) / SLOWDOWN;
  if (x < 0) x = 0;
  if (x + SHIP_SIZE > WIN_W) x = WIN_W - SHIP_SIZE;
  y += ((yin - SHIP_SIZE / 2) - y) / SLOWDOWN;
  if (y < 0) y = 0;
  if (y + SHIP_SIZE > WIN_H) y = WIN_H - SHIP_SIZE;

  ++ticks;
  ECS_e newe;

  if (ticks % 64 == 0) {
    newe = ECS_e_create();
    ECS_SET(newe, Bullet, x + 28, y + 32, 8, 32, 0, 0, 0, -.05, btxt, 0);
  }
}

static void e_update(struct ECS_qd *data, ECS_eint count)
{
  ECS_e newe;

  struct Enemy *e;

  e = ECS_ITER(data, Enemy);

  for (ECS_eint i = count - 1; i > 0; --i) {
    ++e[i].ticks;
    switch (e[i].type) {
    case 1:
      switch (e[i].state) {
        case 0:
          if (e[i].ticks % 256 == 0) {
            newe = ECS_e_create();
            ECS_SET(newe, Bullet, e[i].x + 16, e[i].y + 32, 32, 64, 0, 0, 0, .01, enemy_btxt[0], 1);
          }
          e[i].x += .5;
          if (e[i].x + 64 > WIN_W) e[i].state = 1;
          break;
        case 1:
          if (e[i].ticks % 256 == 0) {
            newe = ECS_e_create();
            ECS_SET(newe, Bullet, e[i].x + 16, e[i].y + 32, 32, 64, 0, 0, 0, .00001, enemy_btxt[0], 1);
          }
          e[i].x -= .5;
          if (e[i].x < 0) e[i].state = 0;
          break;
      }
      break;
    }
  }
}

static void b_update(struct ECS_qd *data, ECS_eint count)
{
  struct ECS_c *c;
  struct Bullet *b;

  c = ECS_ITER(data, ECS_c);
  b = ECS_ITER(data, Bullet);

  for (ECS_eint i = count - 1; i > 0; --i) {
    b[i].xvel += b[i].xaccel;
    b[i].yvel += b[i].yaccel;

    b[i].x += b[i].xvel;
    b[i].y += b[i].yvel;

    if (b[i].y > WIN_H || b[i].y + b[i].h < 0) {
      ECS_e_destroy(c[i].e);
    }

    /* Enemy bullets against player */
    if (b[i].type) {
      if (b[i].x < x + SHIP_SIZE && b[i].x + b[i].w > x &&
          b[i].y < y + SHIP_SIZE && b[i].y + b[i].h > y) {
        CORE_switch_state(lose_init);
      }
    }
    /* Player bullets */
    else {
      /* Need a better way to 'query' within a query */
      struct ECS_qi *cur;
      struct ECS_qd  qd;

      if (qe_update.tables.table == NULL) return;

      cur = &qe_update.tables;
      while (cur) {
        qd.table = cur->table;

        struct Enemy *e;
        e = ECS_ITER(&qd, Enemy);

        for (ECS_eint j = cur->table->count - 1; j > 0; --j) {
          if (b[i].x < e[j].x + SHIP_SIZE && b[i].x + b[i].w > e[j].x &&
              b[i].y < e[j].y + SHIP_SIZE && b[i].y + b[i].h > e[j].y) {
            ECS_e_destroy(c[i].e);
            if (!--e[j].hp) {
              CORE_switch_state(win_init);
            }
          }
        }

        cur = cur->next;
      }
    }
  }
}

void level_update(void)
{
  if (CORE_input.keys.f11 == 1) {
    CORE_window_fullscreen(CORE_wnd, 1);
  }

  if (CORE_input.keys.escape == 1) {
    CORE_window_fullscreen(CORE_wnd, 0);
  }

  ECS_iterate(&qe_update, e_update);
  ECS_iterate(&qb_update, b_update);

  update_player();
}

static void e_draw(struct ECS_qd *data, ECS_eint count)
{
  struct Enemy *e;

  e = ECS_ITER(data, Enemy);

  for (ECS_eint i = count - 1; i > 0; --i) {
    switch (e[i].type) {
      case 1:
        GFX_draw_img(enemy_txt[0], e[i].x, e[i].y, SHIP_SIZE, SHIP_SIZE);
        break;
    }
  }
}

static void b_draw(struct ECS_qd *data, ECS_eint count)
{
  struct Bullet *b;

  b = ECS_ITER(data, Bullet);

  for (ECS_eint i = count - 1; i > 0; --i) {
    GFX_draw_img(b[i].txt, b[i].x, b[i].y, b[i].w, b[i].h);
  }
}

void level_draw(void)
{
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  ECS_iterate(&qe_draw, e_draw);
  ECS_iterate(&qb_draw, b_draw);

  GFX_draw_img(txt, x, y, SHIP_SIZE, SHIP_SIZE);
}

void level_quit(void)
{
  GL_texture_destroy(txt);
  GL_texture_destroy(btxt);
  GL_texture_destroy(enemy_txt[0]);
  GL_texture_destroy(enemy_btxt[0]);

  ECS_q_quit(&qb_draw);
  ECS_q_quit(&qb_update);
  ECS_q_quit(&qe_draw);
  ECS_q_quit(&qe_update);
  ECS_set(NULL);
  ECS_quit(&ctx);
}

void level_init(void)
{
  CORE_foo.update_fn = level_update;
  CORE_foo.draw_fn = level_draw;
  CORE_foo.quit_fn = level_quit;

  /* GFX stuff */
  txt = GL_texture_create("res/space_catet/cat.png");
  btxt = GL_texture_create("res/space_catet/cat_bullet1.png");
  enemy_txt[0] = GL_texture_create("res/space_catet/monkey_1.png");
  enemy_btxt[0] = GL_texture_create("res/space_catet/banana.png");

  GFX_projection(0, WIN_W, WIN_H, 0, -1, 1);
  TXT_projection(0, WIN_W, WIN_H, 0, -1, 1);

  SND_bgm_set("res/space_catet/boss_theme.wav");
  SND_bgm_play();

  /* ECS init */
  if (ECS_init(&ctx)) return;
  ECS_set(&ctx);

  ECS_C(Enemy);
  ECS_C(Bullet);

  /* Create the needed tables */
  ECS_e e;
  e = ECS_e_create();
  ECS_SET(e, Enemy, 0);
  ECS_e_destroy(e);

  e = ECS_e_create();
  ECS_SET(e, Bullet, 0);
  ECS_e_destroy(e);

  ECS_q_init(&qe_update, "<|Enemy>");
  ECS_q_init(&qe_draw,   "<|Enemy>");
  ECS_q_init(&qb_update, "<|Bullet>");
  ECS_q_init(&qb_draw,   "<|Bullet>");

  /* Test enemy */
  e = ECS_e_create();
  ECS_SET(e, Enemy, 0, 0, 1, 0, 0, 5);
}