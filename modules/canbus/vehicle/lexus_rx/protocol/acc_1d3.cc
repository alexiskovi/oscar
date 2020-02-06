#include "modules/canbus/vehicle/lexus_rx/protocol/acc_1d3.h"

#include "modules/drivers/canbus/common/byte.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

using ::apollo::drivers::canbus::Byte;

const int32_t Acc1D3::ID = 0x1d3;

void Acc1D3::Parse(const std::uint8_t *bytes, int32_t length,
                       ChassisDetail *chassis_detail) const {
  chassis_detail->mutable_basic()->set_acc_on_button(
      is_acc_on_pressed(bytes, length));
  chassis_detail->mutable_basic()->set_acc_off_button(
      is_acc_off_pressed(bytes, length));

}

void Acc1D3::Parse(const std::uint8_t *bytes, int32_t length,
                       const struct timeval &timestamp,
                       ChassisDetail *chassis_detail) const {
  chassis_detail->mutable_eps()->set_timestamp_65(
      timestamp.tv_sec + timestamp.tv_usec / 1000000.0);
  Parse(bytes, length, chassis_detail);
}


bool Acc1D3::is_acc_on_pressed(const std::uint8_t *bytes,
                               int32_t length) const {
  Byte frame(bytes + 1);
  return frame.is_bit_1(7);
}

bool Acc1D3::is_acc_off_pressed(const std::uint8_t *bytes,
                                int32_t length) const {
  Byte frame(bytes + 1);
  return !frame.is_bit_1(7);
}


} //lexus_rx
} //canbus
} //apollo


