#include "modules/canbus/vehicle/lexus_rx/protocol/steering_24.h"

#include "gtest/gtest.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

TEST(Steering_24_Test, General) {
  uint8_t data[8] = {0x02, 0x01, 0x02, 0x15, 0xF1, 0xEF, 0x80, 0xA6};
  int32_t length = 8;
  ChassisDetail cd;
  Steering24 steer;
  steer.Parse(data, length, &cd);

  //EXPECT_NEAR(cd.vehicle_spd().lat_acc(), 0.15, 1);
  EXPECT_NEAR(cd.vehicle_spd().yaw_rate(), 1, 1);
  EXPECT_NEAR(cd.eps().epas_torque(), 5, 1);
}

}  // namespace lexus_rx
}  // namespace canbus
}  // namespace apollo

