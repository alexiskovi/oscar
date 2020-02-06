#pragma once

#include <sys/time.h>

#include "modules/canbus/proto/chassis_detail.pb.h"
#include "modules/drivers/canbus/can_comm/protocol_data.h"
#include "modules/common/filters/mean_filter.h"
#include "modules/control/common/pid_controller.h"
#include "modules/control/common/pid_steer_controller.h"
#include "modules/control/proto/pid_steer_conf.pb.h"

/**
 * @namespace apollo::canbus::lexus_rx
 * @brief apollo::canbus::lexus_rx
 */
namespace apollo {
namespace canbus {
namespace lexus_rx {

/**
 * @class Steering7F0
 *
 * @brief one of the protocol data of lexus vehicle
 */
class Steering7F0 : public ::apollo::drivers::canbus::ProtocolData<
                       ::apollo::canbus::ChassisDetail> {
 public:
  Steering7F0();
 
  static const int32_t ID;

  static const uint8_t DLC;

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
  Steering7F0 *set_enable();

  /**
   * @brief set steering request disable to true
   * @return a this pointer to the instance itself
   */
  Steering7F0 *set_disable();

  /**
   * @brief set steering angle speed
   * @return a this pointer to the instance itself
   */
  Steering7F0 *set_current_steering_angle_percent(double angle);

  Steering7F0 *set_goal_steering_angle_percent(double angle);

  Steering7F0 *set_current_steering_angle(double angle);

  Steering7F0 *set_goal_steering_angle(double angle);

  Steering7F0 *set_current_torque(double torque);

  Steering7F0 *set_speed(double speed);


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


  void set_torque_p( uint8_t *data, double curr_angle, double goal_angle );

  uint8_t getCRC( uint8_t *data );

  void set_crc_p( uint8_t *bytes, uint8_t crc );

  uint8_t getDirection() { return direction; }

  double getTorque() { return torque; }

  private:

    void initFilename();
    
    double epas_torque(const std::uint8_t *bytes, int32_t length) const;

    void write_steering_log( const char * str,... );

    void write_steering_log_process( const char * buffer );

    double curr_steering_angle_ = 0.0;
    double goal_steering_angle_ = 0.0;
    double curr_steering_angle_percent_ = 0.0;
    double goal_steering_angle_percent_ = 0.0;
    bool steering_enable_ = false;
    bool clear_driver_override_flag_ = false;
    bool ignore_driver_override_ = false;
    double steering_angle_speed_ = 0.0;
    int32_t watchdog_counter_ = 0;
    bool disable_audible_warning_ = false;
    uint8_t counter = 1;
    bool counter_flag;
    double torque_ = 0;
    int turn_direction = 0;
    double current_torque = 0;
    double speed_ = 0;
    double prev_torque = 0;
    double last_torque = 0;
    uint8_t last_direction = 0x00;

    double prev_delta = 0;

    double torque  = 0;

    uint8_t direction = 0;

    char logfile[80];

    apollo::common::MeanFilter * mf_steer;

    apollo::common::MeanFilter * mf_torque;

    apollo::control::PIDSteerController st_pid_controller;

    apollo::control::PIDController ls_pid_controller;

    apollo::control::PIDSteerController hs_pid_controller;

    double dt;

};

}  // namespace lexus_rx
}  // namespace canbus
}  // namespace apollo



