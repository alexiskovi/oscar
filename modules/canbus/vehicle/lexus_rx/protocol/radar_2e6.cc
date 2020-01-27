#include "modules/canbus/vehicle/lexus_rx/protocol/radar_2e6.h"

#include "modules/drivers/canbus/common/byte.h"

namespace apollo {
namespace canbus {
namespace lexus_rx {

using ::apollo::drivers::canbus::Byte;

static uint16_t lead_rel_dist[] = {2767,2769,2791,2791,2801,2801,2787,2779,2777,2777,2777,2777,2801,2807,2807,2843,2857,2857,2861,2861,2849,2843,2843,2843,2845,2845,2847,2849,2849,2861,2861,2869,2875,2881,2881,2885,2885,2867,2889,2889,2891,2891,2885,2895,2895,2911,2923,2923,2927,2929,2929,2933,2957,2957,2979,2979,2983,2987,2991,2991,2983,2983,2993,3005,3005,3011,3017,3017,3031,3031,3025,3013,3013,3031,3073,3073,3045,3049,3049,3025,3025,2993,3033,3031,3031,3045,3045,3063,3063,3063,3063,3063,3089,3055,3055,3059,3063,3063,3073,3083,3083,3109,3117,3117,3115,3115,3107,3113,3113,3113,3115,3115,3117};
static uint16_t lead_rel_speed[] = {288,272,208,208,288,320,288,320,304,304,304,304,352,352,352,352,368,368,384,384,368,384,384,384,384,384,336,320,320,352,352,352,416,432,432,416,416,368,368,368,384,384,384,400,400,352,352,352,192,240,240,256,288,288,304,304,320,320,224,224,208,208,224,224,224,224,240,240,192,192,224,240,240,208,192,192,208,192,192,256,256,192,256,256,256,240,240,272,256,256,304,304,224,272,272,304,320,320,320,336,336,272,320,320,368,368,384,432,432,432,448,448,448};

// public

const int32_t Radar2E6::ID = 0x2e6;

const uint8_t Radar2E6::DLC = 8;

uint32_t Radar2E6::GetPeriod() const {
  static const uint32_t PERIOD = 30 * 1000;
  return PERIOD;
}

void Radar2E6::Parse(const std::uint8_t *bytes, int32_t length,
                         ChassisDetail *chassis_detail) const {
}

void Radar2E6::Parse(const std::uint8_t *bytes, int32_t length,
                         const struct timeval &timestamp,
                         ChassisDetail *chassis_detail) const {
  chassis_detail->mutable_eps()->set_timestamp_65(
      static_cast<double>(timestamp.tv_sec) +
      static_cast<double>(timestamp.tv_usec) / 1000000.0);
  Parse(bytes, length, chassis_detail);
}

void Radar2E6::UpdateData(uint8_t *data) {
  auto glob_cnt = this->getCounter();
  size_t radarDataSize = sizeof(lead_rel_dist)/sizeof(*lead_rel_dist);
  int step = static_cast<int>( (glob_cnt*5) % radarDataSize );

  uint16_t lead_dist = lead_rel_dist[step];
  uint16_t lead_speed = lead_rel_speed[step];

  set_lead_dist_p(data, lead_dist );
  set_lead_speed_p(data, lead_speed );
  set_crc_p( data, getCRC(data) );
}

void Radar2E6::Reset() {
}

Radar2E6 *Radar2E6::set_enable() {
  radar_enable_ = true;
  return this;
}

Radar2E6 *Radar2E6::set_disable() {
  radar_enable_ = false;
  return this;
}

// private

void Radar2E6::set_lead_dist_p(uint8_t *data, uint16_t dist) {
  //uint16_t x = ( dist << 3 );
  uint16_t x = dist;
  std::uint8_t t = 0;

  t = static_cast<uint8_t>( x & 0xFF );
  Byte frame_high(data + 1);
  frame_high.set_value(t, 0, 8);

  x = static_cast<uint16_t>( x >> 8 );
  t = static_cast<uint8_t>( x & 0xFF );
  Byte frame_low(data + 0);
  frame_low.set_value(t, 0, 8);
}


void Radar2E6::set_lead_speed_p(uint8_t *data, uint16_t speed) {
  
  //uint16_t x = ( speed << 4 );
  uint16_t x = speed;
  std::uint8_t t = 0;
  t = static_cast<uint8_t>( x & 0xFF );
  Byte frame_high(data + 3);
  frame_high.set_value(t, 0, 8);
  x = static_cast<uint16_t> ( x >> 8 );
  t = static_cast<uint8_t>( x & 0xFF );
  Byte frame_low(data + 2);
  frame_low.set_value(t, 0, 8);
}

uint8_t Radar2E6::getCRC( uint8_t *bytes ) {
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


void Radar2E6::set_crc_p( uint8_t *bytes, uint8_t crc ) {
    Byte frame(bytes + 7);
    frame.set_value( crc, 0, 8 );
}

}  // namespace lexus_rx
}  // namespace canbus
}  // namespace apollo
