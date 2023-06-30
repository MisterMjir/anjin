#include "core/core.h"
#include "ecs/ecs.h"
#include "log/log.h"
#include "gfx/gfx.h"
#include "gl/gl_util.h"

#define WIN_W 800.0
#define WIN_H 600.0

struct Position {
  float x;
  float y;
};

struct Sprite {
  unsigned int t;
  int          w;
  int          h;
};

struct Physics {
  float xvel;
  float yvel;
};

static struct ECS_ctx ctx;

static struct ECS_q q_draw;
static struct ECS_q q_phys;
static unsigned int texture;

static void draw(struct ECS_qd *data, ECS_eint count)
{
  struct Position *pos;
  struct Sprite   *spr;

  pos = ECS_ITER(data, Position);
  spr = ECS_ITER(data, Sprite);

  for (ECS_eint i = count - 1; i > 0; --i) {
    GFX_draw_img(spr[i].t, pos[i].x, pos[i].y, spr[i].w, spr[i].h);
  }
}

#define INC 0.01
static void phys(struct ECS_qd* data, ECS_eint count)
{
  struct Position *pos;
  struct Physics  *phys;

  pos = ECS_ITER(data, Position);
  phys = ECS_ITER(data, Physics);

  for (ECS_eint i = count - 1; i > 0; --i) {
    pos[i].x += phys[i].xvel;
    pos[i].y += phys[i].yvel;

    if (pos[i].x <= 0) {
      pos[i].x = 0;
      phys[i].xvel *= -1;
      phys[i].xvel += INC;
      phys[i].yvel *= 1.1;
    }

    if (pos[i].x >= 640 - 64) {
      pos[i].x = 640 - 64;
      phys[i].xvel *= -1;
      phys[i].xvel -= INC;
      phys[i].yvel *= 1.1;
    }

    if (pos[i].y <= 0) {
      pos[i].y = 0;
      phys[i].yvel *= -1;
      phys[i].yvel += INC;
      phys[i].xvel *= 1.1;
    }

    if (pos[i].y >= 480 - 64) {
      pos[i].y = 480 - 64;
      phys[i].yvel *= -1;
      phys[i].yvel -= INC;
      phys[i].xvel *= 1.1;
    }
  }
}

void demo4_update(void)
{
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  ECS_iterate(&q_phys, phys);
}

void demo4_draw(void)
{
  

  ECS_iterate(&q_draw, draw);
}

void demo4_quit(void)
{
  ECS_q_quit(&q_draw);
  ECS_q_quit(&q_phys);
  ECS_set(NULL);
  ECS_quit(&ctx);
}

void demo4_init(void)
{
  CORE_foo.update_fn = demo4_update;
  CORE_foo.draw_fn = demo4_draw;
  CORE_foo.quit_fn = demo4_quit;

  texture = GL_texture_create("res/img/test_image.png");

  GFX_projection(0, 640, 480, 0, -1, 1);

  if (ECS_init(&ctx)) return;
  ECS_set(&ctx);

  ECS_C(Position);
  ECS_C(Sprite);
  ECS_C(Physics);

  ECS_e e;

  for (ECS_eint i = 0; i < 8; ++i) {
    e = ECS_e_create();
    ECS_SET(e, Position, 64 * i, 0);
    ECS_SET(e, Physics, INC, INC);
    ECS_SET(e, Sprite, texture, 64, 64);
  }

  ECS_q_init(&q_draw, "<|Position><|Sprite>");
  ECS_q_init(&q_phys, "<|Position><|Physics>");
}