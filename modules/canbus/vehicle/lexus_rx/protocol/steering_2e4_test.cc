#include "modules/canbus/vehicle/lexus_rx/protocol/steering_2e4.h"

#include "gtest/gtest.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

TEST(Steering_2E4_Test1, General) {
  uint8_t data[5] = {0xE4, 0x00, 0x00, 0x00, 0xCF};
  int32_t length = 5;
  ChassisDetail cd;
  Steering2E4 steer;
  steer.Parse(data, length, &cd);

  EXPECT_NEAR(cd.eps().epas_torque(), 0, 0.1);
}

TEST(Steering_2E4_Test2, General) {
  uint8_t data[5] = {0xE4, 0xFF, 0xF0, 0x00, 0xCF};
  int32_t length = 5;
  ChassisDetail cd;
  Steering2E4 steer;
  steer.Parse(data, length, &cd);

  EXPECT_NEAR(cd.eps().epas_torque(), -16, 1);
}


TEST(Steering_2E4_Control1, General) {
    uint8_t data[8] = {0x67, 0x62, 0x63, 0x64, 0x51};
    Steering2E4 steering;
    steering.set_enable();
    steering.set_current_steering_angle(10);
    steering.set_goal_steering_angle(20);
    EXPECT_EQ(steering.GetPeriod(), 24 * 1000);
    steering.UpdateData(data);
    EXPECT_EQ(data[0], 0b00000000);
}


}  // namespace lexus_rx
}  // namespace canbus
}  // namespace apollo

