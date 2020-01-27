#pragma once

#include "modules/drivers/canbus/can_comm/protocol_data.h"
#include "modules/canbus/proto/chassis_detail.pb.h"
#include "modules/common/filters/mean_filter.h"

/**
 * @namespace apollo::canbus::lexus_rx
 * @brief apollo::canbus::lexus_rx
 */
namespace apollo {
namespace canbus {
namespace lexus_rx {

/**
 * @class Steering25
 *
 * @brief one of the protocol data of lexus_rx vehicle
 */
class Steering25 : public ::apollo::drivers::canbus::ProtocolData<
                    ::apollo::canbus::ChassisDetail> {
 private:
  apollo::common::MeanFilter * mean_filter_steer_;
 public:
  static const int32_t ID;
  Steering25();

  /*
   * @brief parse received data
   * @param bytes a pointer to the input bytes
   * @param length the length of the input bytes
   * @param timestamp the timestamp of input data
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

  /**
   * @brief get the data period
   * @return the value of data period
   */
  virtual uint32_t GetPeriod() const;

  /**
   * @brief update the data
   * @param data a pointer to the data to be updated
   */
  virtual void UpdateData(uint8_t *data);

  /**
   * @brief reset the private variables
   */
  virtual void Reset();

  /**
   * @brief set steering request enable to true
   * @return a this pointer to the instance itself
   */
  Steering25 *set_enable();

  /**
   * @brief set steering request disable to true
   * @return a this pointer to the instance itself
   */
  Steering25 *set_disable();

 private:


  double steering_angle(const std::uint8_t *bytes, int32_t length) const;

  double steering_angle_speed(const std::uint8_t *bytes, int32_t length) const;

  bool steering_enable_;
};

}  // namespace lexus_rx
}  // namespace canbus
}  // namespace apollo

