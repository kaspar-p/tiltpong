#include <assert.h>
#include <coap3/coap.h>
#include <pthread.h>
#include <stdio.h>

#include "game.h"
#include "tiltpong.h"
#include "websocket.h"

void *start_thread_ws(void *arg) {
  printf("Starting websocket server! %d\n", ((int *)arg)[0]);

  websocket_start();

  return "done";
}

void *start_thread_game(void *arg) {
  printf("Starting game! %d\n", ((int *)arg)[0]);

  game_start();  // never exits

  return "done";
}

void *start_thread_coap(void *arg) {
  printf("Starting coap server! %d\n", ((int *)arg)[0]);
  return "done";
}

void sigint_handler(int sig) { interrupted = 1; }

pthread_t *thread_create(void *(entry)(void *)) {
  int status = 0;
  pthread_t *thread = malloc(sizeof(pthread_t));
  assert(thread);
  pthread_attr_t thread_attr;
  status = pthread_attr_init(&thread_attr);
  assert(status == 0);

  int arg = 10;
  status = pthread_create(thread, &thread_attr, entry, &arg);
  assert(status == 0);

  return thread;
}

int main() {
  int status = 0;

  signal(SIGINT, sigint_handler);

  pthread_t *thread_ws = thread_create(&start_thread_ws);
  pthread_t *thread_game = thread_create(&start_thread_game);
  pthread_t *thread_coap = thread_create(&start_thread_coap);

  void *result;
  assert(pthread_join(*thread_ws, &result) == 0);
  assert(pthread_join(*thread_game, &result) == 0);
  assert(pthread_join(*thread_coap, &result) == 0);
  printf("Returned %s", (char *)result);

  return status;
}
