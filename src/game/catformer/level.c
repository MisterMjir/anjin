#include "core/core.h"
#include "gfx/gfx.h"
#include "gl/gl_util.h"
#include "ecs/ecs.h"
#include "catformer.h"
#include "snd/snd.h"

#include <AL/alc.h>

// Ngl this code is horrendous, please don't make a platformer with code like this

int camera_x_target;
int camera_y_target;
int camera_x;
int camera_y;
int flag_timer;

unsigned int banana;

#define MAP_START_X 0
#define MAP_START_Y 0
#define MAP_COLS    40
#define MAP_ROWS    5
static int map[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 2, 2, 2, 2, 2, 2, 0, 0, 0, 1, 1, 2, 2, 2, 2, 0, 0, 2, 0, 2, 0, 2, 0, 0, 0, 2, 2, 2,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1,
};

struct Position {
  float x;
  float y;
};

struct Enemy {
  unsigned int ticks;
};

struct Banana {
  int yum;
};

struct Sprite {
  unsigned int t;
  int          w;
  int          h;
  int          x;
  int          y;
  int          tw;
  int          th;
};

static struct ECS_ctx ctx;
static struct ECS_q q_draw;
static struct ECS_q q_monkey;
static struct ECS_q q_banana;

static void draw(struct ECS_qd *data, ECS_eint count)
{
  struct Position *pos;
  struct Sprite   *spr;

  pos = ECS_ITER(data, Position);
  spr = ECS_ITER(data, Sprite);

  for (ECS_eint i = count - 1; i > 0; --i) {
    GFX_draw_imgx(spr[i].t, pos[i].x - camera_x, pos[i].y - camera_y, spr[i].w, spr[i].h, spr[i].x, spr[i].y, spr[i].tw, spr[i].th, 0, 0);
  }
}

static void monkey(struct ECS_qd *data, ECS_eint count)
{
  struct Position *pos;
  struct Enemy *enemy;

  pos = ECS_ITER(data, Position);
  enemy = ECS_ITER(data, Enemy);

  for (ECS_eint i = count - 1; i > 0; --i) {
    if (++enemy[i].ticks % 64 == 0) {
      ECS_e e = ECS_e_create();
      ECS_SET(e, Position, pos[i].x, pos[i].y);
      ECS_SET(e, Banana, 1);
      ECS_SET(e, Sprite, banana, 64, 32, 0, 0, 64, 32);
    }
  }
}

enum player_state {
  IDLE,
  WALKING,
  JUMPING
};

struct player {
  enum player_state state;
  unsigned int      texture;
  unsigned int      ticks;
  unsigned int      save_tick;
  unsigned int      sub_state;
  int               tex_x;
  int               tex_y;
  int               tex_w;
  int               tex_h;
  float             x;
  float             y;
  float             y_vel;
  int               w;
  int               h;
  int               dir;
  int               hit_x;
  int               hit_y;
  int               hit_w;
  int               hit_h;
  int               grounded;
};

static struct player player;

static void banana_update(struct ECS_qd *data, ECS_eint count)
{
  struct Position *pos;

  pos = ECS_ITER(data, Position);

  for (ECS_eint i = count - 1; i > 0; --i) {
    pos[i].x -= 4;
    if (pos[i].x < player.x + player.hit_x + player.hit_w &&
    pos[i].x + 32 > player.x + player.hit_x &&
    pos[i].y < player.y + player.hit_y + player.hit_h &&
    pos[i].y + 16 > player.y + player.hit_y) {
      CORE_switch_state(lose_init);
    }
  }
}

