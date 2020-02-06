#include "modules/canbus/vehicle/lexus_rx/protocol/accel_228.h"

#include "glog/logging.h"

#include "modules/drivers/canbus/common/byte.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

using ::apollo::drivers::canbus::Byte;

const int32_t Accel228::ID = 0x228;

void Accel228::Parse(const std::uint8_t *bytes, int32_t length,
                         ChassisDetail *chassis_detail) const {
   chassis_detail->mutable_vehicle_spd()->set_long_acc(accel_x(bytes, length));
   chassis_detail->mutable_vehicle_spd()->set_vert_acc(accel_z(bytes, length));
}

void Accel228::Parse(const std::uint8_t *bytes, int32_t length,
                         const struct timeval &timestamp,
                         ChassisDetail *chassis_detail) const {
  chassis_detail->mutable_vehicle_spd()->set_timestamp_sec(
      timestamp.tv_sec + timestamp.tv_usec / 1000000.0);
  Parse(bytes, length, chassis_detail);
}


double Accel228::accel_x(const std::uint8_t *bytes, int32_t length) const {
  // Pedal Command from the command message
  Byte frame_high(bytes + 0);
  int32_t high = frame_high.get_byte(0, 7);
  Byte frame_low(bytes + 1);
  int32_t low = frame_low.get_byte(0, 8);
  int32_t value = (high << 8) | low;
  return double(value)/1000;
}

double Accel228::accel_z(const std::uint8_t *bytes, int32_t length) const {
  // Pedal Command from the command message
  Byte frame_high(bytes + 2);
  int32_t high = frame_high.get_byte(0, 8);
  Byte frame_low(bytes + 3);
  int32_t low = frame_low.get_byte(0, 8);
  int32_t value = (high << 8) | low;
  return double(value)/1000;
}



}  // namespace lexus_rx
}  // namespace canbus
}  // namespace apollo
