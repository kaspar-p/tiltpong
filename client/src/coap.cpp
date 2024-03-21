// reference: https://os.mbed.com/teams/sandbox/code/coap-example/docs/tip/main_8cpp_source.html
#include "coap.h"

UDPSocket socket;
Thread recvfromThread;

struct coap_s *coapHandle;
coap_version_e coapVersion = COAP_VERSION_1;
SocketAddress *sock_addr;
const char * SERVER_IP = "10.0.0.111";

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

  printf("UDPSocket::recvfrom failed, error code %d. Shutting down receive thread.\n", ret);
}

const char *sec2str(nsapi_security_t sec) {
    switch (sec) {
        case NSAPI_SECURITY_NONE:
            return "None";
        case NSAPI_SECURITY_WEP:
            return "WEP";
        case NSAPI_SECURITY_WPA:
            return "WPA";
        case NSAPI_SECURITY_WPA2:
            return "WPA2";
        case NSAPI_SECURITY_WPA_WPA2:
            return "WPA/WPA2";
        case NSAPI_SECURITY_UNKNOWN:
        default:
            return "Unknown";
    }
}

int coap_init() { 
  printf("Initializing network interface... ");
  ISM43362Interface * network = new ISM43362Interface(PC_12, PC_11, PC_10, PE_0, PE_8, PE_1, PB_13);
  printf("Done\n");

  if (!network) {
    printf("Cannot connect to the network, see serial output\n");
    return 1;
  }

  printf("Connecting... ");
  network->connect("Noorani", "billykabacha", NSAPI_SECURITY_WPA2);
  printf("Done\n");

  printf("ipv4: %s\n", network->get_ip_address());

  printf("Opening socket... ");
  socket.open(network);
  socket.set_blocking(true);
  printf("Done\n");

  /*
  SocketAddress addr("10.0.0.111", 8080);
  // network->gethostbyname("https://cloudflare.com/cdn-cgi/trace", &addr);

  std::string request;
  std::string buffer;

  request.append("GET")
  .append("/")
  .append("HTTP/1.1\r\n")
  .append("Host: ")
  .append("http://10.0.0.111:8080")
  .append("\r\n")
  .append("Connection: close\r\n\r\n");

  TCPSocket socket;
  socket.open(network);
  socket.connect(addr);
  socket.send(request.c_str(), request.length());

  printf("request sent\n");
  */

  coapHandle = sn_coap_protocol_init(&coap_malloc, &coap_free, &coap_tx_cb, &coap_rx_cb);

  const char *coap_uri_path = "/update";

  sn_coap_hdr_s *coap_res_ptr = (sn_coap_hdr_s *)calloc(sizeof(sn_coap_hdr_s), 1);
  coap_res_ptr->uri_path_ptr = (uint8_t *)coap_uri_path;
  coap_res_ptr->uri_path_len = strlen(coap_uri_path);
  coap_res_ptr->msg_code = COAP_MSG_CODE_REQUEST_GET;
  coap_res_ptr->content_format = COAP_CT_TEXT_PLAIN;
  coap_res_ptr->payload_len = 0;
  coap_res_ptr->payload_ptr = 0;
  coap_res_ptr->options_list_ptr = 0;
  coap_res_ptr->msg_id = 7;

  uint16_t message_len = sn_coap_builder_calc_needed_packet_data_size(coap_res_ptr);
  uint8_t *message_ptr = (uint8_t *)malloc(message_len);
  sn_coap_builder(message_ptr, coap_res_ptr);

  sock_addr = new SocketAddress(SERVER_IP, 5683);
  socket.connect(*sock_addr);
  
  printf("Sending message... ");

  int scount = socket.send(message_ptr, message_len);
  printf("Done\n");
  printf("Sent %d bytes to %s:5683\n\n", scount, SERVER_IP);

  free(coap_res_ptr);
  free(message_ptr);

  return 0;
}

void coap_send(std::array<double, 3> position) {
  std::string buffer;
  for (int i = 0; i < 3; i++) {
    buffer.append(std::to_string(position[i]));
    if (i != 2) buffer.append(",");
  }
  const char *buffer_cstr = buffer.c_str();
  const char *coap_uri_path = "/update";

  sn_coap_hdr_s *coap_res_ptr = (sn_coap_hdr_s *)calloc(sizeof(sn_coap_hdr_s), 1);
  coap_res_ptr->uri_path_ptr = (uint8_t *)coap_uri_path;
  coap_res_ptr->uri_path_len = strlen(coap_uri_path);
  coap_res_ptr->msg_code = COAP_MSG_CODE_REQUEST_GET;
  coap_res_ptr->content_format = COAP_CT_TEXT_PLAIN;
  coap_res_ptr->payload_len = strlen(buffer_cstr);
  coap_res_ptr->payload_ptr = (uint8_t*)buffer_cstr;
  coap_res_ptr->options_list_ptr = 0;
  coap_res_ptr->msg_id = 7;

  uint16_t message_len = sn_coap_builder_calc_needed_packet_data_size(coap_res_ptr);
  uint8_t *message_ptr = (uint8_t *)malloc(message_len);
  sn_coap_builder(message_ptr, coap_res_ptr);

  int scount = socket.send(message_ptr, message_len);
  printf("Sent %d bytes to %s:5683\n", scount, SERVER_IP);

  free(coap_res_ptr);
  free(message_ptr);
}