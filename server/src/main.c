#include <coap3/coap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  coap_context_t *ctx = NULL;
  coap_session_t *session = NULL;
  coap_address_t dst;
  coap_pdu_t *pdu = NULL;
  int result = EXIT_FAILURE;

  coap_startup();

  /* Set logging level */
  coap_set_log_level(COAP_LOG_WARN);

  coap_log_crit("failed to resolve address, exiting!\n");

  return 1;
}
