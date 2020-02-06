#include "modules/canbus/vehicle/lexus_rx/protocol/steering_7f1.h"

#include "modules/drivers/canbus/common/byte.h"
#include <stdarg.h>
#include "modules/common/time/time.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

using ::apollo::drivers::canbus::Byte;

const int32_t Steering7F1::ID = 0x7f1;

const uint8_t Steering7F1::DLC = 5;

uint32_t Steering7F1::GetPeriod() const {
  static const uint32_t PERIOD = 24 * 1000;
  return PERIOD;
}

void Steering7F1::UpdateData(uint8_t *data) {
    Byte frame(data);
    if ( !steering_enable_ ) {
        if ( acc_off_cnt <=5 ) {
        //if ( acc_button_on ) {
            AERROR << "Previous ACC ON button state is ON. Try to disable ACC ON button. Cnt: " << acc_off_cnt;
            frame.set_value(0x04, 0, 8);
            /* Enable acc ON button */
            acc_button_on = false;
        }
        else if ( acc_off_cnt <= 10 ) {
        //else if ( acc_button_set ) {
            AERROR << "Previous ACC SET button state is ON. Try to disable ACC SET button. Cnt: " << acc_off_cnt;
            frame.set_value(0x00, 0, 8);
            /* Enable acc SET- button */
            acc_button_set = false;
        }
        else {
            frame.set_value(0x00, 0, 8);
        }
        for ( uint8_t l = 1; l < DLC; ++l ) {
            Byte frame(data + l);
            frame.set_value(0x00, 0, 8);
        }
        acc_off_cnt++;
        acc_on_cnt = 0;
    }
    else {
        if ( acc_on_cnt <=5 ) {
        //if ( !acc_button_on ) {
            AERROR << "Previous ACC ON button state is OFF. Try to enable ACC ON button. Cnt: " << acc_on_cnt;
            frame.set_value(0x05, 0, 8);
            /* Enable acc ON button */
            acc_button_on = true;
        }
        else if ( ( acc_on_cnt <=310 ) && ( acc_on_cnt >=300  ) )  {
        //else if ( !acc_button_set ) {
            AERROR << "Previous ACC SET button state is OFF. Try to enable ACC SET button. Cnt: " << acc_on_cnt;
            frame.set_value(0x09, 0, 8);
            /* Enable acc SET- button */
            acc_button_set = true;
        }
        else {
            frame.set_value(0x01, 0, 8);
        }
        for ( uint8_t l = 1; l < DLC; ++l ) {
            Byte frame(data + l);
            frame.set_value(0x00, 0, 8);
        }
        acc_on_cnt++;
        acc_off_cnt = 0;
    }
    /* */
    /*
    for (int z=0; z < 8; z++ ) {
       printf("%02x ", data[z]);
    }
    printf("\n");
    */
}

void Steering7F1::Reset() {
  steering_enable_ = false;
}

Steering7F1 *Steering7F1::set_enable() {
  steering_enable_ = true;
  return this;
}

Steering7F1 *Steering7F1::set_disable() {
  this->Reset();
  return this;
}

} //lexus_rx
} //canbus
} //apollo


