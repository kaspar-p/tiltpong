#include "game.h"

#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "utils.h"

typedef enum {
  PADDLE_LEFT = 0,
  PADDLE_RIGHT = 1,
} side_e;

typedef struct {
  side_e side;
  int score;

  double w;
  double h;

  // center-rectangle
  double pos_x;
  double pos_y;

  paddle_pos_t* pos;
} paddle_t;

typedef struct {
  double pos_x;
  double vel_x;

  double pos_y;
  double vel_y;

  double radius;
} ball_t;

typedef struct {
  int width, height;

  paddle_t left;
  paddle_t right;
  ball_t ball;
} game_t;

paddle_pos_t* left_pos = NULL;
paddle_pos_t* right_pos = NULL;

double width = 400, height = 400;

double dt = 0.5;

game_t* game;
game_buffer_t* GAME_BUFFER = NULL;

void game_score(game_t* game, side_e scoring_side) {
  if (scoring_side == PADDLE_LEFT) {
    game->left.score += 1;
  } else {
    game->right.score += 1;
  }

  int left_score = game->left.score;
  int right_score = game->right.score;

  if (game->left.score == 10) {
    printf(" ============================= \n");
    printf(" ====== LEFT PADDLE WON ====== \n");
    printf(" ============================= \n");
    tiltpong_game_reset();
    return;
  }

  if (game->right.score == 10) {
    printf(" ============================== \n");
    printf(" ====== RIGHT PADDLE WON ====== \n");
    printf(" ============================== \n");
    tiltpong_game_reset();
    return;
  }

  tiltpong_game_reset();
  game->left.score = left_score;
  game->right.score = right_score;
}

void check_paddle_collisions(double width, paddle_t* paddle, ball_t* ball) {
  bool in_y_range =
      ball->pos_y + ball->radius >= paddle->pos_y - paddle->h / 2 &&
      ball->pos_y - ball->radius <= paddle->pos_y + paddle->h / 2;
  bool in_x_range =
      ball->pos_x + ball->radius >= paddle->pos_x - paddle->w / 2 &&
      ball->pos_x - ball->radius <= paddle->pos_x + paddle->w / 2;
  bool hit = in_y_range && in_x_range;
  if (!hit) return;

  ball->vel_x *= -1;
}

long random_at_most(long max) {
  unsigned long
      // max <= RAND_MAX < ULONG_MAX, so this is okay.
      num_bins = (unsigned long)max + 1,
      num_rand = (unsigned long)RAND_MAX + 1, bin_size = num_rand / num_bins,
      defect = num_rand % num_bins;

  long x;
  do {
    x = random();
  }
  // This is carefully written not to overflow
  while (num_rand - defect <= (unsigned long)x);

  // Truncated division is intentional
  return x / bin_size;
}

double clamp(double d, double min, double max) {
  const double t = d < min ? min : d;
  return t > max ? max : t;
}

void game_tick(game_t* game) {
  // Update paddle positions
  game->left.pos_y += dt * game->left.pos->vel_y;
  game->left.pos_y = clamp(game->left.pos_y, 0, game->height);
  game->right.pos_y += dt * game->right.pos->vel_y;
  game->right.pos_y = clamp(game->right.pos_y, 0, game->height);

  // Update ball position
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

game_t* game_init() {
  game_t* game = malloc(sizeof(game_t));
  assert(game);

  left_pos = malloc(sizeof(paddle_pos_t));
  right_pos = malloc(sizeof(paddle_pos_t));

  return game;
}

void tiltpong_game_reset() {
  left_pos->angle = 0;
  left_pos->vel_y = 0;

  right_pos->angle = 0;
  right_pos->vel_y = 0;

  int offset = width * 0.1;
  paddle_t left = {
      .side = PADDLE_LEFT,
      .pos_x = offset,
      .pos_y = height / 2,
      .score = 0,
      .pos = left_pos,
      .w = 6,
      .h = 36,
  };

  paddle_t right = {
      .side = PADDLE_RIGHT,
      .pos_x = width - offset,
      .pos_y = 2 * (height / 3) + 10,
      .score = 0,
      .pos = right_pos,
      .w = 6,
      .h = 36,
  };

  ball_t ball = {
      .pos_x = width / 2,
      .pos_y = height / 2,
      .vel_x = 3,
      .vel_y = 2,
      .radius = 10,
  };

  assert(game);
  game->left = left;
  game->right = right;
  game->ball = ball;
  game->width = width;
  game->height = height;
}

void game_debug_print(game_t* game) {
  printf(
      "L(s=%d, x=%.2f, y=%.2f),  B(x=%.2f, y=%.2f),  R(s=%d, x=%.2f, y=%.2f)\n",
      game->left.score, game->left.pos_x, game->left.pos_y, game->ball.pos_x,
      game->ball.pos_y, game->right.score, game->right.pos_x,
      game->right.pos_y);
}

void serialize(game_t* game) {
  assert(game);
  assert(GAME_BUFFER);
  assert(GAME_BUFFER->buf);

  snprintf(GAME_BUFFER->buf, MAX_GAME_BUFFER_SIZE,
           "{"
           "\"width\":%d,"
           "\"height\":%d,"
           "\"ball\":{\"x\":%d,\"y\":%d,\"radius\":%d},"
           "\"left\":{\"score\":%d,\"x\":%d,\"angle\":%.3f,\"y\":%d,"
           "\"width\":%d,\"height\":%d},"
           "\"right\":{\"score\":%d,\"x\":%d,\"angle\":%.3f,\"y\":%d,"
           "\"width\":%d,\"height\":%d}"
           "}",
           game->width, game->height, (int)game->ball.pos_x,
           (int)game->ball.pos_y, (int)game->ball.radius, game->left.score,
           (int)game->left.pos_x, game->left.pos->angle, (int)game->left.pos_y,
           (int)game->left.w, (int)game->left.h, game->right.score,
           (int)game->right.pos_x, game->right.pos->angle,
           (int)game->right.pos_y, (int)game->right.w, (int)game->right.h);
  GAME_BUFFER->buflen = strlen(GAME_BUFFER->buf);
}

void free_serialization() {
  free(GAME_BUFFER->buf);
  free(GAME_BUFFER);
}

int tiltpong_game_start(void) {
  GAME_BUFFER = malloc(sizeof(game_buffer_t));
  assert(GAME_BUFFER);
  GAME_BUFFER->buf = malloc(MAX_GAME_BUFFER_SIZE);
  assert(GAME_BUFFER->buf);
  GAME_BUFFER->buflen = 0;

  game = game_init();
  tiltpong_game_reset();
  while (true) {
    game_tick(game);
    serialize(game);
    msleep(10);
  }

  free_serialization();

  printf("GAME: Exiting!\n");

  return 0;
}
