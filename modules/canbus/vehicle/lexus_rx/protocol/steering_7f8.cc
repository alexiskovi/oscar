#include "modules/canbus/vehicle/lexus_rx/protocol/steering_7f8.h"

#include "modules/drivers/canbus/common/byte.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

using ::apollo::drivers::canbus::Byte;

const int32_t Steering7F8::ID = 0x7F8;

void Steering7F8::Parse(const std::uint8_t *bytes, int32_t length,
                       ChassisDetail *chassis_detail) const {
    auto lka_state_ = lka_state(bytes, length );
    auto start_btn_state_ = start_btn_state( bytes, length );
    if ( start_btn_state_ == 0x01 ){
        chassis_detail->mutable_basic()->set_acc_on_off_button(true);
    }
    else {
        chassis_detail->mutable_basic()->set_acc_on_off_button(false);
    }
    if ( lka_state_ == 0x04 ) {
        chassis_detail->mutable_eps()->set_channel_1_fault(true);
        AERROR << "Can't enable Steering control. Breake pedal is pressed";
    }
    else {
        chassis_detail->mutable_eps()->set_channel_1_fault(false);
    }

    if ( lka_state_ == 0x08 ) {
        chassis_detail->mutable_eps()->set_channel_2_fault(true);
        AERROR << "Can't enable Steering control. Speed is more than awailable limit";
    }
    else {
        chassis_detail->mutable_eps()->set_channel_1_fault(false);
        chassis_detail->mutable_eps()->set_channel_2_fault(false);
    }
}

void Steering7F8::Parse(const std::uint8_t *bytes, int32_t length,
                       const struct timeval &timestamp,
                       ChassisDetail *chassis_detail) const {
  chassis_detail->mutable_eps()->set_timestamp_65(
      timestamp.tv_sec + timestamp.tv_usec / 1000000.0);
  Parse(bytes, length, chassis_detail);
}

uint8_t Steering7F8::lka_state( const std::uint8_t *bytes, int32_t length) const {
    Byte frame(bytes);
    uint8_t lka_state = frame.get_byte(0, 8);
    return lka_state;
}

uint8_t Steering7F8::start_btn_state( const std::uint8_t *bytes, int32_t length) const {
    Byte frame(bytes+2);
    uint8_t state = frame.get_byte(0, 8);
    return state;
}


} //lexus_rx
} //canbus
} //apollo


