#include "modules/canbus/vehicle/lexus_rx/protocol/horn_750.h"

#include "gtest/gtest.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

TEST(Horn_750_Parse_Test, General) {
  uint8_t data[8] = {0x40, 0x04, 0x30, 0x06, 0x00, 0x20, 0x00, 0x00};
  int32_t length = 8;
  ChassisDetail cd;
  Horn750 horn;
  horn.Parse(data, length, &cd);
  EXPECT_TRUE(cd.light().is_horn_on());
}

TEST(Horn_750_Enable_Test, General) {
  uint8_t data[8] = {0x40, 0x04, 0x30, 0x06, 0x00, 0x20, 0x00, 0x00};
  Horn750 horn;
  EXPECT_EQ(horn.GetPeriod(), 20 * 1000);
  horn.UpdateData(data);
  EXPECT_EQ(data[0], 0b00000000);
  EXPECT_EQ(data[1], 0b00000000);
  EXPECT_EQ(data[2], 0b01100000);
  EXPECT_EQ(data[3], 0b00000000);
  EXPECT_EQ(data[4], 0b01010001);
  EXPECT_EQ(data[5], 0b01010010);
  EXPECT_EQ(data[6], 0b01010011);
  EXPECT_EQ(data[7], 0b00000000);
}

TEST(Horn_750_Disable_Test, General) {
  uint8_t data[8] = {0x40, 0x04, 0x30, 0x06, 0x00, 0x00, 0x00, 0x00};
  Horn750 horn;
  EXPECT_EQ(horn.GetPeriod(), 20 * 1000);
  horn.UpdateData(data);
  EXPECT_EQ(data[0], 0b00000000);
  EXPECT_EQ(data[1], 0b00000000);
  EXPECT_EQ(data[2], 0b01100000);
  EXPECT_EQ(data[3], 0b00000000);
  EXPECT_EQ(data[4], 0b01010001);
  EXPECT_EQ(data[5], 0b01010010);
  EXPECT_EQ(data[6], 0b01010011);
  EXPECT_EQ(data[7], 0b00000000);
}


}  // namespace lexus_rx
}  // namespace canbus
}  // namespace apollo

