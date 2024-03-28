#include <coap3/coap.h>

coap_context_t* coap_init(void);

void coap_free_(coap_context_t *ctx);

void coap_listen(coap_context_t *ctx);
