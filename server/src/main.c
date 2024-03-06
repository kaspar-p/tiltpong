#include <coap3/coap.h>
#include <netdb.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

int resolve_address(const char *host, const char *service,
                    coap_address_t *dst) {
  struct addrinfo *res, *ainfo;
  struct addrinfo hints;
  int error, len = -1;

  memset(&hints, 0, sizeof(hints));
  memset(dst, 0, sizeof(*dst));
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_family = AF_UNSPEC;

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
  coap_log_crit("Got UPDATE!\n");
  coap_pdu_set_code(response, COAP_RESPONSE_CODE_CONTENT);
  coap_add_data(response, 14, (const uint8_t *)"Hello, world!");
  coap_show_pdu(COAP_LOG_WARN, response);
}

int main() {
  coap_address_t dst;
  int result = EXIT_FAILURE;

  coap_startup();

  /* Set logging level */
  coap_set_log_level(COAP_LOG_DEBUG);

  coap_context_t *ctx = NULL;
  if (resolve_address("localhost", "5683", &dst) < 0) {
    coap_log_crit("Failed to resolve address\n");
    goto finish;
  }

  ctx = coap_new_context(NULL);
  if (!ctx || !(coap_new_endpoint(ctx, &dst, COAP_PROTO_UDP))) {
    coap_log_emerg("cannot initialize context!\n");
    goto finish;
  }

  coap_str_const_t *resource_uri = coap_make_str_const("update");
  coap_resource_t *resource = coap_resource_init(resource_uri, 0);
  coap_register_handler(resource, COAP_REQUEST_GET, &handler_update);
  coap_add_resource(ctx, resource);

  while (true) {
    coap_io_process(ctx, COAP_IO_WAIT);
  }

  result = EXIT_SUCCESS;
finish:
  coap_free_context(ctx);
  coap_cleanup();

  return result;
}
