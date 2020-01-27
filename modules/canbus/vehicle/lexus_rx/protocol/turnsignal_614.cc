#include "modules/canbus/vehicle/lexus_rx/protocol/turnsignal_614.h"

#include "glog/logging.h"

#include "modules/drivers/canbus/common/byte.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

using ::apollo::drivers::canbus::Byte;

const int32_t Turnsignal614::ID = 0x614;

void Turnsignal614::Parse(const std::uint8_t *bytes, int32_t length,
                         ChassisDetail *chassis_detail) const {

    auto status = turn_state( bytes, length );
    switch (status) {
    case 0x03:
      chassis_detail->mutable_light()->set_turn_light_type( Light::TURN_LIGHT_OFF );
      break;
    case 0x01:
      chassis_detail->mutable_light()->set_turn_light_type( Light::TURN_LEFT_ON );
      break;
    case 0x02:
      chassis_detail->mutable_light()->set_turn_light_type( Light::TURN_RIGHT_ON );
      break;
    default:
      chassis_detail->mutable_light()->set_turn_light_type( Light::TURN_LIGHT_OFF );
      break;
  }

}

void Turnsignal614::Parse(const std::uint8_t *bytes, int32_t length,
                         const struct timeval &timestamp,
                         ChassisDetail *chassis_detail) const {
  chassis_detail->mutable_eps()->set_timestamp_65(
      static_cast<double>(timestamp.tv_sec) +
      static_cast<double>(timestamp.tv_usec) / 1000000.0);
  Parse(bytes, length, chassis_detail);
}


int32_t Turnsignal614::turn_state(const std::uint8_t *bytes, int32_t length) const {
  Byte frame(bytes + 3 );
  return frame.get_byte(4,5);
}


}  // namespace lexus_rx
}  // namespace canbus
}  // namespace apollo
