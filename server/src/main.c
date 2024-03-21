#include <assert.h>
#include <coap3/coap.h>
#include <pthread.h>
#include <stdio.h>

void *start_thread_ws(void *arg) {
  printf("Working! %d\n", ((int *)arg)[0]);
  return "hi!\n";
}

int main() {
  int status = 0;
  pthread_t *thread_ws = malloc(sizeof(pthread_t));
  assert(thread_ws);
  pthread_attr_t thread_ws_attr;

  status = pthread_attr_init(&thread_ws_attr);
  assert(status == 0);
  int arg = 17;
  pthread_create(thread_ws, &thread_ws_attr, &start_thread_ws, &arg);

  pthread_t *thread_coap = malloc(sizeof(pthread_t));
  assert(thread_ws);
  pthread_attr_t thread_coap_attr;

  void *result;
  status = pthread_join(*thread_ws, &result);
  assert(status == 0);

  printf("Returned %s", (char *)result);

  return status;
}
