#include "modules/canbus/vehicle/lexus_rx/protocol/rpm_1c4.h"

#include "glog/logging.h"

#include "modules/drivers/canbus/common/byte.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

using ::apollo::drivers::canbus::Byte;

const int32_t Rpm1C4::ID = 0x1c4;

void Rpm1C4::Parse(const std::uint8_t *bytes, int32_t length,
                         ChassisDetail *chassis_detail) const {
  chassis_detail->mutable_ems()->set_engine_rpm(
      engine_rpm(bytes, length));
}

void Rpm1C4::Parse(const std::uint8_t *bytes, int32_t length,
                         const struct timeval &timestamp,
                         ChassisDetail *chassis_detail) const {
  Parse(bytes, length, chassis_detail);
}

double Rpm1C4::engine_rpm(const std::uint8_t *bytes,
                                            int32_t length) const {
  DCHECK_GE(length, 8);
  double value =  parse_two_frames(bytes[1], bytes[0]);
  value *= 0.8;
  return value;
}

double Rpm1C4::parse_two_frames(const std::uint8_t low_byte,
                                      const std::uint8_t high_byte) const {
  Byte high_frame(&high_byte);
  int32_t high = high_frame.get_byte(0, 8);
  Byte low_frame(&low_byte);
  int32_t low = low_frame.get_byte(0, 8);
  int32_t value = (high << 8) | low;
  return value;
}

}  // namespace lexus_rx
}  // namespace canbus
}  // namespace apollo
