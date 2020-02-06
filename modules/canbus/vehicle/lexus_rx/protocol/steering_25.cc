#include "modules/canbus/vehicle/lexus_rx/protocol/steering_25.h"

#include "modules/drivers/canbus/common/byte.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

using ::apollo::drivers::canbus::Byte;

const int32_t Steering25::ID = 0x25;
const int32_t THRESHOLD = 20.0;
double old_value_ = -1000;

Steering25::Steering25() {
    mean_filter_steer_ = new apollo::common::MeanFilter(10);
}

void Steering25::Parse(const std::uint8_t *bytes, int32_t length,
                       ChassisDetail *chassis_detail) const {
                         
  double val = steering_angle(bytes, length);

  //threshold filter
  //first hit
  //if (old_value_ == -1000) old_value_ = val;
  //if (fabs(val - old_value_) > THRESHOLD) val = old_value_;
  //old_value_ = val;
  //mean filter (from Apollo)
  //val = mean_filter_steer_ -> Update(val);


  chassis_detail->mutable_eps()->set_steering_angle(val);
  // no steering angle speed

  chassis_detail->mutable_eps()->set_steering_angle_spd( steering_angle_speed( bytes, length ) );

  chassis_detail->mutable_eps()->set_is_steering_angle_valid(true);

  chassis_detail->mutable_eps()->set_steering_enabled(true);
}

void Steering25::Parse(const std::uint8_t *bytes, int32_t length,
                       const struct timeval &timestamp,
                       ChassisDetail *chassis_detail) const {
  chassis_detail->mutable_eps()->set_timestamp_65(
      static_cast<double>(timestamp.tv_sec) +
      static_cast<double>(timestamp.tv_usec) / 1000000.0);
  Parse(bytes, length, chassis_detail);
}


uint32_t Steering25::GetPeriod() const {
  // receive rate??
  // receive timeout would trigger fault, letting en=0 and etc.
  static const uint32_t PERIOD = 12 * 1000; // 12 ms
  return PERIOD;
}

void Steering25::UpdateData(uint8_t *data) {
}

void Steering25::Reset() {
  //mean_filter_steer_ = common::MeanFilter(static_cast<int32_t>(FILTER_WIDTH));
  //mean_filter_steer_ = new apollo::common::MeanFilter(10);
}

Steering25 *Steering25::set_enable() {
  steering_enable_ = true;
  return this;
}

Steering25 *Steering25::set_disable() {
  steering_enable_ = false;
  return this;
}

// private


double Steering25::steering_angle(const std::uint8_t *bytes,
                                  int32_t length) const {

  Byte frame_high(bytes + 0);
  int32_t high = frame_high.get_byte(0, 8);
  Byte frame_low(bytes + 1);
  int32_t low = frame_low.get_byte(0, 8);
  int32_t value = (high << 8) | low;

  if (value > 0x800) {
    value = value - 0x1000;
  }
  return value * 1.500000;

}

/*
 * steering range speed between -2000 and 2000
 */

double Steering25::steering_angle_speed( const std::uint8_t *bytes,
                                  int32_t length) const {
    Byte frame_high(bytes + 4);
    int32_t high = frame_high.get_byte(4, 4);
    Byte frame_low(bytes + 5);
    int32_t low = frame_low.get_byte(0, 6);
    int32_t value = (high << 8) | low;

    if (value > 0x800) {
       value = value - 0x1000;
    }
    return value;
}

}  // namespace lexus_rx
}  // namespace canbus
}  // namespace apollo


