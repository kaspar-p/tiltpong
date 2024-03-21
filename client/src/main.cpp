#include "mbed.h"
#include "coap.h"
#include "sensors.h"

EventQueue queue;
InterruptIn button1(BUTTON1);

bool button_pressed = false;

void get_sensor_data() {
  if (button_pressed) {
    std::array<double, 3> position = get_new_position();
    coap_send(position);

    button_pressed = false;
  }
}

void button_fall_handler() {
  if (button1.read() == 1) {
    button_pressed = true;
  }
}

int main() {
  printf("START\n");

  coap_init();

  accelerometer_init();
  gyro_init();

  queue.call_every(5ms, button_fall_handler);
  queue.call_every(1000ms, get_sensor_data);
  queue.dispatch_forever();

  // should never reach here
  return 0;
}
