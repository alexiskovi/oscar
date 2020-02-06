#include "modules/canbus/vehicle/lexus_rx/protocol/horn_750.h"

#include "modules/drivers/canbus/common/byte.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

using ::apollo::drivers::canbus::Byte;

// public

const int32_t Horn750::ID = 0x750;

uint32_t Horn750::GetPeriod() const {
  static const uint32_t PERIOD = 20 * 1000;
  return PERIOD;
}

void Horn750::Parse(const std::uint8_t *bytes, int32_t length,
                         ChassisDetail *chassis_detail) const {
   chassis_detail->mutable_light()->set_is_horn_on((bytes, length));
}

void Horn750::Parse(const std::uint8_t *bytes, int32_t length,
                         const struct timeval &timestamp,
                         ChassisDetail *chassis_detail) const {
  chassis_detail->mutable_eps()->set_timestamp_65(
      static_cast<double>(timestamp.tv_sec) +
      static_cast<double>(timestamp.tv_usec) / 1000000.0);
  Parse(bytes, length, chassis_detail);
}

void Horn750::UpdateData(uint8_t *data) {
  set_enable_p(data, horn_enable_);
}

void Horn750::Reset() {
  horn_enable_ = false;
}

Horn750 *Horn750::set_enable() {
  horn_enable_ = true;
  return this;
}

Horn750 *Horn750::set_disable() {
  horn_enable_ = false;
  return this;
}

// private

void Horn750::set_enable_p(uint8_t *bytes, bool enable) {
  Byte frame(bytes + 5);
  uint8_t val = 0x00;
  if ( enable ) val = 0x20;
  else val = 0x00;
  frame.set_value(val,0,8);
}

bool Horn750::horn_status( const std::uint8_t *bytes, int32_t length) const {
    Byte frame(bytes + 5);
    uint8_t value = frame.get_byte(0,8);
    if ( value == 0x20 ) return true;
    else return false;
    return false;
}


}  // namespace lexus_rx
}  // namespace canbus
}  // namespace apollo
