#include "modules/canbus/vehicle/lexus_rx/protocol/radar_2e6.h"

#include "gtest/gtest.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

TEST(Radar_2e6_Test, General) {
  uint8_t data[8] = {0x40, 0x04, 0x30, 0x06, 0x00, 0x20, 0x00, 0x00};
  Radar2E6 radar;
  EXPECT_EQ(radar.GetPeriod(), 30 * 1000);
  radar.UpdateData(data);
}

}  // namespace lexus_rx
}  // namespace canbus
}  // namespace apollo

