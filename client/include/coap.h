#include "sn_coap_header.h"
#include "sn_coap_protocol.h"
#include "NetworkInterface.h"
#include <string>
#include <UDPSocket.h>
#include "ISM43362Interface.h"
#include "WiFiAccessPoint.h"
#include "WiFiInterface.h"

void *coap_malloc(uint16_t size);

void coap_free(void *addr);

uint8_t coap_tx_cb(uint8_t *a, uint16_t b, sn_nsdl_addr_s *c, void *d);

int8_t coap_rx_cb(sn_coap_hdr_s *a, sn_nsdl_addr_s *b, void *c);

void coap_ready();

void coap_init();

void coap_send(double velocity, double tilt);
