#include "modules/canbus/vehicle/lexus_rx/protocol/mileage_611.h"

#include "glog/logging.h"

#include "modules/drivers/canbus/common/byte.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

using ::apollo::drivers::canbus::Byte;

const int32_t Mileage611::ID = 0x611;

void Mileage611::Parse(const std::uint8_t *bytes, int32_t length,
                         ChassisDetail *chassis_detail) const {
  chassis_detail->mutable_basic()->set_odo_meter(
      engine_odometer(bytes, length));
}

void Mileage611::Parse(const std::uint8_t *bytes, int32_t length,
                         const struct timeval &timestamp,
                         ChassisDetail *chassis_detail) const {
  chassis_detail->mutable_eps()->set_timestamp_65(
      static_cast<double>(timestamp.tv_sec) +
      static_cast<double>(timestamp.tv_usec) / 1000000.0);
  Parse(bytes, length, chassis_detail);
}

double Mileage611::engine_odometer(const std::uint8_t *bytes,
                                            int32_t length) const {
  Byte frame1(bytes + 4);
  int32_t val1 = frame1.get_byte(0, 8);
  Byte frame2(bytes + 5);
  int32_t val2 = frame2.get_byte(0, 8);
  Byte frame3(bytes + 6);
  int32_t val3 = frame3.get_byte(0, 8);
  Byte frame4(bytes + 7);
  int32_t val4 = frame4.get_byte(0, 8);

  int32_t value = (val1 << 24) | (val2 << 16) | (val3 << 8) | val4;
  return double(value);
}

}  // namespace lexus_rx
}  // namespace canbus
}  // namespace apollo
