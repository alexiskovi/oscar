#include "modules/canbus/vehicle/lexus_rx/protocol/brake_226.h"

#include "glog/logging.h"

#include "modules/drivers/canbus/common/byte.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

using ::apollo::drivers::canbus::Byte;

const int32_t Brake226::ID = 0x226;

void Brake226::Parse(const std::uint8_t *bytes, int32_t length,
                         ChassisDetail *chassis_detail) const {

  chassis_detail->mutable_deceleration()->set_brake_pressure(
      brake_pressure(bytes, length));

  chassis_detail->mutable_brake()->set_brake_pedal_position(
      brake_position(bytes, length));

  chassis_detail->mutable_brake()->set_is_brake_pedal_pressed(
      brake_pressed(bytes, length));
}

void Brake226::Parse(const std::uint8_t *bytes, int32_t length,
                         const struct timeval &timestamp,
                         ChassisDetail *chassis_detail) const {
  chassis_detail->mutable_eps()->set_timestamp_65(
      static_cast<double>(timestamp.tv_sec) +
      static_cast<double>(timestamp.tv_usec) / 1000000.0);
  Parse(bytes, length, chassis_detail);
}

double Brake226::brake_position(const std::uint8_t *bytes,
                                            int32_t length) const {
  Byte frame_high(bytes + 1);
  int32_t value = frame_high.get_byte(0, 8);
  return value;
}

double Brake226::brake_pressure(const std::uint8_t *bytes,
                                            int32_t length) const {
  Byte frame_high(bytes + 0);
  int32_t value = frame_high.get_byte(0, 8);
  return value;
}

bool Brake226::brake_pressed(const std::uint8_t *bytes,
                                            int32_t length) const {
    Byte frame(bytes + 4);
    return frame.is_bit_1(6)||frame.is_bit_1(5)||frame.is_bit_1(7);
}

}  // namespace lexus_rx
}  // namespace canbus
}  // namespace apollo
