#include <coap3/coap.h>

coap_context_t* coap_init();

void coap_free(coap_context_t *ctx);

void coap_listen(coap_context_t *ctx);