#define SET_IDLE player.state = IDLE; \
player.hit_x = 0; \
player.hit_y = 0; \
player.hit_w = 128; \
player.hit_h = 48; \
player.w = 128; \
player.h = 64; \
player.tex_x = 240; \
player.tex_y = 0; \
player.tex_w = 128; \
player.tex_h = 64;
#define SET_WALKING player.state = WALKING; \
player.hit_x = 0; \
player.hit_y = 0; \
player.hit_w = 80; \
player.hit_h = 120; \
player.w = 80; \
player.h = 128; \
player.tex_x = 0; \
player.tex_y = 0; \
player.tex_w = 80; \
player.tex_h = 128; \
player.sub_state = 0; \
player.ticks = -1;
#define SET_JUMPING player.state = JUMPING; \
player.hit_x = 0; \
player.hit_y = 0; \
player.hit_w = 128; \
player.hit_h = 48; \
player.w = 128; \
player.h = 96; \
player.tex_x = 368; \
player.tex_y = 0; \
player.tex_w = 128; \
player.tex_h = 96; \
player.grounded = 0;
void level_update(void)
{
  // Input/Physics/Animations
  player.y_vel += 0.5;

  switch (player.state) {
    case IDLE: {
      if (CORE_input.keys.a) {
        player.dir = 0;
        player.y -= 80;
        SET_WALKING;
      }
      if (CORE_input.keys.d) {
        player.dir = 1;
        player.y -= 80;
        SET_WALKING;
      }
      if (CORE_input.keys.w == 1) {
        player.y -= 32;
        player.y_vel = -8;
        SET_JUMPING;
      }
      break;
    }
    case WALKING: {
      ++player.ticks;

      if (CORE_input.keys.a || CORE_input.keys.d) {
        player.save_tick = 0;
        // Animation stuff
        if (player.ticks % 8 == 0) {
          ++player.sub_state;
          if (player.sub_state > 3) player.sub_state = 0;
          switch (player.sub_state) {
            case 0: player.tex_x = 0; break;
            case 1: player.tex_x = 80; break;
            case 2: player.tex_x = 0; break;
            case 3: player.tex_x = 160; break;
          }
        }
        if (CORE_input.keys.a) {
          player.dir = 0;
          player.x -= 4;
        }
        else {
          player.dir = 1;
          player.x += 4;
        }
      }
      else {
        player.tex_x = 0;
        if (++player.save_tick % 16 == 0) {
          player.y += 80;
          SET_IDLE;
        }
      }
      if (CORE_input.keys.w == 1) {
        player.y -= 32;
        player.y_vel = -8;
        SET_JUMPING;
      }
      break;
    }
    case JUMPING: {
      if (CORE_input.keys.a) {
        player.dir = 0;
        player.x -= 4;
      }
      if (CORE_input.keys.d) {
        player.dir = 1;
        player.x += 4;
      }
      if (player.grounded) {
        SET_IDLE;
        player.grounded = 0;
      }
      break;
    }
  }

  //player.x += player.x_vel;
  player.y += player.y_vel;

  // Player map collisions
  if (player.x + player.hit_x + player.hit_w >= MAP_START_X && player.x + player.hit_x < MAP_START_X + 64 * MAP_COLS &&
  player.y + player.hit_y + player.hit_h >= MAP_START_Y && player.y + player.hit_y < MAP_START_Y + 64 * MAP_ROWS) {
    int tile_x = (int) (player.x + player.hit_w) / 64;
    int tile_y = (int) (player.y + player.hit_h) / 64;

    if (map[tile_y * MAP_COLS + tile_x]) {
      player.y_vel = 0;
      player.y = tile_y * 64 - player.hit_h;
      player.grounded = 1;
    }

    if (tile_x == 40 && tile_y == 2) {
      ++flag_timer;
      if (flag_timer == 64) {
        CORE_switch_state(win_init);
      }
    } else {
      flag_timer = 0;
    }
  }

  camera_x_target = player.x - 128;
  camera_y_target = player.y - 160;

  camera_x += (camera_x_target - camera_x) / 4;
  camera_y += (camera_y_target - camera_y) / 4;

  // Fallen off the map
  if (player.y > camera_y + 640) { 
    CORE_switch_state(lose_init);
  }

  ECS_iterate(&q_monkey, monkey);
  ECS_iterate(&q_banana, banana_update);

  // DEBUG THINGY
  int error;
  if (CORE_input.keys.p == 1) {
    if ((error = alcGetError(NULL)) != ALC_NO_ERROR)
    {
      printf("OpenAL error: %d\n", error);
    } 
  }
}

