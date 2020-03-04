#include "modules/canbus/vehicle/lexus_rx/protocol/throttle_343.h"

#include "modules/drivers/canbus/common/byte.h"

#include "modules/common/time/time.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

using ::apollo::drivers::canbus::Byte;

const int MAX_LOG_SIZE = 200;

// public

const int32_t Throttle343::ID = 0x343;

const uint8_t Throttle343::DLC = 8;


void Throttle343::initFilename() {
   time_t rawtime;
   std::time(&rawtime);
   strftime(logfile, 80, "/apollo/data/log/throttle_%F_%H%M%S.txt",
      localtime(&rawtime));
}

uint32_t Throttle343::GetPeriod() const {
  static const uint32_t PERIOD = 30 * 1000;
  return PERIOD;
}

void Throttle343::Parse(const std::uint8_t *bytes, int32_t length,
                         ChassisDetail *chassis_detail) const {
   //auto gas = throttle_cmd(bytes, length)*1000;
   //chassis_detail->mutable_gas()->set_throttle_cmd(gas);
   chassis_detail->mutable_gas()->set_driver_override(driver_override(bytes, length));
}

void Throttle343::Parse(const std::uint8_t *bytes, int32_t length,
                         const struct timeval &timestamp,
                         ChassisDetail *chassis_detail) const {
  chassis_detail->mutable_eps()->set_timestamp_65(
      static_cast<double>(timestamp.tv_sec) +
      static_cast<double>(timestamp.tv_usec) / 1000000.0);
  Parse(bytes, length, chassis_detail);
}

double Throttle343::throttle_cmd(const std::uint8_t *bytes, int32_t length) const {
  // Pedal Command from the command message
  Byte frame_high(bytes + 0);
  int32_t high = frame_high.get_byte(0, 8);
  Byte frame_low(bytes + 1);
  int32_t low = frame_low.get_byte(0, 8);
  int32_t value = (high << 8) | low;
  return double(value)/1000;
}

void Throttle343::set_throttle( double throttle ) {
  throttle_value_ = throttle;
}


bool Throttle343::driver_override(const std::uint8_t *bytes, int32_t length) const {
  // Pedal Command from the command message
  Byte frame(bytes + 3);
  return frame.is_bit_1(7);
}

void Throttle343::UpdateData(uint8_t *data) {
  set_pedal_p(data, pedal_cmd_);
  set_enable_p(data, pedal_enable_);
  set_clear_driver_override_flag_p(data, true);
  set_ignore_driver_override_p(data, true);
  set_crc_p( data, getCRC(data) );
}

void Throttle343::Reset() {
  pedal_cmd_ = 0.0;
  pedal_enable_ = false;
  clear_driver_override_flag_ = false;
  ignore_driver_override_ = false;
  watchdog_counter_ = 0;
  throttle_value_ = 0;
}

Throttle343 *Throttle343::set_pedal(double pedal) {
  double goal_throttle = 0;
  if ( pedal >= 0 ) goal_throttle = pedal * 15;
  //else goal_throttle = pedal * 45;
  else goal_throttle = pedal * 15;
  auto delta_throttle = goal_throttle - throttle_value_;
  write_throttle_log("%f;%f;%f;%f;%f", pedal, throttle_value_, pedal_cmd_, goal_throttle, delta_throttle );
  //pedal_cmd_ = throttle_value_ + delta_throttle/8;
  pedal_cmd_ = goal_throttle;
  return this;
}

Throttle343 *Throttle343::set_enable() {
  if ( !pedal_enable_ ) {
      initFilename();
  }
  pedal_enable_ = true;
  return this;
}

Throttle343 *Throttle343::set_disable() {
  pedal_enable_ = false;
  return this;
}

// private

void Throttle343::set_pedal_p(uint8_t *data, double pedal) {
  // change from [0-1500] to [0.00-1.50]
  // and a rough mapping
  //pedal /= 100.0;
  //pedal = ProtocolData::BoundedValue(0.0, 1.0, pedal);
  //int32_t x = static_cast<int32_t>(pedal / 1.52590218966964e-03);


  int32_t x = static_cast<int32_t>(pedal);
  std::uint8_t t = 0;

  t = static_cast<uint8_t>( x & 0xFF );
  Byte frame_high(data + 1);
  frame_high.set_value(t, 0, 8);

  x >>= 8;
  t = static_cast<uint8_t>( x & 0xFF );
  Byte frame_low(data + 0);
  frame_low.set_value(t, 0, 8);

  Byte frame_x63(data + 2 );
  frame_x63.set_value(0x61, 0, 8);
}

void Throttle343::set_enable_p(uint8_t *bytes, bool enable) {
  Byte frame(bytes + 3);
  if (enable) {
    frame.set_bit_1(7);
  } else {
    frame.set_bit_0(7);
  }
  frame.set_bit_1(7);
}

void Throttle343::set_clear_driver_override_flag_p(uint8_t *bytes, bool clear) {
  Byte frame(bytes + 3);
  if (clear) {
    frame.set_bit_1(6);
  } else {
    frame.set_bit_0(6);
  }
  frame.set_bit_1(6);
}

void Throttle343::set_ignore_driver_override_p(uint8_t *bytes, bool ignore) {
  Byte frame(bytes + 3);
  if (ignore) {
    frame.set_bit_1(0);
  } else {
    frame.set_bit_0(0);
  }
  frame.set_bit_0(0);
}

uint8_t Throttle343::getCRC( uint8_t *bytes ) {
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
void Throttle343::set_crc_p( uint8_t *bytes, uint8_t crc ) {
    Byte frame(bytes + 7);
    frame.set_value( crc, 0, 8 );
}

void Throttle343::write_throttle_log( const char * str,... ) {
    char *buffer = new char[MAX_LOG_SIZE];
    va_list ap;
    va_start (ap, str);
    vsnprintf (buffer,MAX_LOG_SIZE, str, ap);
    va_end(ap);
    write_throttle_log_process( buffer );
    delete [] buffer;
}

void Throttle343::write_throttle_log_process( const char * buffer ) {
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

}  // namespace lexus_rx
}  // namespace canbus
}  // namespace apollo
