#include "modules/canbus/vehicle/lexus_rx/protocol/wheelspeed_aa.h"

#include "glog/logging.h"

#include "modules/drivers/canbus/common/byte.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

using ::apollo::drivers::canbus::Byte;

const int32_t WheelspeedAA::ID = 0xAA;

void WheelspeedAA::Parse(const std::uint8_t *bytes, int32_t length,
                         ChassisDetail *chassis_detail) const {
  // how to set direction
  // what is "valid"
  // front left
  chassis_detail->mutable_vehicle_spd()->set_wheel_spd_fl(
      front_left_wheel_speed(bytes, length));
  chassis_detail->mutable_vehicle_spd()->set_is_wheel_spd_fl_valid(true);
  // front right
  chassis_detail->mutable_vehicle_spd()->set_wheel_spd_fr(
      front_right_wheel_speed(bytes, length));
  chassis_detail->mutable_vehicle_spd()->set_is_wheel_spd_fr_valid(true);
  // rear left
  chassis_detail->mutable_vehicle_spd()->set_wheel_spd_rl(
      rear_left_wheel_speed(bytes, length));
  chassis_detail->mutable_vehicle_spd()->set_is_wheel_spd_rl_valid(true);
  // rear right
  chassis_detail->mutable_vehicle_spd()->set_wheel_spd_rr(
      rear_right_wheel_speed(bytes, length));
  chassis_detail->mutable_vehicle_spd()->set_is_wheel_spd_rr_valid(true);
  /*
  -?(rr(bytes, length));
  -?(rl(bytes, length));
  -?(fr(bytes, length));
  -?(fl(bytes, length));*/
}

void WheelspeedAA::Parse(const std::uint8_t *bytes, int32_t length,
                         const struct timeval &timestamp,
                         ChassisDetail *chassis_detail) const {
  chassis_detail->mutable_eps()->set_timestamp_65(
      static_cast<double>(timestamp.tv_sec) +
      static_cast<double>(timestamp.tv_usec) / 1000000.0);
  Parse(bytes, length, chassis_detail);
}

double WheelspeedAA::front_left_wheel_speed(const std::uint8_t *bytes,
                                            int32_t length) const {
  DCHECK_GE(length, 2);
  double value = parse_two_frames(bytes[3], bytes[2]);
  value -= 67.67;
  return value;
}

double WheelspeedAA::front_right_wheel_speed(const std::uint8_t *bytes,
                                             int32_t length) const {
  DCHECK_GE(length, 4);
  double value =  parse_two_frames(bytes[1], bytes[0]);
  value -= 67.67;
  return value;
}

double WheelspeedAA::rear_left_wheel_speed(const std::uint8_t *bytes,
                                           int32_t length) const {
  DCHECK_GE(length, 6);
  double value = parse_two_frames(bytes[7], bytes[6]);
  value -= 67.67;
  return value;
}

double WheelspeedAA::rear_right_wheel_speed(const std::uint8_t *bytes,
                                            int32_t length) const {
  DCHECK_GE(length, 8);
  double value =  parse_two_frames(bytes[5], bytes[4]);
  value -= 67.67;
  return value;
}

double WheelspeedAA::parse_two_frames(const std::uint8_t low_byte,
                                      const std::uint8_t high_byte) const {
  Byte high_frame(&high_byte);
  int32_t high = high_frame.get_byte(0, 8);
  Byte low_frame(&low_byte);
  int32_t low = low_frame.get_byte(0, 8);
  int32_t value = (high << 8) | low;
  return value * 0.010000;
}

}  // namespace lexus_rx
}  // namespace canbus
}  // namespace apollo
