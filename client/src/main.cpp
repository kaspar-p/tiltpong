#include "mbed.h"
#include "sensors.h"

int main() {
  accelerometer_init();
  gyro_init();
  while (1) {
    read_accelerometer();
    read_gyro();
  }
}
