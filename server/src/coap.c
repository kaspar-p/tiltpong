#include <arpa/inet.h>
#include <coap3/coap.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "game.h"

char *interface = "en0";

int resolve_address(const char *host, const char *service,
                    coap_address_t *dst) {
  struct addrinfo *res, *ainfo;
  struct addrinfo hints;
  int error, len = -1;

  memset(&hints, 0, sizeof(hints));
  memset(dst, 0, sizeof(*dst));
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_family = AF_INET;

  error = getaddrinfo(host, service, &hints, &res);

  if (error != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error));
    return error;
  }

  for (ainfo = res; ainfo != NULL; ainfo = ainfo->ai_next) {
    switch (ainfo->ai_family) {
      case AF_INET6:
      case AF_INET:
        len = dst->size = ainfo->ai_addrlen;
        memcpy(&dst->addr.sin6, ainfo->ai_addr, dst->size);
        goto finish;
      default:;
    }
  }

finish:
  freeaddrinfo(res);
  return len;
}

/**
 * @brief The handler for /update
 *
 * Type definitions from
 * https://libcoap.net/doc/reference/4.3.4/coap__resource_8h_source.html
 */
void handler_update(coap_resource_t *resource, coap_session_t *session,
                    const coap_pdu_t *request, const coap_string_t *query,
                    coap_pdu_t *response) {
  // coap_log_crit("Got UPDATE!\n");
  coap_pdu_set_code(response, COAP_RESPONSE_CODE_CONTENT);
  // const char *message = "Hello, world!";
  // coap_add_data(response, strlen(message), (const uint8_t *)message);
  coap_show_pdu(COAP_LOG_WARN, response);

  size_t len;
  const uint8_t *temp_buf;
  coap_get_data(request, &len, &temp_buf);

  char buf[len];
  strncpy(buf, (char *)temp_buf, len);

  printf("%zu: '%s'\n", len, buf);
  fflush(stdout);

  int uid = 1;
  int moving_up = 0;
  double velocity = 0;
  double tilt = 0;
  int ret =
      sscanf(buf, "{\"uid\":%d,\"moving_up\":%d,\"velocity\":%lf,\"tilt\":%lf}",
             &uid, (int *)&moving_up, &velocity, &tilt);
  assert(ret == 4);

  paddle_pos_t *pos = NULL;
  if (uid == 1) {
    pos = left_pos;
  } else {
    pos = right_pos;
  }
  printf("uid: %d, moving_up: %d, velocity: %f, tilt: %f\n", uid, moving_up,
         velocity, tilt);

  assert(pos != NULL);
  pos->vel_y = moving_up ? velocity : -velocity;
  pos->angle = tilt * (180.0f / 3.1415926f);

  printf("pos: %f, %f\n", pos->vel_y, pos->angle);
}

/**
 * @brief The handler for /ready
 *
 * Type definitions from
 * https://libcoap.net/doc/reference/4.3.4/coap__resource_8h_source.html
 */
void handler_ready(coap_resource_t *resource, coap_session_t *session,
                   const coap_pdu_t *request, const coap_string_t *query,
                   coap_pdu_t *response) {
  coap_log_crit("Got READY!\n");
  coap_pdu_set_code(response, COAP_RESPONSE_CODE_CONTENT);
  const char *message = "Received Ready!";
  coap_add_data(response, strlen(message), (const uint8_t *)message);
  coap_show_pdu(COAP_LOG_WARN, response);
}

void handler_init_connection(coap_resource_t *resource, coap_session_t *session,
                             const coap_pdu_t *request,
                             const coap_string_t *query, coap_pdu_t *response) {
  coap_log_crit("Got INIT!\n");
  coap_pdu_set_code(response, COAP_RESPONSE_CODE_CONTENT);
  // const char *message = "Received Ready!";
  // coap_add_data(response, strlen(message), (const uint8_t *)message);
  coap_show_pdu(COAP_LOG_WARN, response);
}

void tiltpong_coap_free(coap_context_t *ctx) {
  coap_free_context(ctx);
  coap_cleanup();
}

coap_context_t *tiltpong_coap_init(void) {
  coap_address_t dst;

  coap_startup();

  /* Set logging level */
  coap_set_log_level(COAP_LOG_DEBUG);

  struct ifaddrs *ifap, *ifa;
  struct sockaddr_in *sa;
  char *addr;

  getifaddrs(&ifap);
  for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
    if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET) {
      if (strcmp(ifa->ifa_name, interface) == 0) {
        sa = (struct sockaddr_in *)ifa->ifa_addr;
        addr = inet_ntoa(sa->sin_addr);
        printf("IP: %s (%s)\n", addr, ifa->ifa_name);
      }
    }
  }

  freeifaddrs(ifap);

  coap_context_t *ctx = NULL;
  if (resolve_address(addr, "5683", &dst) < 0) {
    coap_log_crit("Failed to resolve address\n");
    return NULL;
  }

  ctx = coap_new_context(NULL);
  if (!ctx || !(coap_new_endpoint(ctx, &dst, COAP_PROTO_UDP))) {
    coap_log_emerg("cannot initialize context!\n");
    return NULL;
  }

  coap_str_const_t *update_uri = coap_make_str_const("update");
  coap_resource_t *update_resource = coap_resource_init(update_uri, 0);
  coap_register_handler(update_resource, COAP_REQUEST_POST, &handler_update);
  coap_add_resource(ctx, update_resource);

  coap_str_const_t *ready_uri = coap_make_str_const("ready");
  coap_resource_t *ready_resource = coap_resource_init(ready_uri, 0);
  coap_register_handler(ready_resource, COAP_REQUEST_POST, &handler_ready);
  coap_add_resource(ctx, ready_resource);

  coap_str_const_t *init_uri = coap_make_str_const("init");
  coap_resource_t *init_resource = coap_resource_init(init_uri, 0);
  coap_register_handler(init_resource, COAP_REQUEST_POST,
                        &handler_init_connection);
  coap_add_resource(ctx, init_resource);

  return ctx;
}

int tiltpong_coap_listen(coap_context_t *ctx) {
  while (true) {
    coap_io_process(ctx, COAP_IO_WAIT);
  }

  printf("COAP: Exiting!\n");
  return 1;
}
