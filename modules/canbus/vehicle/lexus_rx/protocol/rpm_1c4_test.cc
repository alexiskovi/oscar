#include "modules/canbus/vehicle/lexus_rx/protocol/rpm_1c4.h"

#include "gtest/gtest.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

TEST(Rpm_1c4_Test, General) {
  uint8_t data[8] = {0x06, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE2};
  int32_t length = 8;
  ChassisDetail cd;
  Rpm1C4 rpm;
  rpm.Parse(data, length, &cd);

  EXPECT_NEAR(cd.ems().engine_rpm(), 1240, 1);
}

}  // namespace lexus_rx
}  // namespace canbus
}  // namespace apollo

