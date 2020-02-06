#include "modules/canbus/vehicle/lexus_rx/protocol/steering_41.h"

#include "modules/drivers/canbus/common/byte.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

using ::apollo::drivers::canbus::Byte;

const int32_t Steering41::ID = 0x41;

const uint8_t Steering41::DLC = 5;

const int MAX_STEP_VALUE = 6;

const double MIN_TORQUE = 40;
const double MAX_TORQUE = 65;
const double MIN_TORQUE_RIGHT = 50;
const double MAX_TORQUE_RIGHT = 75;

uint32_t Steering41::GetPeriod() const {
  // receive rate??
  // receive timeout would trigger fault, letting en=0 and etc.
  static const uint32_t PERIOD = 50 * 1000;
  return PERIOD;
}

void Steering41::UpdateData(uint8_t *data) {
    set_torque_p( data, curr_steering_angle_, goal_steering_angle_ );
}

void Steering41::Reset() {
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
Steering41 *Steering41::set_current_steering_angle(double angle) {
  curr_steering_angle_ = angle;
  return this;
}

/* Set goal value of steering angle */
Steering41 *Steering41::set_goal_steering_angle(double angle) {
  goal_steering_angle_ = angle;
  return this;
}



Steering41 *Steering41::set_enable() {
  steering_enable_ = true;
  return this;
}

Steering41 *Steering41::set_disable() {
  steering_enable_ = false;
  torque_ = 0;
  return this;
}

Steering41 *Steering41::set_current_torque(double torq) {
  current_torque = torq;
  return this;
}

Steering41 *Steering41::set_speed(double speed) {
  speed_ = speed;
  return this;
}

void Steering41::set_torque_p( uint8_t *data, double curr_angle, double goal_angle ) {
    double torque = 1;
    //steering_enable_ = true;
    if ( !steering_enable_ ) {
        AERROR << "Steering is disable";
        for ( uint8_t l = 0; l < DLC; ++l ) {
            Byte frame(data + l);
            frame.set_value(0x00, 0, 8);
        }
    } else {
        double delta = static_cast<double>(goal_angle - curr_angle);
        uint8_t direction = 0x00;
        torque = 0;
        if ( delta > 5 ) {
            direction = 0x01;
            torque = fabs(delta/7.0) + 15.0;
            AINFO << "Calculate torque left: " << torque;

            torque = std::max( torque, MIN_TORQUE );
            torque = std::min( torque, MAX_TORQUE );
        }
        else if ( delta < -5 ) {
            direction = 0x02;
            torque = fabs(delta/6.0) + 15.0;
            AINFO << "Calculate torque right: " << torque;

            torque = std::max( torque, MIN_TORQUE_RIGHT );
            torque = std::min( torque, MAX_TORQUE_RIGHT );
        }
        else if ( fabs(delta) <= 5 ) direction = 0x00;

        torque -= 5*speed_;
        torque = std::max( torque, 0.0 );
        AINFO << "Result torque: " << torque << "; speed: " << speed_;
        /* */
        Byte frame_0(data);
        frame_0.set_value(direction, 0, 8);
        /* */
        Byte frame_1(data+1);
        frame_1.set_value(0x00, 0, 8);
        /* */
        Byte frame_2(data+2);
        frame_2.set_value(static_cast<uint8_t>(torque), 0, 8);
        /* */
        for ( uint8_t l = 3; l < DLC; ++l ) {
            Byte frame(data + l);
            frame.set_value(0x00, 0, 8);
        }
    }
}

void Steering41::Parse(const std::uint8_t *bytes, int32_t length,
                       ChassisDetail *chassis_detail) const {

    //chassis_detail->mutable_eps()->set_epas_torque(epas_torque(bytes, length));
    //chassis_detail->mutable_eps()->set_steering_angle_cmd(epas_torque(bytes, length));
}

void Steering41::Parse(const std::uint8_t *bytes, int32_t length,
                       const struct timeval &timestamp,
                       ChassisDetail *chassis_detail) const {

  chassis_detail->mutable_eps()->set_timestamp_65(
      static_cast<double>(timestamp.tv_sec) +
      static_cast<double>(timestamp.tv_usec) / 1000000.0);
  Parse(bytes, length, chassis_detail);
}


double Steering41::epas_torque(const std::uint8_t *bytes, int32_t length) const {
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

uint8_t Steering41::getCRC( uint8_t *bytes ) {
    uint8_t crc = 0;
    crc = static_cast<uint8_t>( crc + DLC );
    crc = static_cast<uint8_t>( crc + (ID & 0xFF) );
    crc = static_cast<uint8_t>( crc + ( (ID & 0xFF00) >> 8 ) );
    for ( int i=0; i < DLC-1; ++i ) {
        crc = static_cast<uint8_t>( crc + bytes[i] );
    }
    crc = static_cast<uint8_t>( crc & 0xFF );
    return crc;
}

void Steering41::set_crc_p( uint8_t *bytes, uint8_t crc ) {
    Byte frame(bytes + 4);
    frame.set_value( crc, 0, 8 );
}

} //lexus_rx
} //canbus
} //apollo


