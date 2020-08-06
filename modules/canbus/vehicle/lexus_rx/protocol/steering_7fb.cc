#include "modules/canbus/vehicle/lexus_rx/protocol/steering_7fb.h"

#include "modules/drivers/canbus/common/byte.h"
#include <stdarg.h>
#include "modules/common/time/time.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

using ::apollo::drivers::canbus::Byte;

const int32_t Steering7FB::ID = 0x7fb;

const uint8_t Steering7FB::DLC = 8;

uint32_t Steering7FB::GetPeriod() const {
  static const uint32_t PERIOD = 24 * 1000;
  return PERIOD;
}

void Steering7FB::UpdateData(uint8_t *data) {
    Byte frame_0(data);
    frame_0.set_value(0x4f, 0, 8);

    Byte frame_1(data+1);
    frame_1.set_value(0x53, 0, 8);

    Byte frame_2(data+2);
    frame_2.set_value(0x43, 0, 8);

    Byte frame_3(data+3);
    frame_3.set_value(0x41, 0, 8);

    Byte frame_4(data+4);
    frame_4.set_value(0x52, 0, 8);


    Byte frame_5(data+5);
    if ( !steering_enable_ ) {
        if ( acc_res_off_cnt <= 10 ) {
            AERROR << "Previous ACC SET button state is ON. Try to disable ACC SET button. Cnt: " << acc_res_off_cnt;
            frame_5.set_value(0x00, 0, 8);
            /* Enable acc RES+ button */
            acc_res_on = false;
        }
        else {
            frame_5.set_value(0x00, 0, 8);
        }
        for ( uint8_t l = 6; l < DLC; ++l ) {
            Byte frame(data + l);
            frame.set_value(0x00, 0, 8);
        }
        acc_res_off_cnt++;
        acc_res_on_cnt = 0;
    }
    else {
        if ( ( ( acc_res_on_cnt <= 690 ) && ( acc_res_on_cnt >= 640  ) ) || 
           ( ( acc_res_on_cnt <= 970 ) && ( acc_res_on_cnt >= 890  ) ) )	{
            AERROR << "!!! Previous ACC RES button state is OFF. Try to enable ACC RES button. Cnt: " << acc_res_on_cnt;
            frame_5.set_value(0x01, 0, 8);
            /* Enable acc RES+ button */
            acc_res_on = true;
        }
        else {
            frame_5.set_value(0x00, 0, 8);
        }
        for ( uint8_t l = 6; l < DLC; ++l ) {
            Byte frame(data + l);
            frame.set_value(0x00, 0, 8);
        }
        acc_res_on_cnt++;
        acc_res_off_cnt = 0;
    }
}

void Steering7FB::Reset() {
  steering_enable_ = false;
}

Steering7FB *Steering7FB::set_enable() {
  steering_enable_ = true;
  return this;
}

Steering7FB *Steering7FB::set_disable() {
  this->Reset();
  return this;
}

} //lexus_rx
} //canbus
} //apollo


