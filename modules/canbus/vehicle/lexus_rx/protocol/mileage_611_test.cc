#include "modules/canbus/vehicle/lexus_rx/protocol/mileage_611.h"

#include "gtest/gtest.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

TEST(Mileage_611_Test, General) {
  uint8_t data[8] = {0x21, 0x00, 0x00, 0x10, 0x00, 0x00, 0x03, 0xA8};
  int32_t length = 8;
  ChassisDetail cd;
  Mileage611 odo;
  odo.Parse(data, length, &cd);

  EXPECT_NEAR(cd.basic().odo_meter(), 936, 1);
}

}  // namespace lexus_rx
}  // namespace canbus
}  // namespace apollo

