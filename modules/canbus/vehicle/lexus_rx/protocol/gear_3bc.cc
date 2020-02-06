#include "modules/canbus/vehicle/lexus_rx/protocol/gear_3bc.h"

#include "glog/logging.h"

#include "modules/drivers/canbus/common/byte.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

using ::apollo::drivers::canbus::Byte;

const int32_t Gear3BC::ID = 0x3bc;

void Gear3BC::Parse(const std::uint8_t *bytes, int32_t length,
                         ChassisDetail *chassis_detail) const {

    auto gear = gear_state( bytes, length );
    AERROR << "Current gear state: " << gear;
    switch (gear) {
    case 0x00:
      chassis_detail->mutable_gear()->set_gear_state(Chassis::GEAR_DRIVE);
      break;
    case 0x08:
      chassis_detail->mutable_gear()->set_gear_state(Chassis::GEAR_NEUTRAL);
      break;
    case 0x10:
      chassis_detail->mutable_gear()->set_gear_state(Chassis::GEAR_REVERSE);
      break;
    case 0x20:
      chassis_detail->mutable_gear()->set_gear_state(Chassis::GEAR_PARKING);
      //chassis_detail->mutable_gear()->set_gear_state(Chassis::GEAR_DRIVE);
      break;
    default:
      chassis_detail->mutable_gear()->set_gear_state(Chassis::GEAR_INVALID);
      break;
  }

}

void Gear3BC::Parse(const std::uint8_t *bytes, int32_t length,
                         const struct timeval &timestamp,
                         ChassisDetail *chassis_detail) const {
  chassis_detail->mutable_eps()->set_timestamp_65(
      static_cast<double>(timestamp.tv_sec) +
      static_cast<double>(timestamp.tv_usec) / 1000000.0);
  Parse(bytes, length, chassis_detail);
}


int32_t Gear3BC::gear_state(const std::uint8_t *bytes, int32_t length) const {
  Byte frame_d(bytes + 5 );
  bool is_drive = frame_d.is_bit_1(8);
  if ( is_drive ) return 0;

  Byte frame_s(bytes + 4 );
  bool is_sport = frame_s.is_bit_1(2);
  if ( is_sport ) return 0;

  Byte frame(bytes + 1);
  int32_t x = frame.get_byte(0, 6);
  return x;
}


}  // namespace lexus_rx
}  // namespace canbus
}  // namespace apollo
