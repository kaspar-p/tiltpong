/*
 * lws-minimal-ws-server-echo
 *
 * Written in 2010-2019 by Andy Green <andy@warmcat.com>
 *
 * This file is made available under the Creative Commons CC0 1.0
 * Universal Public Domain Dedication.
 *
 * This demonstrates a ws server that echoes back what it was sent, in a way
 * compatible with autobahn -m fuzzingclient
 */

#include "websocket.h"

#include <libwebsockets.h>
#include <signal.h>
#include <string.h>

#include "tiltpong.h"

static struct lws_protocols protocols[] = {
    LWS_PLUGIN_PROTOCOL_MINIMAL_SERVER_ECHO, LWS_PROTOCOL_SEND_GAME_DATA,
    LWS_PROTOCOL_LIST_TERM};

static int port = 7681, options;

/* pass pointers to shared vars to the protocol */

static const struct lws_protocol_vhost_options pvo_options = {
    NULL, NULL, "options", /* pvo name */
    (void *)&options       /* pvo value */
};

static const struct lws_protocol_vhost_options pvo_interrupted = {
    &pvo_options, NULL, "interrupted", /* pvo name */
    (void *)&interrupted               /* pvo value */
};

static const struct lws_protocol_vhost_options pvo = {
    NULL,                      /* "next" pvo linked-list */
    &pvo_interrupted,          /* "child" pvo linked-list */
    "lws-minimal-server-echo", /* protocol name we belong to on this vhost */
    ""                         /* ignored */
};

int websocket_start() {
  struct lws_context_creation_info info;
  struct lws_context *context;
  int n = 0, logs = LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE
      /* for LLL_ verbosity above NOTICE to be built into lws,
       * lws must have been configured and built with
       * -DCMAKE_BUILD_TYPE=DEBUG instead of =RELEASE */
      /* | LLL_INFO */ /* | LLL_PARSER */ /* | LLL_HEADER */
      /* | LLL_EXT */ /* | LLL_CLIENT */  /* | LLL_LATENCY */
      /* | LLL_DEBUG */;

  lws_set_log_level(logs, NULL);
  lwsl_user(
      "LWS minimal ws client echo + permessage-deflate + multifragment bulk "
      "message\n");
  lwsl_user(
      "   lws-minimal-ws-client-echo [-n (no exts)] [-p port] [-o (once)]\n");

  memset(&info, 0, sizeof info); /* otherwise uninitialized garbage */
  info.port = port;
  info.protocols = protocols;
  info.pvo = &pvo;
  info.pt_serv_buf_size = 32 * 1024;
  info.options = LWS_SERVER_OPTION_VALIDATE_UTF8 |
                 LWS_SERVER_OPTION_HTTP_HEADERS_SECURITY_BEST_PRACTICES_ENFORCE;

  context = lws_create_context(&info);
  if (!context) {
    lwsl_err("lws init failed\n");
    return 1;
  }

  while (n >= 0 && !interrupted) n = lws_service(context, 0);

  lws_context_destroy(context);

  lwsl_user("Completed %s\n", interrupted == 2 ? "OK" : "failed");

  return interrupted != 2;
}
