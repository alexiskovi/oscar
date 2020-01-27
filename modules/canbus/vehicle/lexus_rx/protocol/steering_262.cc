#include "modules/canbus/vehicle/lexus_rx/protocol/steering_262.h"

#include "modules/drivers/canbus/common/byte.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

using ::apollo::drivers::canbus::Byte;

const int32_t Steering262::ID = 0x262;

void Steering262::Parse(const std::uint8_t *bytes, int32_t length,
                       ChassisDetail *chassis_detail) const {
    //auto ipas_state_ = ipas_state(bytes, length );
    //auto lka_type_ = lka_type(bytes, length );
    auto lka_state_ = lka_state(bytes, length );
    if ( lka_state_ > 5 ) {
        chassis_detail->mutable_eps()->set_channel_1_fault(true);
        chassis_detail->mutable_eps()->set_channel_2_fault(true);
        printf("LKA state error!!!\n");
    }
    else {
        chassis_detail->mutable_eps()->set_channel_1_fault(false);
        chassis_detail->mutable_eps()->set_channel_2_fault(false);
    }
}

void Steering262::Parse(const std::uint8_t *bytes, int32_t length,
                       const struct timeval &timestamp,
                       ChassisDetail *chassis_detail) const {
  chassis_detail->mutable_eps()->set_timestamp_65(
      static_cast<double>(timestamp.tv_sec) +
      static_cast<double>(timestamp.tv_usec) / 1000000.0);
  Parse(bytes, length, chassis_detail);
}

uint8_t Steering262::ipas_state(const std::uint8_t *bytes, int32_t length) const {
    Byte frame(bytes + 0);
    uint8_t ipas_state = frame.get_byte(0, 4);
    return ipas_state;
}

uint8_t Steering262::lka_type( const std::uint8_t *bytes, int32_t length) const {
    Byte frame(bytes + 3);
    uint8_t lka_type = frame.get_byte(0, 1);
    return lka_type;
}


uint8_t Steering262::lka_state( const std::uint8_t *bytes, int32_t length) const {
    Byte frame(bytes + 3);
    uint8_t lka_state = frame.get_byte(0, 8);
    return lka_state/2;
}

} //lexus_rx
} //canbus
} //apollo


