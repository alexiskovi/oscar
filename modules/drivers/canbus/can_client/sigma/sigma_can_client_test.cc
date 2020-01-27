#include "modules/drivers/canbus/can_client/sigma/sigma_can_client.h"

#include <vector>

#include "gtest/gtest.h"

#include "modules/drivers/canbus/proto/can_card_parameter.pb.h"

namespace apollo {
namespace drivers {
namespace canbus {
namespace can {

using apollo::common::ErrorCode;


TEST(SigmaCanClientTestSend1, sigma_test) {

  uint8_t buff[] = {  0x00, 0x00, 0x63, 0x81, 0x00, 0x00, 0x00, 0x00 };

  SigmaCanClient can_client;
  std::vector<uint8_t> canMsg = can_client.getCanSendMessage( buff, 8, 835  );
  EXPECT_EQ( canMsg.size(), 26 );
  for ( auto &msg: canMsg ) {
     printf("%02x ", msg );
  }
  EXPECT_EQ( canMsg[24], 0xed );
  EXPECT_EQ( canMsg[25], 0x35 );
  printf("\n");
}

TEST(SigmaCanClientTestParse1, sigma_test) {

  std::vector<uint8_t> buff = {0xaa, 0xff, 0x10, 0x00, 0x10, 0x00, 0xc1, 0x00,
                               0x10, 0x02, 0x00, 0x01, 0x00, 0x00, 0x01, 0x08,
                               0xdc, 0x01, 0x00, 0x3d, 0x21, 0x20, 0x01, 0x71,
                               0x8c, 0x35};

  SigmaCanClient can_client;
  can_client.setVBuffer( buff );
  std::vector<uint8_t> canMsg = can_client.getCanMessage( );
  EXPECT_EQ( canMsg.size(), 8 );
  EXPECT_EQ( canMsg[0], 0xdc );
  EXPECT_EQ( canMsg[7], 0x71 );
  EXPECT_EQ( can_client.getCanId(), 0x100 );

}

TEST(SigmaCanClientTestParse2, sigma_test) {

  std::vector<uint8_t> buff = { 0xaa, 0x00, 0xaa, 0xff, 0x10, 0x00, 0x10, 0x00, 0xc1, 0x00,
                               0x10, 0x02, 0x00, 0x01, 0x00, 0x00, 0x01, 0x08,
                               0xdc, 0x01, 0x00, 0x3d, 0x21, 0x20, 0x01, 0x71,
                               0x8c, 0x35, 0x11, 0x22 };

  SigmaCanClient can_client;
  can_client.setVBuffer( buff );
  std::vector<uint8_t> canMsg = can_client.getCanMessage( );
  EXPECT_EQ( canMsg.size(), 8 );
  EXPECT_EQ( canMsg[0], 0xdc );
  EXPECT_EQ( canMsg[7], 0x71 );
  EXPECT_EQ( can_client.getCanId(), 0x100 );
}


TEST(SigmaCanClientTestParse3, sigma_test) {

  std::vector<uint8_t> buff = { 0xaa, 0x00, 0xaa, 0xff, 0x10, 0x00, 0x10, 0x00, 0xc1, 0x00,
                               0x10, 0x02, 0x00, 0x01, 0x00, 0x00, 0x01, 0x05,
                               0xdc, 0x01, 0x00, 0x3d, 0x21, 0x20, 0x01, 0x71,
                               0x8c, 0x35, 0x11, 0x22 };

  SigmaCanClient can_client;
  can_client.setVBuffer( buff );
  std::vector<uint8_t> canMsg = can_client.getCanMessage();
  EXPECT_EQ( canMsg.size(), 5 );
  EXPECT_EQ( canMsg[0], 0xdc );
  EXPECT_EQ( canMsg[4], 0x21 );
  EXPECT_EQ( can_client.getCanId(), 0x100 );
}


/* Can message size more than 8 bytes */
TEST(SigmaCanClientTestParse4, sigma_test) {

  std::vector<uint8_t> buff = { 0xaa, 0x00, 0xaa, 0xff, 0x10, 0x00, 0x10, 0x00, 0xc1, 0x00,
                               0x10, 0x02, 0x00, 0x01, 0x00, 0x00, 0x01, 0x09,
                               0xdc, 0x01, 0x00, 0x3d, 0x21, 0x20, 0x01, 0x71,
                               0x8c, 0x35, 0x11, 0x22 };

  SigmaCanClient can_client;
  can_client.setVBuffer( buff );
  std::vector<uint8_t> canMsg = can_client.getCanMessage( );
  EXPECT_EQ( canMsg.size(), 0 );
  EXPECT_EQ( can_client.getCanId(), 0 );
}

/* Can message size more than 8 bytes */
TEST(SigmaCanClientTestParse5, sigma_test) {

  std::vector<uint8_t> buff = { 0xaa, 0x00, 0xaa, 0xff, 0x10, 0x00, 0x10, 0x00, 0xc1, 0x00,
                               0x10, 0x02, 0x00, 0x01, 0x00, 0x00, 0x01, 0x08,
                               0xdc, 0x01, 0x00, 0x3d, 0x21, 0x20, 0x01, 0x71,
                               0x8c };

  SigmaCanClient can_client;
  can_client.setVBuffer( buff );
  std::vector<uint8_t> canMsg = can_client.getCanMessage();
  EXPECT_EQ( canMsg.size(), 0 );
  EXPECT_EQ( can_client.getCanId(), 0 );
}

}  // namespace can
}  // namespace canbus
}  // namespace drivers
}  // namespace apollo
