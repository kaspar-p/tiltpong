#include "lsm6dsl.h"
#include "mbed.h"
#include "stm32l475e_iot01_accelero.h"
#include "stm32l475e_iot01_gyro.h"
#include <TARGET_STM32L4/STM32Cube_FW/CMSIS/stm32l475xx.h>

#include <cmath>
#include <vector>
#include <array>

void gyro_init();
void read_gyro();

void accelerometer_init();
void get_accelerometer(int16_t *xyz_counts);
double get_tilt();
std::array<double, 3> get_new_position();

