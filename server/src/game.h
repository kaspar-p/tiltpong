#ifndef _GAME_H_
#define _GAME_H_

#include <stddef.h>

typedef struct {
  double angle;
  double vel_y;
} paddle_pos_t;

extern paddle_pos_t* left_pos;
extern paddle_pos_t* right_pos;

typedef struct {
  char* buf;
  size_t buflen;
} game_buffer_t;

#define MAX_GAME_BUFFER_SIZE 1024
extern game_buffer_t* GAME_BUFFER;

void tiltpong_game_reset();

int tiltpong_game_start(void);

#endif /* _GAME_H_ */