void level_draw(void)
{
  glClearColor(0.0475, 0.709, 0.950, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  ECS_iterate(&q_draw, draw);

  GFX_draw_imgx(
    player.texture,
    player.x - camera_x,
    player.y - camera_y,
    player.w,
    player.h,
    player.tex_x,
    player.tex_y,
    player.tex_w,
    player.tex_h,
    player.dir,
    0);
}

void level_quit(void)
{
  // Should destroy textures here but it's fine (probably)
  ECS_q_quit(&q_draw);
  ECS_set(NULL);
  ECS_quit(&ctx);
}

void level_init(void)
{
  CORE_foo.update_fn = level_update;
  CORE_foo.draw_fn = level_draw;
  CORE_foo.quit_fn = level_quit;

  SET_IDLE;
  player.ticks = 0;
  player.sub_state = 0;
  player.x = 0;
  player.y = -128;
  player.y_vel = 0;
  player.dir = 1;

  camera_x_target = 0;
  camera_y_target = 0;
  camera_x = 0;
  camera_y = 0;

  player.texture = GL_texture_create("res/catformer/plsfrogiveme.png");
  unsigned int tmp = GL_texture_create("res/catformer/tiles.png");
  banana = GL_texture_create("res/catformer/banana.png");

  GFX_projection(0, 640, 480, 0, -1, 1);

  if (ECS_init(&ctx)) return;
  ECS_set(&ctx);

  ECS_C(Position);
  ECS_C(Sprite);
  ECS_C(Enemy);
  ECS_C(Banana);

  ECS_e e;
  int x = MAP_START_X;
  int y = MAP_START_Y;
  for (ECS_eint i = 0; i < sizeof(map) / sizeof(int);) {
    if (map[i]) {
      e = ECS_e_create();
      ECS_SET(e, Position, x, y);
      int ty;
      switch (map[i]) {
        case 1: ty = 0; break;
        case 2: ty = 32; break;
      }
      ECS_SET(e, Sprite, tmp, 64, 64, 0, ty, 32, 32); // TODO
    }
    x += 64;
    if (++i % MAP_COLS == 0) {
      x = MAP_START_X;
      y += 64;
    }
  }

  tmp = GL_texture_create("res/catformer/monkey.png");
  e = ECS_e_create();
  ECS_SET(e, Position, 128 * 3, 16);
  ECS_SET(e, Enemy, 0);
  ECS_SET(e, Sprite, tmp, 128, 128, 0, 0, 128, 128);

  e = ECS_e_create();
  ECS_SET(e, Position, 128 * 8, 16);
  ECS_SET(e, Enemy, 2);
  ECS_SET(e, Sprite, tmp, 128, 128, 0, 0, 128, 128);

  e = ECS_e_create();
  ECS_SET(e, Position, 128 * 18 + 64, 16);
  ECS_SET(e, Enemy, 4);
  ECS_SET(e, Sprite, tmp, 128, 128, 0, 0, 128, 128);

  tmp = GL_texture_create("res/catformer/flag.png");
  e = ECS_e_create();
  ECS_SET(e, Position, 128 * 19 + 64, 64);
  ECS_SET(e, Sprite, tmp, 64, 64, 0, 0, 64, 64);
  flag_timer = 0;

  e = ECS_e_create();
  ECS_SET(e, Position, 128 * 19 + 64, 64);
  ECS_SET(e, Sprite, tmp, 64, 64, 0, 0, 64, 64);
  ECS_SET(e, Banana, 1);

  ECS_q_init(&q_draw, "<|Position><|Sprite>");
  ECS_q_init(&q_monkey, "<|Position><|Enemy>");
  ECS_q_init(&q_banana, "<|Position><|Banana>");

  ECS_e_destroy(e);

  SND_bgm_set("res/catformer/level.wav");
  SND_bgm_play();
}