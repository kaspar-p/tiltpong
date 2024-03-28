#include <assert.h>
#include <coap3/coap.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>

#include "coap.h"
#include "game.h"
#include "websocket.h"

void *start_thread_ws(void *arg) {
  printf("Starting websocket server! %d\n", ((int *)arg)[0]);

  int ret = tiltpong_websocket_start();
  printf("WEBSOCKET: Exited with %d\n", ret);

  return "done";
}

void *start_thread_game(void *arg) {
  printf("Starting game! %d\n", ((int *)arg)[0]);

  int ret = tiltpong_game_start();
  printf("GAME: Exited with %d\n", ret);

  return "done";
}

void *start_thread_coap(void *arg) {
  printf("Starting coap server! %d\n", ((int *)arg)[0]);

  coap_context_t *ctx = tiltpong_coap_init();
  int ret = tiltpong_coap_listen(ctx);
  printf("COAP: Exited with %d\n", ret);

  return "done";
}

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

int main(void) {
  int status = 0;

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
