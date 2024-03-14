#include "sensors.h"
#include "stm32l475e_iot01_gyro.h"

void gyro_init() {
  uint32_t status = BSP_GYRO_Init();
  assert(status == GYRO_OK);
}

void read_gyro() {
  float xyz_counts[3] = {0};
  BSP_GYRO_GetXYZ(xyz_counts);
  printf("[GYRO] (x, y, z) = (%f, %f, %f)\n", xyz_counts[0], xyz_counts[1],
         xyz_counts[2]);
}

void accelerometer_init() {
  uint32_t status;
  status = BSP_ACCELERO_Init();
  assert(status == ACCELERO_OK);
}

void read_accelerometer_raw(int16_t *raw_counts) {
  // BSP_ACCELERO_AccGetXYZ(raw_counts);
  uint8_t ctrlx = 0;
  uint8_t buffer[6];
  uint8_t i = 0;
  float sensitivity = 0;

  // found this here dont know how i supposed to figure this out lmao
  // why use SENSOR_IO_ReadMultiple when  BSP_ACCELERO_AccGetXYZ is right there
  // https://github.com/STMicroelectronics/stm32-lsm6dsl/blob/main/lsm6dsl.c

  /* Read the acceleration control register content */
  ctrlx = SENSOR_IO_Read(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW,
                         LSM6DSL_ACC_GYRO_CTRL1_XL);

  /* Read output register X, Y & Z acceleration */
  SENSOR_IO_ReadMultiple(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW,
                         LSM6DSL_ACC_GYRO_OUTX_L_XL, buffer, 6);

  for (i = 0; i < 3; i++) {
    raw_counts[i] =
        ((((uint16_t)buffer[2 * i + 1]) << 8) + (uint16_t)buffer[2 * i]);
  }

  /* Normal mode */
  /* Switch the sensitivity value set in the CRTL1_XL */
  switch (ctrlx & 0x0C) {
  case LSM6DSL_ACC_FULLSCALE_2G:
    sensitivity = LSM6DSL_ACC_SENSITIVITY_2G;
    break;
  case LSM6DSL_ACC_FULLSCALE_4G:
    sensitivity = LSM6DSL_ACC_SENSITIVITY_4G;
    break;
  case LSM6DSL_ACC_FULLSCALE_8G:
    sensitivity = LSM6DSL_ACC_SENSITIVITY_8G;
    break;
  case LSM6DSL_ACC_FULLSCALE_16G:
    sensitivity = LSM6DSL_ACC_SENSITIVITY_16G;
    break;
  }

  /* Obtain the mg value for the three axis */
  for (i = 0; i < 3; i++) {
    raw_counts[i] = (int16_t)(raw_counts[i] * sensitivity);
  }
}

/**
 * @brief Read and print (to stdout) the accelerometer data.
 */
void read_accelerometer() {
  float magnitude_g = 0.0;
  float x_mg = 0.0, y_mg = 0.0, z_mg = 0.0;

  int16_t xyz_counts[3] = {0};
  read_accelerometer_raw(xyz_counts);

  // TODO: Convert the raw xyz values to mg
  x_mg = xyz_counts[0];
  y_mg = xyz_counts[1];
  z_mg = xyz_counts[2];
  // TODO: Calculate the mangitude of acceleration and convert it to g (1g =
  // 1000mg)
  // TODO: The magnitude is the square root of the sum of x^2, y^2, and z^2
  magnitude_g = sqrt(x_mg * x_mg + y_mg * y_mg + z_mg * z_mg) / 1000;

  printf("[ACCELEROMETER]  %f g; (x, y, z) = (%.1f mg, %.1f mg, %.1f mg)\n",
         magnitude_g, x_mg, y_mg, z_mg);
}
