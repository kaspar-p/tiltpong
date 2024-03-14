#include "lsm6dsl.h"
#include "mbed.h"
#include "stm32l475e_iot01_accelero.h"
#include "stm32l475e_iot01_gyro.h"
#include <TARGET_STM32L4/STM32Cube_FW/CMSIS/stm32l475xx.h>

void gyro_init();
void read_gyro();

void accelerometer_init();
void read_accelerometer_raw(int16_t *raw_counts);
void read_accelerometer();
