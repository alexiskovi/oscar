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
 * @class Steering24
 *
 * @brief one of the protocol data of lincoln vehicle
 */
class Steering24 : public ::apollo::drivers::canbus::ProtocolData<
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
   * @param timestamp the timestamp of input data
   * @param chassis_detail the parsed chassis_detail
   */
  virtual void Parse(const std::uint8_t *bytes, int32_t length,
                     const struct timeval &timestamp,
                     ChassisDetail *chassis_detail) const;

  double accel_y(const std::uint8_t *bytes, int32_t length) const;

  double yaw_rate(const std::uint8_t *bytes, int32_t length) const;

  double steering_torque(const std::uint8_t *bytes, int32_t length) const;

};

}  // namespace lexus_rx
}  // namespace canbus
}  // namespace apollo



