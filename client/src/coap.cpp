// reference: https://os.mbed.com/teams/sandbox/code/coap-example/docs/tip/main_8cpp_source.html
#include "coap.h"

UDPSocket socket;
// Thread recvFromThread;

struct coap_s *coapHandle;
coap_version_e coapVersion = COAP_VERSION_1;

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

void recvfromMain() {
  SocketAddress addr;
  uint8_t *recv_buffer = (uint8_t *)malloc(
      1280); // Suggested is to keep packet size under 1280 bytes

  nsapi_size_or_error_t ret;

  while ((ret = socket.recvfrom(&addr, recv_buffer, 1280)) >= 0) {
    // to see where the message came from, inspect addr.get_addr() and
    // addr.get_port()

    printf("Received a message of length '%d'\n", ret);

    sn_coap_hdr_s *parsed =
        sn_coap_parser(coapHandle, ret, recv_buffer, &coapVersion);

    // We know the payload is going to be a string
    std::string payload((const char *)parsed->payload_ptr, parsed->payload_len);

    printf("\tmsg_id:           %d\n", parsed->msg_id);
    printf("\tmsg_code:         %d\n", parsed->msg_code);
    printf("\tcontent_format:   %d\n", parsed->content_format);
    printf("\tpayload_len:      %d\n", parsed->payload_len);
    printf("\tpayload:          %s\n", payload.c_str());
    printf("\toptions_list_ptr: %p\n", parsed->options_list_ptr);
  }

  free(recv_buffer);

  printf("UDPSocket::recvfrom failed, error code %d. Shutting down receive "
         "thread.\n",
         ret);
}

int coap_init() { 
  NetworkInterface *network = WiFiInterface::get_default_instance();
  if (!network) {
    printf("Cannot connect to the network, see serial output\n");
    return 1;
  }

  printf("Connected to the network, opening socket\n");

  socket.open(network);

  coapHandle = sn_coap_protocol_init(&coap_malloc, &coap_free, &coap_tx_cb, &coap_rx_cb);

  // recvFromThread.start(&recvfromMain);

  const char* coap_uri_path = "/update";

  sn_coap_hdr_s *coap_res_ptr = (sn_coap_hdr_s*)calloc(sizeof(sn_coap_hdr_s), 1);
  coap_res_ptr->uri_path_ptr = (uint8_t *)coap_uri_path;
  coap_res_ptr->uri_path_len = strlen(coap_uri_path);
  coap_res_ptr->msg_code = COAP_MSG_CODE_REQUEST_GET;
  coap_res_ptr->content_format = COAP_CT_TEXT_PLAIN;
  coap_res_ptr->payload_len = 0;
  coap_res_ptr->payload_ptr = 0;
  coap_res_ptr->options_list_ptr = 0;
  coap_res_ptr->msg_id = 7;

  uint16_t message_len = sn_coap_builder_calc_needed_packet_data_size(coap_res_ptr);
  printf("Message length: %d bytes\n", message_len);

  uint8_t* message_ptr = (uint8_t*)malloc(message_len);
  sn_coap_builder(message_ptr, coap_res_ptr);

  SocketAddress sock_addr("::1", 5683);
  int scount = socket.sendto(sock_addr, message_ptr, message_len);
  printf("sent %d bytes to ::1:5683\n", message_len);

  free(coap_res_ptr);
  free(message_ptr);

  return 0;
}
