#pragma once

#include <sys/time.h>

#include "modules/canbus/proto/chassis_detail.pb.h"
#include "modules/drivers/canbus/can_comm/protocol_data.h"

/**
 * @namespace apollo::canbus::lexus_rx
 * @brief apollo::canbus::lexus_rx
 */
namespace apollo {
namespace canbus {
namespace lexus_rx {

/**
 * @class WheelspeedAA
 *
 * @brief one of the protocol data of lexus_rx vehicle
 */
class WheelspeedAA : public ::apollo::drivers::canbus::ProtocolData<
                         ::apollo::canbus::ChassisDetail> {
 public:
  static const int32_t ID;

  /*
   * @brief parse received data
   * @param bytes a pointer to the input bytes
   * @param length the length of the input bytes
   * @param chassis_detail the parsed chassis_detail
   */
  virtual void Parse(const std::uint8_t *bytes, int32_t length,
                     ChassisDetail *chassis_detail) const;

  /*
   * @brief parse received data
   * @param bytes a pointer to the input bytes
   * @param length the length of the input bytes
   * @param timestamp the timestamp of input bytes
   * @param chassis_detail the parsed chassis_detail
   */
  virtual void Parse(const std::uint8_t *bytes, int32_t length,
                     const struct timeval &timestamp,
                     ChassisDetail *chassis_detail) const;

 private:

  double front_left_wheel_speed(const std::uint8_t *bytes,
                                int32_t length) const;

  double front_right_wheel_speed(const std::uint8_t *bytes,
                                 int32_t length) const;

  double rear_left_wheel_speed(const std::uint8_t *bytes, int32_t length) const;

  double rear_right_wheel_speed(const std::uint8_t *bytes,
                                int32_t length) const;

  double parse_two_frames(const std::uint8_t low_byte,
                          const std::uint8_t high_byte) const;
};

}  // namespace lexus_rx
}  // namespace canbus
}  // namespace apollo
