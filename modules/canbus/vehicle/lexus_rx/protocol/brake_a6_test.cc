#include "modules/canbus/vehicle/lexus_rx/protocol/brake_a6.h"

#include "gtest/gtest.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

TEST(Break_a6_Test, General) {
  uint8_t data[8] = {0x3A, 0x17, 0x29, 0x70, 0x00, 0x00, 0x00, 0x98 };
  int32_t length = 8;
  ChassisDetail cd;
  BrakeA6 brake;
  brake.Parse(data, length, &cd);

  EXPECT_NEAR(cd.brake().brake_output(), 1, 1);
}

}  // namespace lexus_rx
}  // namespace canbus
}  // namespace apollo

