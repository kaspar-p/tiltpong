#include "sensors.h"
#include "stm32l475e_iot01_gyro.h"

void gyro_init()
{
  uint32_t status = BSP_GYRO_Init();
  assert(status == GYRO_OK);
}

void read_gyro()
{
  float xyz_counts[3] = {0};
  BSP_GYRO_GetXYZ(xyz_counts);
  printf("[GYRO] (x, y, z) = (%f, %f, %f)\n", xyz_counts[0], xyz_counts[1],
         xyz_counts[2]);
}

void accelerometer_init()
{
  uint32_t status;
  status = BSP_ACCELERO_Init();
  assert(status == ACCELERO_OK);
}

/**
 * @brief Read and print (to stdout) the accelerometer data.
 */
void get_accelerometer(int16_t *xyz_counts)
{
  float magnitude_g = 0.0;
  float x_mg = 0.0, y_mg = 0.0, z_mg = 0.0;
  // get acceleration is each axis in mg
  BSP_ACCELERO_AccGetXYZ(xyz_counts);
  x_mg = xyz_counts[0];
  y_mg = xyz_counts[1];
  z_mg = xyz_counts[2];
  // magnitude is in g
  magnitude_g = sqrt(x_mg * x_mg + y_mg * y_mg + z_mg * z_mg) / 1000;
  printf("[ACCELEROMETER]  %f g; (x, y, z) = (%.1f mg, %.1f mg, %.1f mg)\n",
         magnitude_g, x_mg, y_mg, z_mg);
}

/**
 * Gets the tilt angle (phi) between the device and the z axis
 * phi is limited to [0,180] degrees
 */
double get_tilt()
{
  int16_t xyz_counts[3] = {0};
  BSP_ACCELERO_AccGetXYZ(xyz_counts);
  double x = xyz_counts[0];
  double y = xyz_counts[1];
  double z = xyz_counts[2];

  double phi = acos(z / sqrt(x * x + y * y + z * z)); // radians
  // phi = phi * (180 / 3.14159);                        // convert to degrees
  return phi;
}

// assume device starts at position (0,0,0) and is not moving
double old_acceleration = 0;
double old_velocity = 0;
double old_position = 0;
double max_diff = 0; 
/**
 * Compute the velocity of the device by integrating acceleration.
 * Several assumptions are made to improve usability
 * - If acceleration is 0 then velocity is set to 0
 * - We're only computing the velocity along the z axis.
 * - The device has to be mostly held still such that gravity mostly only acts on the z axis
 */
double get_new_position()
{
  double delta_t = 0.01;

  // acceleration reading
  int16_t xyz_counts[3] = {0};
  BSP_ACCELERO_AccGetXYZ(xyz_counts);
  double angle = get_tilt();
  double true_g = xyz_counts[2];
  double approx_g = cos(angle) * 1000;
  max_diff = (true_g - approx_g >= max_diff || approx_g - true_g >= max_diff) ? true_g - approx_g : max_diff;
  double new_acceleration = true_g - approx_g; // remove gravity from calculation

  double new_velocity = 0;
  double new_position = 0;

  // close enough to not moving that we set it to 0
  // compute new velocity
  // when movement stops we wipe the previous informaiton as an attempt to counter drift
  // might have to mess with these boundaries

  // accel dead zone
  if (-10 <= new_acceleration && new_acceleration <= 10)
  {
    new_acceleration = 0;
    old_acceleration = 0;
    old_velocity = 0;
    new_position = old_position;
  }
  else
  {
    new_velocity = old_velocity + (old_acceleration + new_acceleration) / 2 * delta_t;
    // velocity dead zone
    if (-6 <= new_velocity && new_velocity <= 6)
    {
      new_velocity = 0;
    }
    new_position = old_position + (old_velocity + new_velocity) / 2 * delta_t;
  }

  old_acceleration = new_acceleration;
  old_velocity = new_velocity;
  old_position = new_position;

  // + is up - is down
  char direction = (new_velocity >= 0) ? '-' : '+';
  direction = (new_velocity == 0) ? 'X' : direction;

  printf("[TRUE_G] %.2f ", true_g);
  printf("[APPROX_G] %.2f ", approx_g);
  printf("[DIFF] %.2f ", xyz_counts[2] - approx_g);
  printf("[MAX_DIFF] %.2f ", max_diff);
  printf("[ACCELERATION] %.2f ", new_acceleration);
  printf("[VELOCITY] %.2f ", new_velocity);
  printf("[DIRECTION] %c\n", direction);

  return new_velocity;
}
