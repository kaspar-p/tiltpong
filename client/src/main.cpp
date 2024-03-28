#include "mbed.h"
#include "coap.h"
#include "sensors.h"

EventQueue queue;
InterruptIn button1(BUTTON1);

bool ready_sent = false;
double velocity = 0;
double tilt = 0;

void get_sensor_data() {
  velocity = get_velocity();
  tilt = get_tilt();
}

void send_sensor_data() {
  coap_send(velocity, tilt);
}

void button_fall_handler() {
  if (button1.read() == 0 && ready_sent == false) {
      coap_ready();
      queue.call_every(60ms, send_sensor_data);
      ready_sent = true;
  }
}

int main() {
  printf("START\n");

  coap_init();

  accelerometer_init();
  gyro_init();

  queue.call_every(5ms, button_fall_handler);
  queue.call_every(50ms, get_sensor_data);
  queue.dispatch_forever();

  // should never reach here
  return 0;
}
