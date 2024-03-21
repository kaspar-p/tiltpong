#ifndef _WS_H
#define _WS_H

#define LWS_DLL
#define LWS_INTERNAL

#include <libwebsockets.h>
#include <string.h>

struct per_session_data__minimal_server_echo {
  struct lws_ring *ring;
  uint32_t msglen;
  uint32_t tail;
  uint8_t completed : 1;
  uint8_t flow_controlled : 1;
  uint8_t write_consume_pending : 1;
};

int callback_minimal_server_echo(struct lws *wsi,
                                 enum lws_callback_reasons reason, void *user,
                                 void *in, size_t len);

#define LWS_PLUGIN_PROTOCOL_MINIMAL_SERVER_ECHO                                \
  {                                                                            \
    "lws-minimal-server-echo", callback_minimal_server_echo,                   \
        sizeof(struct per_session_data__minimal_server_echo), 1024, 0, NULL, 0 \
  }
#endif
