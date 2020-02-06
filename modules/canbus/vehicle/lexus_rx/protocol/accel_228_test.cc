#include "modules/canbus/vehicle/lexus_rx/protocol/accel_228.h"

#include "gtest/gtest.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

TEST(Accel_228_Test, General) {
  uint8_t data[4] = {0x00, 0x96, 0x00, 0xC4};
  int32_t length = 4;
  ChassisDetail cd;
  Accel228 accel;
  accel.Parse(data, length, &cd);

  EXPECT_NEAR(cd.vehicle_spd().long_acc(), 0.15, 0.1);
  EXPECT_NEAR(cd.vehicle_spd().vert_acc(), 0.196, 0.1);
}

}  // namespace lexus_rx
}  // namespace canbus
}  // namespace apollo

