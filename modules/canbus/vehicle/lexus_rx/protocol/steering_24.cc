#include "modules/canbus/vehicle/lexus_rx/protocol/steering_24.h"

#include "modules/drivers/canbus/common/byte.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

using ::apollo::drivers::canbus::Byte;

const int32_t Steering24::ID = 0x24;

void Steering24::Parse(const std::uint8_t *bytes, int32_t length,
                       ChassisDetail *chassis_detail) const {

    chassis_detail->mutable_vehicle_spd()->set_lat_acc( accel_y( bytes, length ) );
    
    chassis_detail->mutable_vehicle_spd()->set_yaw_rate( yaw_rate( bytes, length ) );

    //chassis_detail->mutable_eps()->set_epas_torque( steering_torque( bytes, length ) );

}

void Steering24::Parse(const std::uint8_t *bytes, int32_t length,
                       const struct timeval &timestamp,
                       ChassisDetail *chassis_detail) const {
  chassis_detail->mutable_eps()->set_timestamp_65(
      static_cast<double>(timestamp.tv_sec) +
      static_cast<double>(timestamp.tv_usec) / 1000000.0);
  Parse(bytes, length, chassis_detail);
}

double Steering24::accel_y(const std::uint8_t *bytes, int32_t length) const {
    Byte frame_high(bytes + 4);
    int32_t high = frame_high.get_byte(1, 8);
    Byte frame_low(bytes + 5);
    int32_t low = frame_low.get_byte(0, 3);
    int32_t value = (high << 8) | low;
    value -= 512;

    return (double)value/1000;
}

double Steering24::yaw_rate(const std::uint8_t *bytes, int32_t length) const {
    Byte frame_high(bytes + 0);
    int32_t high = frame_high.get_byte(0, 8);
    Byte frame_low(bytes + 1);
    int32_t low = frame_low.get_byte(0, 3);
    int32_t value = (high << 8) | low;
    value -= 512;
    return (double)value;
}

double Steering24::steering_torque(const std::uint8_t *bytes, int32_t length) const {
    Byte frame_high(bytes + 2);
    int32_t high = frame_high.get_byte(0, 2);
    Byte frame_low(bytes + 3);
    int32_t low = frame_low.get_byte(0, 8);
    int32_t value = (high << 8) | low;
    value -= 512;
    return (double)value;
}

} //lexus_rx
} //canbus
} //apollo


