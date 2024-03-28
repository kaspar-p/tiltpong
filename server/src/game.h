#ifndef _GAME_H_
#define _GAME_H_

#include <stddef.h>

typedef enum {
  PADDLE_LEFT = 0,
  PADDLE_RIGHT = 1,
} side_e;

typedef struct {
  side_e side;
  int score;

  double w;
  double h;

  double angle;

  // center-rectangle
  double pos_x;
  double pos_y;

  double vel_y;
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

typedef struct {
  char* buf;
  size_t buflen;
} game_buffer_t;

#define MAX_GAME_BUFFER_SIZE 1024
extern game_buffer_t* GAME_BUFFER;

void tiltpong_game_reset();

int tiltpong_game_start(void);

#endif /* _GAME_H_ */
