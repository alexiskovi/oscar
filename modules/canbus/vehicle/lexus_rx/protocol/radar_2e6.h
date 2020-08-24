#pragma once

#include "modules/drivers/canbus/can_comm/protocol_data.h"
#include "modules/canbus/proto/chassis_detail.pb.h"

/**
 * @namespace apollo::canbus::lexus_rx
 * @brief apollo::canbus::lexus_rx
 */
namespace apollo {
namespace canbus {
namespace lexus_rx {

/**
 * @class Radar2E6
 *
 * @brief one of the protocol data of lexus_rx vehicle
 */
class Radar2E6 : public ::apollo::drivers::canbus::ProtocolData<
                    ::apollo::canbus::ChassisDetail> {
 public:
  static const int32_t ID;

  static const uint8_t DLC;


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

  Radar2E6 *set_enable();

  /**
   * @brief set disable
   * @return a this pointer to the instance itself
   */
  Radar2E6 *set_disable();

 private:

  void set_lead_speed_p(uint8_t *data, uint16_t lead_speed);

  void set_lead_dist_p(uint8_t *bytes, uint16_t lead_dist);

  uint8_t getCRC( uint8_t *data );

  void set_crc_p( uint8_t *bytes, uint8_t crc );

 private:
  bool radar_enable_ = false;
};

}  // namespace lexus_rx
}  // namespace canbus
}  // namespace apollo