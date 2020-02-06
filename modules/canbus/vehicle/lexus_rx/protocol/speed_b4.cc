#include "modules/canbus/vehicle/lexus_rx/protocol/speed_b4.h"

#include "glog/logging.h"

#include "modules/drivers/canbus/common/byte.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

using ::apollo::drivers::canbus::Byte;

const int32_t SpeedB4::ID = 0xB4;

void SpeedB4::Parse(const std::uint8_t *bytes, int32_t length,
                         ChassisDetail *chassis_detail) const {
  chassis_detail->mutable_vehicle_spd()->set_vehicle_spd(
      vehicle_speed(bytes, length));
}

void SpeedB4::Parse(const std::uint8_t *bytes, int32_t length,
                         const struct timeval &timestamp,
                         ChassisDetail *chassis_detail) const {
  chassis_detail->mutable_eps()->set_timestamp_65(
      static_cast<double>(timestamp.tv_sec) +
      static_cast<double>(timestamp.tv_usec) / 1000000.0);
  Parse(bytes, length, chassis_detail);
}

double SpeedB4::vehicle_speed(const std::uint8_t *bytes,
                                            int32_t length) const {
  DCHECK_GE(length, 8);
  Byte high_frame(bytes + 5);
  int32_t high = high_frame.get_byte(0, 8);
  Byte low_frame(bytes + 6);
  int32_t low = low_frame.get_byte(0, 8);
  int32_t value = (high << 8) | low;
  double val = value * 0.010000;
  val /= 3.6;
  return val;
}

}  // namespace lexus_rx
}  // namespace canbus
}  // namespace apollo
