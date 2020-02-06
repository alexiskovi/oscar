#include "modules/canbus/vehicle/lexus_rx/protocol/steering_2e4.h"

#include "modules/drivers/canbus/common/byte.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

using ::apollo::drivers::canbus::Byte;

const int32_t Steering2E4::ID = 0x2e4;

const uint8_t Steering2E4::DLC = 5;

const int MAX_STEP_VALUE = 6;

uint32_t Steering2E4::GetPeriod() const {
  // receive rate??
  // receive timeout would trigger fault, letting en=0 and etc.
  static const uint32_t PERIOD = 16 * 1000;
  return PERIOD;
}

void Steering2E4::UpdateData(uint8_t *data) {
    set_counter_p( data );
    set_enable_p(data, steering_enable_);
    set_torque_p( data, curr_steering_angle_, goal_steering_angle_ );
    set_crc_p( data, getCRC(data) );
}

void Steering2E4::Reset() {
  counter = 0;
  speed_ = 0;
  counter_flag = false;
  curr_steering_angle_ = 0.0;
  goal_steering_angle_ = 0.0;
  steering_enable_ = false;
  clear_driver_override_flag_ = false;
  ignore_driver_override_ = false;
}


/* Set current value of steering angle */
Steering2E4 *Steering2E4::set_current_steering_angle(double angle) {
  curr_steering_angle_ = angle;
  return this;
}

/* Set goal value of steering angle */
Steering2E4 *Steering2E4::set_goal_steering_angle(double angle) {
  goal_steering_angle_ = angle;
  return this;
}



Steering2E4 *Steering2E4::set_enable() {
  AINFO << "LKA Enable steering";
  steering_enable_ = true;
  return this;
}

Steering2E4 *Steering2E4::set_disable() {
  if ( steering_enable_ ) {
     printf("Disable LKA steering\n");
     AINFO << "LKA Disable steering";
  }
  steering_enable_ = false;
  torque_ = 0;
  return this;
}

Steering2E4 *Steering2E4::set_current_torque(double torq) {
  current_torque = torq;
  return this;
}

Steering2E4 *Steering2E4::set_speed(double speed) {
  speed_ = speed;
  return this;
}


void Steering2E4::set_counter_p( uint8_t *data  ) {
    auto glob_cnt = this->getCounter();
    counter = static_cast<uint8_t>((3*glob_cnt)/2);
    counter = static_cast<uint8_t>( ( counter  % 64 ) + 1 );
    Byte frame(data + 0);
    frame.set_value(counter, 1, 6);

    frame.set_bit_1(7);
}


void Steering2E4::set_enable_p( uint8_t *data, bool flag ) {
     Byte frame(data + 0);
     if ( flag ) {
         frame.set_bit_1(0);
    
     }
     else {
         frame.set_bit_0(0);
     }
}

void Steering2E4::set_torque_p( uint8_t *data, double curr_angle, double goal_angle ) {
    /* Calculate torque for change steerng angle from curr_angle to goal_angle */
    /* TODO: Check current speed for calculate torque */
    double torque = 1;

    auto delta = goal_angle - curr_angle;
    printf("Current torque: %f; Previous: %f; delta: %f\n", current_torque, torque_, delta );
  
    //torque_ += (goal_angle - curr_angle)/50;
    auto upper_value = std::min( int(current_torque + 300), 1450 );
    auto lower_value = std::min( int(current_torque - 300), -1450 );
    


    int torque_delta = static_cast<int>(torque_ - current_torque);
    if ( ( delta > 0 ) || ( torque_ < lower_value ) ) {
        auto step = std::min(MAX_STEP_VALUE,int(torque + static_cast<int>( abs(torque_delta)/8 )));
        torque_ = static_cast<double>(torque_ + step);
        printf("New torque: %f; step: %f\n", torque_, step );
    }
    else if ( ( delta < 0 ) || ( torque_ > upper_value ) ){
        auto step = std::min(MAX_STEP_VALUE,int(torque + static_cast<int>( abs(torque_delta)/8 )));
        torque_ = static_cast<double>( torque_ - step );
        printf("New torque: %f; step: %f\n", torque_, step );
    }
    else {
        printf("Current torque %f is out of torque limit from %f to %f\n", lower_value, upper_value );
    }

    torque_ = std::min( torque_, current_torque + 200 );
    torque_ = std::max( torque_, current_torque - 200 );

    int32_t x = (int32_t)torque_;
    if ( steering_enable_ ) {
        printf("LKA is enabled. Speed: %f; torque: %f\n", speed_, torque_);
    }

    uint8_t t =  static_cast<uint8_t>( x & 0xFF );
    Byte frame_high(data + 2);
    frame_high.set_value(t, 0, 8);

    x >>= 8;
    t = static_cast<uint8_t>( x & 0xFF );
    Byte frame_low(data + 1);
    frame_low.set_value(t, 0, 8);

    prev_torque = torque_;
}

void Steering2E4::Parse(const std::uint8_t *bytes, int32_t length,
                       ChassisDetail *chassis_detail) const {

    //auto value = epas_torque(bytes, length);
    //chassis_detail->mutable_eps()->set_epas_torque(epas_torque(bytes, length));
    //chassis_detail->mutable_eps()->set_steering_angle_cmd(epas_torque(bytes, length));
}

void Steering2E4::Parse(const std::uint8_t *bytes, int32_t length,
                       const struct timeval &timestamp,
                       ChassisDetail *chassis_detail) const {

  chassis_detail->mutable_eps()->set_timestamp_65(
      static_cast<double>(timestamp.tv_sec) +
      static_cast<double>(timestamp.tv_usec) / 1000000.0);
  Parse(bytes, length, chassis_detail);
}


double Steering2E4::epas_torque(const std::uint8_t *bytes, int32_t length) const {
    Byte frame_high(bytes + 1);
    int32_t high = frame_high.get_byte(0, 8);
    Byte frame_low(bytes + 2);
    int32_t low = frame_low.get_byte(0, 8);
    int32_t value = (high << 8) | low;

    if (value > 0x8000) {

       value = value - 0x10000;
    }
    return value;
}

uint8_t Steering2E4::getCRC( uint8_t *bytes ) {
    uint8_t crc = 0;
    crc += DLC;
    crc += ID & 0xFF;
    crc += (ID & 0xFF00) >> 8;
    for ( int i=0; i < DLC-1; ++i ) {
        crc += bytes[i];
    }
    crc = crc & 0xFF;
    return crc;
}

void Steering2E4::set_crc_p( uint8_t *bytes, uint8_t crc ) {
    Byte frame(bytes + 4);
    frame.set_value( crc, 0, 8 );
}

} //lexus_rx
} //canbus
} //apollo


