#include "modules/canbus/vehicle/lexus_rx/protocol/brake_226.h"

#include "gtest/gtest.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

TEST(Brake_226_Test, General) {
  uint8_t data[8] = {0x00, 0x01, 0x10, 0x01, 0x00, 0x00, 0x00, 0x00 };
  int32_t length = 8;
  ChassisDetail cd;
  Brake226 brake;
  brake.Parse(data, length, &cd);

  EXPECT_NEAR(cd.brake().brake_pedal_position(), 1, 1);
  EXPECT_NEAR(cd.deceleration().brake_pressure(), 1, 1);
}

}  // namespace lexus_rx
}  // namespace canbus
}  // namespace apollo

