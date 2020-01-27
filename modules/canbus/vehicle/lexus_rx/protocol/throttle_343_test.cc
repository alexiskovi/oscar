#include "modules/canbus/vehicle/lexus_rx/protocol/throttle_343.h"

#include "gtest/gtest.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

TEST(Throttle_343_Test, General) {
  uint8_t data[8] = {0xFC, 0x3E, 0x61, 0xC0, 0x00, 0xED, 0x00, 0x96};
  int32_t length = 8;
  ChassisDetail cd;
  Throttle343 throttle;
  throttle.Parse(data, length, &cd);
  EXPECT_NEAR(cd.gas().throttle_cmd(), 65, 1);
  EXPECT_FALSE(cd.gas().driver_override());
}

}  // namespace lexus_rx
}  // namespace canbus
}  // namespace apollo

