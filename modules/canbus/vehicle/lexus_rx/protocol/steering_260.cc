#include "modules/canbus/vehicle/lexus_rx/protocol/steering_260.h"

#include "modules/drivers/canbus/common/byte.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

using ::apollo::drivers::canbus::Byte;

const int32_t Steering260::ID = 0x260;

void Steering260::Parse(const std::uint8_t *bytes, int32_t length,
                       ChassisDetail *chassis_detail) const {
    auto driver_torque_ = driver_torque( bytes, length );
    auto epas_torque_ = eps_torque( bytes, length );
    auto driver_override_ = driver_override( bytes, length );
    
    chassis_detail->mutable_eps()->set_eps_driver_hand_torq( driver_torque_ );
    
    chassis_detail->mutable_eps()->set_epas_torque( epas_torque_ );

    chassis_detail->mutable_eps()->set_driver_override( driver_override_ );
    AINFO << "Driver: " << driver_torque_ << "; Override: " << driver_override_;
    AINFO << "Epas torque:" << epas_torque_;
}

void Steering260::Parse(const std::uint8_t *bytes, int32_t length,
                       const struct timeval &timestamp,
                       ChassisDetail *chassis_detail) const {

  chassis_detail->mutable_eps()->set_timestamp_65(
      static_cast<double>(timestamp.tv_sec) +
      static_cast<double>(timestamp.tv_usec) / 1000000.0);
  Parse(bytes, length, chassis_detail);
}

double Steering260::driver_torque(const std::uint8_t *bytes, int32_t length) const {
    Byte frame_high(bytes + 1);
    int32_t high = frame_high.get_byte(0, 8);
    Byte frame_low(bytes + 2);
    int32_t low = frame_low.get_byte(0, 8);
    int32_t value = (high << 8) | low;
    if (value > 0x800) {
        value = value - 0x10000;
    }
    return (double)value;
}

double Steering260::eps_torque(const std::uint8_t *bytes, int32_t length) const {
    Byte frame_high(bytes + 5);
    int32_t high = frame_high.get_byte(0, 8);
    Byte frame_low(bytes + 6);
    int32_t low = frame_low.get_byte(0, 8);
    int32_t value = (high << 8) | low;
    if (value > 0x8000) {
        value = value - 0x10000;
    }
    return (double)value;
}

bool Steering260::driver_override(const std::uint8_t *bytes, int32_t length) const {
    Byte frame(bytes + 0);
    return frame.is_bit_1(0);
}

} //lexus_rx
} //canbus
} //apollo


