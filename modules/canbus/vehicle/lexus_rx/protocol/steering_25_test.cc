#include "modules/canbus/vehicle/lexus_rx/protocol/steering_25.h"

#include "gtest/gtest.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

TEST(Steering_25_Test1, General) {
  uint8_t data[8] = {0x00, 0x01, 0x20, 0x00, 0x70, 0x00, 0x00, 0xBE};
  int32_t length = 8;
  ChassisDetail cd;
  Steering25 steer;
  steer.Parse(data, length, &cd);
  
  EXPECT_DOUBLE_EQ(cd.eps().steering_angle(), 1.5);
}


/* Reverse */
TEST(Steering_25_Test2, General) {
  uint8_t data[8] = {0xFF, 0xF0, 0x20, 0x00, 0x70, 0x00, 0x00, 0xBE};
  int32_t length = 8;
  ChassisDetail cd;
  Steering25 steer;
  steer.Parse(data, length, &cd);

  EXPECT_NEAR(cd.eps().steering_angle(), -24, 1);
}

}  // namespace lexus_rx
}  // namespace canbus
}  // namespace apollo

