#include "modules/canbus/vehicle/lexus_rx/protocol/steering_7f0.h"

#include "modules/drivers/canbus/common/byte.h"
#include <stdarg.h>
#include "modules/common/time/time.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

using ::apollo::drivers::canbus::Byte;

const int32_t Steering7F0::ID = 0x7f0;

const uint8_t Steering7F0::DLC = 5;

const int MAX_STEP_VALUE = 6;

//const double MIN_TORQUE = 30;
const double MIN_TORQUE = -85;
const double MAX_TORQUE = 85;

const double ABS_MIN_TORQUE = -75;
const double ABS_MAX_TORQUE = 65;
//const double MIN_TORQUE_RIGHT = 35;
const double MIN_TORQUE_RIGHT = -85;
const double MAX_TORQUE_RIGHT = -40;
const double MIN_TORQUE_LEFT = 40;
const double MAX_TORQUE_LEFT = 85;

const int MAX_LOG_SIZE = 200;



void Steering7F0::initFilename() {
   time_t rawtime;
   std::time(&rawtime);
   strftime(logfile, 80, "/apollo/data/log/steering_%F_%H%M%S.txt",
      localtime(&rawtime));
}

Steering7F0::Steering7F0() {
   mf_steer = new apollo::common::MeanFilter(1);
   mf_torque = new apollo::common::MeanFilter(1);

  /* Parking controller */
  apollo::control::PidSteerConf st_pid_conf;
  //st_pid_conf.set_kp(1.0);
  //st_pid_conf.set_kd(-0.5);
  //st_pid_conf.set_ki(1.0);
  st_pid_conf.set_kp(1.8);
  st_pid_conf.set_kd(-0.5);
  st_pid_conf.set_ki(0.1);
  st_pid_conf.set_integrator_enable( true );
  //st_pid_conf.set_integrator_saturation_level( 35.0 );
  //st_pid_conf.set_output_saturation_level( 35.0 );
  st_pid_conf.set_integrator_saturation_level( 25.0 );
  st_pid_conf.set_output_saturation_level( 25.0 );

  st_pid_controller.Init(st_pid_conf);
  st_pid_controller.Reset();

  /* Low speed controller */
  apollo::control::PidConf ls_pid_conf; //Low speed controller

  /* LAST TESTED
  ls_pid_conf.set_kp(1.2);
  ls_pid_conf.set_kd(-0.0025);
  ls_pid_conf.set_ki(0.2);
  ls_pid_conf.set_integrator_enable( true );
  ls_pid_conf.set_integrator_saturation_level( 10.0 );
  ls_pid_conf.set_output_saturation_level( 10.0 );
  */

  ls_pid_conf.set_kp(0.7); // 1.0
  ls_pid_conf.set_kd(0.0); //-0.013
  ls_pid_conf.set_ki(0.8); // 0.3
  ls_pid_conf.set_integrator_enable( true );
  ls_pid_conf.set_integrator_saturation_level( 85.0 );
  ls_pid_conf.set_output_saturation_level( 85.0 );

  ls_pid_controller.Init(ls_pid_conf);
  ls_pid_controller.Reset();
  dt = 0.012;
  /* */
  apollo::control::PidSteerConf hs_pid_conf; //Low speed controller
  hs_pid_conf.set_kp(2.2);
  hs_pid_conf.set_kd(-0.1);
  hs_pid_conf.set_ki(1.0);
  hs_pid_conf.set_integrator_enable( true );
  hs_pid_conf.set_integrator_saturation_level( 20.0 );
  hs_pid_conf.set_output_saturation_level( 20.0 );

  hs_pid_controller.Init(hs_pid_conf);
  hs_pid_controller.Reset();


}

uint32_t Steering7F0::GetPeriod() const {
  // receive rate??
  // receive timeout would trigger fault, letting en=0 and etc.
  static const uint32_t PERIOD = 12 * 1000;
  return PERIOD;
}

void Steering7F0::UpdateData(uint8_t *data) {
    set_torque_p( data, curr_steering_angle_, goal_steering_angle_ );
}

