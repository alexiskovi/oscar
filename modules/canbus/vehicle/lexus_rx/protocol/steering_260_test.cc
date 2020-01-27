#include "modules/canbus/vehicle/lexus_rx/protocol/steering_260.h"

#include "gtest/gtest.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

TEST(Steering_260_Test, General) {
  uint8_t data[8] = {0x08, 0x00, 0x2E, 0x00, 0x00, 0x00, 0x26, 0xC6};
  int32_t length = 8;
  ChassisDetail cd;
  Steering260 steer;
  steer.Parse(data, length, &cd);

  EXPECT_DOUBLE_EQ(cd.eps().eps_driver_hand_torq(), 46);
  EXPECT_NEAR(cd.eps().epas_torque(), 27.74, 0.1);
  EXPECT_FALSE( cd.eps().driver_override() );
}

}  // namespace lexus_rx
}  // namespace canbus
}  // namespace apollo

