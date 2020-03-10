#include "modules/canbus/vehicle/lexus_rx/protocol/gear_777.h"

#include "modules/drivers/canbus/common/byte.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

using ::apollo::drivers::canbus::Byte;

const int32_t Gear777::ID = 0x7FA;

const uint8_t Gear777::DLC = 8;

const uint8_t GEAR_NONE = 0x00;

const uint8_t GEAR_REVERSE = 0x02;

const uint8_t GEAR_NEUTRAL = 0x03;

const uint8_t GEAR_DRIVE = 0x04;


uint32_t Gear777::GetPeriod() const {
  // receive rate??
  // receive timeout would trigger fault, letting en=0 and etc.
  static const uint32_t PERIOD = 80 * 1000;
  return PERIOD;
}

void Gear777::UpdateData(uint8_t *data) {
    set_counter_p( data );
    set_gear_p(data, gear_position_);
}

void Gear777::Reset() {
  gear_position_ = GEAR_NEUTRAL;
  drive_position_counter_ = 0;
}

void Gear777::set_gear_p( uint8_t *data, uint8_t gear_pos ) {
  uint8_t t =  static_cast<uint8_t>( gear_pos & 0xFF );
  Byte frame_high(data + 1);
  frame_high.set_value(t, 0, 8);
   
}

void Gear777::set_counter_p( uint8_t *data ) {
  //auto glob_cnt = this->getCounter();
  //uint8_t cnt = glob_cnt & 0x0E ;
  Byte frame_high(data);
  frame_high.set_value(0xFA, 0, 8);
}

/* Neutral */
  void Gear777::set_gear_neutral() {
    gear_position_ = GEAR_NEUTRAL;
    drive_position_counter_ = 0;
  }

  /* Reverse */
  void Gear777::set_gear_reverse() {
    gear_position_ = GEAR_REVERSE;
  }

  /* Drive */
  void Gear777::set_gear_drive() {
    printf("Current counter: %ld\n", drive_position_counter_);
    drive_position_counter_++;
    if ( drive_position_counter_ >= 70 ) {
	if ( gear_position_ != GEAR_DRIVE ) {
	    printf("Swith GEAR to DRIVE at %ld counter\n", drive_position_counter_ );
	}
        gear_position_ = GEAR_DRIVE;
    }
  }

  /* Parking */
  void Gear777::set_gear_park() {
    gear_position_ = GEAR_NONE;
  }

  /* Parking */
  void Gear777::set_gear_none() {
    gear_position_ = GEAR_NONE;
  }

} //lexus_rx
} //canbus
} //apollo
