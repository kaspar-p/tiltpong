// reference: https://os.mbed.com/teams/sandbox/code/coap-example/docs/tip/main_8cpp_source.html
#include "coap.h"

UDPSocket socket;
Thread recvfromThread;

struct coap_s *coapHandle;
coap_version_e coapVersion = COAP_VERSION_1;
SocketAddress *sock_addr;
const char *SERVER_IP = "10.0.0.111";

extern "C" void mbed_mac_address(char *s);
uint8_t uid = 1;

void *coap_malloc(uint16_t size) { return malloc(size); }

void coap_free(void *addr) { free(addr); }

uint8_t coap_tx_cb(uint8_t *a, uint16_t b, sn_nsdl_addr_s *c, void *d) {
    printf("coap tx cb\n");
    return 0;
}

int8_t coap_rx_cb(sn_coap_hdr_s *a, sn_nsdl_addr_s *b, void *c) {
    printf("coap rx cb\n");
    return 0;
}

void coap_ready() {
  const char *coap_uri_path = "ready";
  auto payload = "{\"uid\":" + std::to_string(uid) + "}";
  const char *payload_cstr = payload.c_str();

  sn_coap_hdr_s *coap_res_ptr = (sn_coap_hdr_s *)calloc(sizeof(sn_coap_hdr_s), 1);
  coap_res_ptr->token_len = 0;
  // coap_res_ptr->coap_status = COAP_STATUS_OK;
  coap_res_ptr->msg_code = COAP_MSG_CODE_REQUEST_POST;
  coap_res_ptr->msg_type = COAP_MSG_TYPE_CONFIRMABLE;
  coap_res_ptr->content_format = COAP_CT_TEXT_PLAIN;
  coap_res_ptr->msg_id = 300;
  coap_res_ptr->uri_path_len = strlen(coap_uri_path);
  coap_res_ptr->payload_len = strlen(payload_cstr);
  coap_res_ptr->token_ptr = NULL;
  coap_res_ptr->uri_path_ptr = (uint8_t *)coap_uri_path;
  coap_res_ptr->payload_ptr = (uint8_t *)payload_cstr;
  coap_res_ptr->options_list_ptr = NULL;

  uint16_t message_len = sn_coap_builder_calc_needed_packet_data_size(coap_res_ptr);
  uint8_t *message_ptr = (uint8_t *)malloc(message_len);

  sn_coap_builder(message_ptr, coap_res_ptr);

  socket.send(message_ptr, message_len);

  free(coap_res_ptr);
  free(message_ptr);
}

void coap_init() {
  printf("UID: %d\n", uid);

  // Setup network
  printf("Initializing network interface... ");
  ISM43362Interface * network = new ISM43362Interface(PC_12, PC_11, PC_10, PE_0, PE_8, PE_1, PB_13);
  printf("Done\n");

  if (!network) {
    printf("Cannot connect to the network, see serial output\n");
    return;
  }

  network->set_as_default();
  network->set_blocking(true);

  printf("Connecting... ");
  network->connect("Noorani", "billykabacha", NSAPI_SECURITY_WPA2);
  printf("Done\n");

  // printf("ipv4: %s\n", network->get_ip_address());

  printf("Opening socket... ");
  socket.open(network);
  socket.set_blocking(true); // TODO test setting this to false with game performance
  printf("Done\n");

  // Setup CoAP
  coapHandle = sn_coap_protocol_init(&coap_malloc, &coap_free, &coap_tx_cb, &coap_rx_cb);

  // Setup CoAP init packet
  const char *coap_uri_path = "init";
  auto payload = "{\"uid\":" + std::to_string(uid) + "}";
  const char *payload_cstr = payload.c_str();

  sn_coap_hdr_s *coap_res_ptr = (sn_coap_hdr_s *)calloc(sizeof(sn_coap_hdr_s), 1);
  coap_res_ptr->token_len = 0;
  // coap_res_ptr->coap_status = COAP_STATUS_OK;
  coap_res_ptr->msg_code = COAP_MSG_CODE_REQUEST_POST;
  coap_res_ptr->msg_type = COAP_MSG_TYPE_CONFIRMABLE;
  coap_res_ptr->content_format = COAP_CT_TEXT_PLAIN;
  coap_res_ptr->msg_id = 300;
  coap_res_ptr->uri_path_len = strlen(coap_uri_path);
  coap_res_ptr->payload_len = strlen(payload_cstr);
  coap_res_ptr->token_ptr = NULL;
  coap_res_ptr->uri_path_ptr = (uint8_t *)coap_uri_path;
  coap_res_ptr->payload_ptr = (uint8_t *)payload_cstr;
  coap_res_ptr->options_list_ptr = NULL;

  uint16_t message_len = sn_coap_builder_calc_needed_packet_data_size(coap_res_ptr);
  uint8_t *message_ptr = (uint8_t *)malloc(message_len);

  sn_coap_builder(message_ptr, coap_res_ptr);

  sock_addr = new SocketAddress(SERVER_IP, 5683);
  socket.connect(*sock_addr);

  printf("Sending message... ");

  socket.send(message_ptr, message_len);
  printf("Done\n");

  free(coap_res_ptr);
  free(message_ptr);
}

void coap_send(double velocity, double tilt) {
  const char *coap_uri_path = "update";
  auto payload =
    "{\"uid\":" + std::to_string(uid)
    + ",\"moving_up\":" + (velocity > 0 ? "true" : "false")
    + ",\"velocity\":" + std::to_string(std::abs(velocity))
    + ",\"tilt\":" + std::to_string(tilt)
    + "}";
  const char *payload_cstr = payload.c_str();

  sn_coap_hdr_s *coap_res_ptr = (sn_coap_hdr_s *)calloc(sizeof(sn_coap_hdr_s), 1);
  coap_res_ptr->token_len = 0;
  // coap_res_ptr->coap_status = COAP_STATUS_OK;
  coap_res_ptr->msg_code = COAP_MSG_CODE_REQUEST_POST;
  coap_res_ptr->msg_type = COAP_MSG_TYPE_CONFIRMABLE;
  coap_res_ptr->content_format = COAP_CT_TEXT_PLAIN;
  coap_res_ptr->msg_id = 300;
  coap_res_ptr->uri_path_len = strlen(coap_uri_path);
  coap_res_ptr->payload_len = strlen(payload_cstr);
  coap_res_ptr->token_ptr = NULL;
  coap_res_ptr->uri_path_ptr = (uint8_t *)coap_uri_path;
  coap_res_ptr->payload_ptr = (uint8_t *)payload_cstr;
  coap_res_ptr->options_list_ptr = NULL;

  uint16_t message_len = sn_coap_builder_calc_needed_packet_data_size(coap_res_ptr);
  uint8_t *message_ptr = (uint8_t *)malloc(message_len);

  sn_coap_builder(message_ptr, coap_res_ptr);

  socket.send(message_ptr, message_len);

  free(coap_res_ptr);
  free(message_ptr);
}