void Steering7F0::Reset() {
  counter = 0;
  speed_ = 0;
  counter_flag = false;
  curr_steering_angle_ = 0.0;
  goal_steering_angle_ = 0.0;
  steering_enable_ = false;
  clear_driver_override_flag_ = false;
  ignore_driver_override_ = false;
  last_torque = 0;
  last_direction = 0x00;
}


/* Set current value of steering angle */
Steering7F0 *Steering7F0::set_current_steering_angle(double angle) {
  //curr_steering_angle_ = mf_steer->Update(angle);
  curr_steering_angle_ = angle;
  return this;
}

/* Set goal value of steering angle */
Steering7F0 *Steering7F0::set_goal_steering_angle(double angle) {
  goal_steering_angle_ = angle;
  return this;
}

/* Set current value of steering angle */
Steering7F0 *Steering7F0::set_current_steering_angle_percent(double angle) {
  curr_steering_angle_percent_ = angle;
  return this;
}

/* Set goal value of steering angle */
Steering7F0 *Steering7F0::set_goal_steering_angle_percent(double angle) {
  goal_steering_angle_percent_ = angle;
  return this;
}

Steering7F0 *Steering7F0::set_enable() {
  if ( !steering_enable_ ) {
      initFilename();
  }
  steering_enable_ = true;
  return this;
}

Steering7F0 *Steering7F0::set_disable() {
  if ( steering_enable_ ) {
      AERROR << "Disable steering after enabled one";
  }
  torque_ = 0;
  last_torque = 0;
  this->Reset();
  return this;
}

Steering7F0 *Steering7F0::set_current_torque(double torq) {
  current_torque = torq;
  return this;
}

Steering7F0 *Steering7F0::set_speed(double speed) {
  speed_ = speed;
  return this;
}

void Steering7F0::set_torque_p( uint8_t *data, double curr_angle, double goal_angle ) {
    torque = 0;
    //steering_enable_ = false;
    if ( !steering_enable_ ) {
        for ( uint8_t l = 0; l < DLC; ++l ) {
            Byte frame(data + l);
            frame.set_value(0x00, 0, 8);
        }
    } else {
        double delta_angle = static_cast<double>(goal_angle - curr_angle);
        direction = 0x00;
        //printf("Curr angle: %f; goal angle: %f; delta: %f; last_torque: %f; speed: %f\n", curr_angle, goal_angle, delta_angle, last_torque, speed_);
        //if ( ( speed_ > 0 ) && ( speed_ <= 10 ) ) torque = ls_pid_controller.Control( delta_angle, dt );
        //double torque_val = ls_pid_controller.Control( curr_angle, goal_angle, dt );
        double torque_val = ls_pid_controller.Control( delta_angle, dt );
        /*
        if ( fabs( torque_val + last_torque ) < 15  && fabs( torque_val ) > 40 )  {
            torque_val = last_torque;
        }
        */
        //else if ( speed_ > 3 ) torque = hs_pid_controller.Control( delta_angle, dt );
        /*
	        else {
            printf("Use st_pid\n");
            torque = st_pid_controller.Control( delta_angle, dt );
        }
        */
        //AERROR << "Pid torque: " << torque_val;
	//torque_val = torque_val +  (curr_angle/5)*0.1; // last 0.3
        torque_val = std::max( torque_val, MIN_TORQUE );
        torque_val = std::min( torque_val, MAX_TORQUE );

        //torque_val += goal_steering_angle_ / 50;

        //torque_val = std::max( torque_val, ABS_MIN_TORQUE );
        //torque_val = std::min( torque_val, ABS_MAX_TORQUE );
        //AERROR << "Pid torque after filter 1: " << torque_val;

        //torque = mf_torque->Update( torque_val );
	/*
        if ( fabs(goal_steering_angle_) > 70 ) {
            if ( goal_steering_angle_ > 0 ) {
                torque_val = std::max( torque_val, MIN_TORQUE_LEFT );
                torque_val = std::min( torque_val, MAX_TORQUE_LEFT );
            }
            if ( goal_steering_angle_ < 0 ) {
                torque_val = std::max( torque_val, MIN_TORQUE_RIGHT );
                torque_val = std::min( torque_val, MAX_TORQUE_RIGHT );
            }
        }
	*/
        //torque = mf_torque->Update( torque_val );
        torque = torque_val;

        //AERROR << "Pid torque after filter 2: " << torque;

        last_torque = torque;
        if ( torque > 0 ) {
            direction = 0x01;
            //printf("Direction %x. Left\n", direction );
        }
        if ( torque < 0 ) {
            direction = 0x02;
            //printf("Direction %x. Right\n", direction );
        }

        //write_steering_log( ";%f;%f;%f;%f;%f;%f;%f;%f;%f", curr_angle, goal_angle, delta_angle, speed_, torque, last_torque,  ls_pid_controller.getProportional(), ls_pid_controller.getIntegral(), ls_pid_controller.getDifferential());

        torque = fabs(torque);


        Byte frame_0(data);
        frame_0.set_value(direction, 0, 4);
        frame_0.set_value(0x01, 4, 4 );
         
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

        last_direction = direction;
        //prev_delta = mf_delta->Update(delta_angle);
    }
}

