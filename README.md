# tiltpong

A pong game, playable by moving and tilting microcontrollers. By Kaspar Poland,
Hisbaan Noorani, and Mac Niu.

## Building

This project uses third-party code via Git submodules. To initialize them, run:

```sh
git submodule update --init
```

Then run `make` from the top-level, and everything should build!

## Running

There are three components, the game, game server, and controllers.

### Game

The game is an HTML/Javascript webpage. It is built using Typescript, and run
using the `live-server` npm package. Just:

```bash
cd game
npm ci
npm run build
```

To continue development, keep a terminal tab open with `npm run watch` running,
which will rebuild main.ts when it changes. In another terminal tab, run
`npm run server`, which starts a port 8080 localhost webserver.

### Server

The server is a C server that has 3 main threads: a CoAP server to listen to
microcontroller connections and signals, a websocket server to send the
serialized game representation to the webpage, and the game simulation thread.

To start the server, simply `make` and run the `tiltpongsrv` executable that's
generated.

### Controllers

Microcontrollers. IDK.
