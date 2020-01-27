#include "modules/canbus/vehicle/lexus_rx/protocol/wheelspeed_aa.h"

#include "gtest/gtest.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

TEST(Wheelspeed_AA_Test, General) {
  WheelspeedAA wheelspeed;
  uint8_t data[8] = {0x1F, 0xFE, 0x1F, 0xFB, 0x1F, 0xEF, 0x1F, 0xFA};
  int32_t length = 8;
  ChassisDetail cd;
  struct timeval timestamp;
  wheelspeed.Parse(data, length, timestamp, &cd);

  EXPECT_TRUE(cd.vehicle_spd().is_wheel_spd_fl_valid());
  EXPECT_NEAR(cd.vehicle_spd().wheel_spd_rr(), 14, 1);
  EXPECT_TRUE(cd.vehicle_spd().is_wheel_spd_fr_valid());
  EXPECT_NEAR(cd.vehicle_spd().wheel_spd_rl(), 14, 1);
  EXPECT_TRUE(cd.vehicle_spd().is_wheel_spd_rl_valid());
  EXPECT_NEAR(cd.vehicle_spd().wheel_spd_fr(), 14, 1);
  EXPECT_TRUE(cd.vehicle_spd().is_wheel_spd_rr_valid());
  EXPECT_NEAR(cd.vehicle_spd().wheel_spd_fl(), 14 ,1);
}

}  // namespace lexus_rx
}  // namespace canbus
}  // namespace apollo
