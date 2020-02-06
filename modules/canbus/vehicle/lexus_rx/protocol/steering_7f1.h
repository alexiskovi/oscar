#pragma once

#include <sys/time.h>

#include "modules/canbus/proto/chassis_detail.pb.h"
#include "modules/drivers/canbus/can_comm/protocol_data.h"
#include "modules/common/filters/mean_filter.h"
#include "modules/control/common/pid_controller.h"
#include "modules/control/proto/pid_conf.pb.h"

/**
 * @namespace apollo::canbus::lexus_rx
 * @brief apollo::canbus::lexus_rx
 */
namespace apollo {
namespace canbus {
namespace lexus_rx {

/**
 * @class Steering7F1
 *
 * @brief one of the protocol data of lexus vehicle
 */
class Steering7F1 : public ::apollo::drivers::canbus::ProtocolData<
                       ::apollo::canbus::ChassisDetail> {
 public:
 
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
  Steering7F1 *set_enable();

  /**
   * @brief set steering request disable to true
   * @return a this pointer to the instance itself
   */
  Steering7F1 *set_disable();


  uint8_t getCRC( uint8_t *data );

  void set_crc_p( uint8_t *bytes, uint8_t crc );


  private:

    bool steering_enable_ = false;

    bool acc_button_on = false;

    bool acc_button_set = false;

    size_t acc_on_cnt = 0;

    size_t acc_off_cnt = 100;
};

}  // namespace lexus_rx
}  // namespace canbus
}  // namespace apollo



