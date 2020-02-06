#include "modules/canbus/vehicle/lexus_rx/protocol/cruise_361.h"

#include "modules/drivers/canbus/common/byte.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

using ::apollo::drivers::canbus::Byte;

const int32_t Cruise361::ID = 0x361;

void Cruise361::Parse(const std::uint8_t *bytes, int32_t length,
                       ChassisDetail *chassis_detail) const {
  printf("Receive cruse data status\n");
  chassis_detail->mutable_basic()->set_acc_on_button(
      is_acc_on_pressed(bytes, length));
  chassis_detail->mutable_basic()->set_acc_off_button(
      is_acc_off_pressed(bytes, length));

}

void Cruise361::Parse(const std::uint8_t *bytes, int32_t length,
                       const struct timeval &timestamp,
                       ChassisDetail *chassis_detail) const {
  chassis_detail->mutable_eps()->set_timestamp_65(
      timestamp.tv_sec + timestamp.tv_usec / 1000000.0);
  Parse(bytes, length, chassis_detail);
}


bool Cruise361::is_acc_on_pressed(const std::uint8_t *bytes,
                               int32_t length) const {
  Byte frame(bytes);
  return frame.is_bit_1(5);
}

bool Cruise361::is_acc_off_pressed(const std::uint8_t *bytes,
                                int32_t length) const {
  Byte frame(bytes);
  return !frame.is_bit_1(5);
}


} //lexus_rx
} //canbus
} //apollo


