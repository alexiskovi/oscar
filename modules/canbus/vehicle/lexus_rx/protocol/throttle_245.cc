#include "modules/canbus/vehicle/lexus_rx/protocol/throttle_245.h"

#include "glog/logging.h"

#include "modules/drivers/canbus/common/byte.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

using ::apollo::drivers::canbus::Byte;

const int32_t Throttle245::ID = 0x245;

void Throttle245::Parse(const std::uint8_t *bytes, int32_t length,
                         ChassisDetail *chassis_detail) const {

   chassis_detail->mutable_gas()->set_gas_pedal_position(gas_pedal(bytes, length));
   chassis_detail->mutable_gas()->set_throttle_output(gas_pedal(bytes, length));
}

void Throttle245::Parse(const std::uint8_t *bytes, int32_t length,
                         const struct timeval &timestamp,
                         ChassisDetail *chassis_detail) const {
  chassis_detail->mutable_eps()->set_timestamp_65(
      static_cast<double>(timestamp.tv_sec) +
      static_cast<double>(timestamp.tv_usec) / 1000000.0);
  Parse(bytes, length, chassis_detail);
}


double Throttle245::gas_pedal(const std::uint8_t *bytes, int32_t length) const {
  // Pedal Command position
  Byte frame(bytes + 2);
  int32_t value = frame.get_byte(0, 8);
  return double(value)/2;
}

}  // namespace lexus_rx
}  // namespace canbus
}  // namespace apollo
