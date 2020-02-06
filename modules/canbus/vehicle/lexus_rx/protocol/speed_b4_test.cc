#include "modules/canbus/vehicle/lexus_rx/protocol/speed_b4.h"

#include "gtest/gtest.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

TEST(Speed_b4_Test, General) {
  uint8_t data[8] = {0x00, 0x00, 0x00, 0x00, 0x71, 0x07, 0x81, 0xB5};
  int32_t length = 8;
  ChassisDetail cd;
  SpeedB4 speed;
  speed.Parse(data, length, &cd);

  EXPECT_NEAR(cd.vehicle_spd().vehicle_spd(), 14, 1);
}

}  // namespace lexus_rx
}  // namespace canbus
}  // namespace apollo

