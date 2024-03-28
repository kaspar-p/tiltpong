#include "game.h"

#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

extern int interrupted;
double dt = 0.5;
game_t *game;

void game_score(game_t* game, side_e side) {
  if (side == PADDLE_LEFT) {
    game->left.score += 1;
  } else {
    game->right.score += 1;
  }

  game->ball.pos_x = game->width / 2;
  game->ball.pos_y = game->height / 2;
  game->ball.vel_x = 1;
  game->ball.vel_y = 0;

  game->left.pos_y = game->height / 2;
  game->left.vel_y = 0;

  game->right.pos_y = game->height / 2;
  game->right.vel_y = 0;
}

void check_paddle_collisions(double width, paddle_t* paddle, ball_t* ball) {
  bool in_y_range = ball->pos_y + ball->radius >= paddle->pos_y &&
                    ball->pos_y - ball->radius <= paddle->pos_y + paddle->h;
  bool in_x_range = ball->pos_x + ball->radius >= paddle->pos_x &&
                    ball->pos_x - ball->radius <= paddle->pos_x + paddle->w;
  bool hit = in_y_range && in_x_range;
  if (!hit) return;

  ball->vel_x *= -1;
}

void game_tick(game_t* game) {
  // Update positions
  game->ball.pos_x += dt * game->ball.vel_x;
  game->ball.pos_y += dt * game->ball.vel_y;

  // Score bounds checking
  if (game->ball.pos_x >= (game->width - game->ball.radius)) {
    game_score(game, PADDLE_LEFT);
    return;
  }

  if (game->ball.pos_x <= game->ball.radius) {
    game_score(game, PADDLE_RIGHT);
    return;
  }

  check_paddle_collisions(game->width, &game->left, &game->ball);
  check_paddle_collisions(game->width, &game->right, &game->ball);

  // Hits the top or bottom
  if (game->ball.pos_y >= game->height || game->ball.pos_y <= 0) {
    game->ball.vel_y *= -1;
  }
}

game_t* game_init(double height, double width) {
  int offset = width * 0.1;
  paddle_t left = {
      .side = PADDLE_LEFT,
      .pos_x = offset,
      .pos_y = height / 2,
      .score = 0,
      .vel_y = 0,
      .angle = 90,
      .w = 5,
      .h = 25,
  };

  paddle_t right = {
      .side = PADDLE_RIGHT,
      .pos_x = width - offset,
      .pos_y = height / 2,
      .score = 0,
      .vel_y = 0,
      .angle = 90,
      .w = 5,
      .h = 25,
  };

  ball_t ball = {
      .pos_x = width / 2,
      .pos_y = height / 2,
      .vel_x = 1,
      .vel_y = 0,
      .radius = 5,
  };

  game_t* game = malloc(sizeof(game_t));
  assert(game);
  game->left = left;
  game->right = right;
  game->ball = ball;
  game->width = width;
  game->height = height;

  return game;
}

void game_debug_print(game_t* game) {
  printf(
      "L(s=%d, x=%.2f, y=%.2f),  B(x=%.2f, y=%.2f),  R(s=%d, x=%.2f, y=%.2f)\n",
      game->left.score, game->left.pos_x, game->left.pos_y, game->ball.pos_x,
      game->ball.pos_y, game->right.score, game->right.pos_x,
      game->right.pos_y);
}

int msleep(long msec) {
  struct timespec ts;
  int res;

  if (msec < 0) {
    errno = EINVAL;
    return -1;
  }

  ts.tv_sec = msec / 1000;
  ts.tv_nsec = (msec % 1000) * 1000000;

  do {
    res = nanosleep(&ts, &ts);
  } while (res && errno == EINTR);

  return res;
}

typedef struct {
  char* buf;
  size_t buflen;
} game_buffer_t;

game_buffer_t game_create_serialization(game_t* game) {
  game_buffer_t buf;
  buf.buf = malloc(1024);
  assert(buf.buf);

  snprintf(buf.buf, 1024,
           "{"
           "\"width\":%d,"
           "\"height\":%d,"
           "\"ball\":{\"x\":%d,\"y\":%d},"
           "\"left\":{\"score\":%d,\"x\":%d,\"angle\":%.3f,\"y\":%d},"
           "\"right\":{\"score\":%d,\"x\":%d,\"angle\":%.3f,\"y\":%d}"
           "}",
           game->width, game->height, (int)game->ball.pos_x,
           (int)game->ball.pos_y, game->left.score, (int)game->left.pos_x,
           game->left.angle, (int)game->left.pos_y, game->right.score,
           (int)game->right.pos_x, game->right.angle, (int)game->right.pos_y);
  buf.buflen = strlen(buf.buf);

  printf("GAME:\n");
  printf("%s\n", buf.buf);

  return buf;
}

void game_free_serialization(game_buffer_t* buf) { free(buf->buf); }

int game_start(void) {
  game_t* g = game_init(400, 400);

  while (!interrupted) {
    game_tick(g);
    game_create_serialization(g);
    msleep(10);
  }

  return 0;
}
