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
 * @class Throttle343
 *
 * @brief one of the protocol data of lexus_rx vehicle
 */
class Throttle343 : public ::apollo::drivers::canbus::ProtocolData<
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

  double throttle_cmd(const std::uint8_t *bytes,
                                int32_t length) const;

  bool driver_override( const std::uint8_t *bytes,
                                int32_t length) const;

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
   * @brief set pedal based on pedal command
   * @return a this pointer to the instance itself
   */
  Throttle343 *set_pedal(double pcmd);

  /**
   * @brief set enable
   * @return a this pointer to the instance itself
   */
  Throttle343 *set_enable();

  /**
   * @brief set disable
   * @return a this pointer to the instance itself
   */
  Throttle343 *set_disable();

  /**
   * @brief set enable ignore driver override
   * @return a this pointer to the instance itself
   */
  Throttle343 *set_enable_ignore_driver_override();

  void set_throttle( double throttle );

 private:

  void set_pedal_p(uint8_t *data, double pcmd);

  void set_enable_p(uint8_t *bytes, bool en);

  void set_clear_driver_override_flag_p(uint8_t *bytes, bool clear);

  void set_ignore_driver_override_p(uint8_t *bytes, bool ignore);

  void set_watchdog_counter_p(uint8_t *data, int32_t count);

  uint8_t getCRC( uint8_t *data );

  void set_crc_p( uint8_t *bytes, uint8_t crc );

  void initFilename();

  void write_throttle_log( const char * str,... );

  void write_throttle_log_process( const char * buffer );

 private:
  double pedal_cmd_ = 0.0;
  bool pedal_enable_ = false;
  bool clear_driver_override_flag_ = false;
  bool ignore_driver_override_ = false;
  int32_t watchdog_counter_ = 0;
  double throttle_value_ = 0;
  char logfile[80];
};

}  // namespace lexus_rx
}  // namespace canbus
}  // namespace apollo
