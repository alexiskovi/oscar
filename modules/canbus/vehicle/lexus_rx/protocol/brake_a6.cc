#include "modules/canbus/vehicle/lexus_rx/protocol/brake_a6.h"

#include "glog/logging.h"

#include "modules/drivers/canbus/common/byte.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

using ::apollo::drivers::canbus::Byte;

const int32_t BrakeA6::ID = 0xA6;

void BrakeA6::Parse(const std::uint8_t *bytes, int32_t length,
                         ChassisDetail *chassis_detail) const {

  auto brake_out = brake_pedal(bytes, length);
  //printf("Brake out: %f\n", brake_out);
  chassis_detail->mutable_brake()->set_brake_pedal_position(
      brake_amount(bytes, length));
  chassis_detail->mutable_brake()->set_brake_output(
      brake_out);
}

void BrakeA6::Parse(const std::uint8_t *bytes, int32_t length,
                         const struct timeval &timestamp,
                         ChassisDetail *chassis_detail) const {
  chassis_detail->mutable_eps()->set_timestamp_65(
      static_cast<double>(timestamp.tv_sec) +
      static_cast<double>(timestamp.tv_usec) / 1000000.0);
  Parse(bytes, length, chassis_detail);
}

double BrakeA6::brake_pedal(const std::uint8_t *bytes,
                                            int32_t length) const {

  Byte frame_high(bytes + 2);
  int32_t value = frame_high.get_byte(0, 8);
  double val = double(value)/2.55;
  return val;
}

double BrakeA6::brake_amount(const std::uint8_t *bytes,
                                            int32_t length) const {
  Byte frame_high(bytes + 0);
  int32_t value = frame_high.get_byte(0, 8);
  double val = double(value)/255;
  return val;
}

}  // namespace lexus_rx
}  // namespace canbus
}  // namespace apollo
