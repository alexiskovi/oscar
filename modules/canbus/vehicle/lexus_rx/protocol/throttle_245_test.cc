#include "modules/canbus/vehicle/lexus_rx/protocol/throttle_245.h"

#include "gtest/gtest.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

TEST(Throttle_245_Test, General) {
  uint8_t data[5] = {0x00, 0x00, 0x00, 0x80, 0xCC};
  int32_t length = 5;
  ChassisDetail cd;
  Throttle245 gas;
  gas.Parse(data, length, &cd);
  
  EXPECT_DOUBLE_EQ(cd.gas().gas_pedal_position(), 0);
}

}  // namespace lexus_rx
}  // namespace canbus
}  // namespace apollo

