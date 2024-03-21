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
  phi = phi * (180 / 3.14159); // convert to degrees
  return phi;
}

// assume device starts at position (0,0,0) and is not moving
std::vector<std::array<double, 3>> acceleration = { {0, 0, 0} };
std::vector<std::array<double, 3>> velocity = { {0, 0, 0} };
std::vector<std::array<double, 3>> position = { {0, 0, 0} };


/**
 * Crude method for computing position by double integrating acceleration
 * Not very accurate and likely needs a lot of work 
 * Consider other ideas for position of paddle
*/
std::array<double, 3> get_new_position()
{
  // for now only doing time steps of 1 second
  double delta_t = 1; 

  // get previous values for computing
  std::array<double, 3> old_acceleration = acceleration.back();
  std::array<double, 3> old_velocity = velocity.back();
  std::array<double, 3> old_position = position.back();

  // acceleration reading
  int16_t xyz_counts[3] = {0};
  BSP_ACCELERO_AccGetXYZ(xyz_counts);
  xyz_counts[2] = xyz_counts[2] - 1000; // remove gravity from calculation

  for (int i = 0; i < 3; i++) {
    if (xyz_counts[i] <= 100 && xyz_counts[i] >= -100) {
      xyz_counts[i] = 0; 
    }
  }

  std::array<double, 3> new_acceleration = {(double)xyz_counts[0], (double)xyz_counts[1], (double)xyz_counts[2]};

  // compute new velocity 
  double new_velocity_x = old_velocity[0] + (old_acceleration[0] + new_acceleration[0]) / 2 * delta_t;
  double new_velocity_y = old_velocity[1] + (old_acceleration[1] + new_acceleration[1]) / 2 * delta_t;
  double new_velocity_z = old_velocity[2] + (old_acceleration[2] + new_acceleration[2]) / 2 * delta_t;
  
  std::array<double, 3> new_velocity = {new_velocity_x, new_velocity_y, new_velocity_z};

  // compute new position from computed velocity
  double new_position_x = old_position[0] + (old_velocity[0] + new_velocity[0]) / 2 * delta_t;
  double new_position_y = old_position[1] + (old_velocity[1] + new_velocity[1]) / 2 * delta_t;
  double new_position_z = old_position[2] + (old_velocity[2] + new_velocity[2]) / 2 * delta_t;

  std::array<double, 3> new_position = {new_position_x, new_position_y, new_position_z};

  acceleration.push_back(new_acceleration);
  velocity.push_back(new_velocity);
  position.push_back(new_position);


  printf("[ACCELERATION] (x, y, z) = (%f, %f, %f)\n", new_acceleration[0], new_acceleration[1], new_acceleration[2]);
  printf("[VELOCITY]     (x, y, z) = (%f, %f, %f)\n", new_velocity[0], new_velocity[1], new_velocity[2]);
  printf("[POSITION]     (x, y, z) = (%f, %f, %f)\n", new_position[0], new_position[1], new_position[2]);

  return new_position;  
}


