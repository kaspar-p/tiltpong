#include <coap3/coap.h>

coap_context_t *tiltpong_coap_init(void);

void tiltpong_coap_free(coap_context_t *ctx);

int tiltpong_coap_listen(coap_context_t *ctx);