void Steering7F0::Parse(const std::uint8_t *bytes, int32_t length,
                       ChassisDetail *chassis_detail) const {

    /*
    printf("Steering 0x7f0:\n");
    for (int i=0; i < length; ++i ) printf("%02x ", bytes[i]);
    printf("\n");
    */
    //chassis_detail->mutable_eps()->set_epas_torque(epas_torque(bytes, length));
    chassis_detail->mutable_eps()->set_steering_angle_cmd(epas_torque(bytes, length));
}

void Steering7F0::Parse(const std::uint8_t *bytes, int32_t length,
                       const struct timeval &timestamp,
                       ChassisDetail *chassis_detail) const {

  chassis_detail->mutable_eps()->set_timestamp_65(
      static_cast<double>(timestamp.tv_sec) +
      static_cast<double>(timestamp.tv_usec) / 1000000.0);
  Parse(bytes, length, chassis_detail);
}


double Steering7F0::epas_torque(const std::uint8_t *bytes, int32_t length) const {
    Byte frame_high(bytes + 1);
    int32_t high = frame_high.get_byte(0, 8);
    Byte frame_low(bytes + 2);
    int32_t low = frame_low.get_byte(0, 8);
    //printf("Epas torque %02x %02x\n", high, low );
    int32_t value = (high << 8) | low;

    if (value > 0x8000) {
       //printf("Epas torque reverse value: %d(%04x)\n", value, value );
       value = value - 0x10000;
    }

    //printf("Epas torque: %d(%04x)\n", value, value );
    return value;
}

uint8_t Steering7F0::getCRC( uint8_t *bytes ) {
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

void Steering7F0::set_crc_p( uint8_t *bytes, uint8_t crc ) {
    Byte frame(bytes + 4);
    frame.set_value( crc, 0, 8 );
}


void Steering7F0::write_steering_log( const char * str,... ) {
    char *buffer = new char[MAX_LOG_SIZE];
    va_list ap;
    va_start (ap, str);
    vsnprintf (buffer,MAX_LOG_SIZE, str, ap);
    va_end(ap);
    write_steering_log_process( buffer );
    delete [] buffer;
}

void Steering7F0::write_steering_log_process( const char * buffer ) {
    time_t now_t;
    now_t = time(NULL);
    struct tm * nt;
    nt = localtime(&now_t);

    char time_string[80]; /* Время для записи лога */
    strftime (time_string,80,"%Y-%m-%d %X",nt);

    FILE * f;
    f = fopen( logfile, "a+" );
    if ( f !=NULL )
    {
        fprintf( f, "%s\t%s\n", time_string, buffer );
        fclose( f );
    }
}

} //lexus_rx
} //canbus
} //apollo


