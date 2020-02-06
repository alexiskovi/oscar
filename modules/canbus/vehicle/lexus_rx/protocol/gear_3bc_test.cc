#include "modules/canbus/vehicle/lexus_rx/protocol/gear_3bc.h"

#include "gtest/gtest.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

TEST(Gear3BC_Test, General) {
  uint8_t data[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 0x00, 0x30};
  int32_t length = 8;
  ChassisDetail cd;
  Gear3BC gear;
  gear.Parse(data, length, &cd);
  EXPECT_EQ(cd.gear().gear_state(), Chassis::GEAR_DRIVE);
}

}  // namespace lexus_rx
}  // namespace canbus
}  // namespace apollo

