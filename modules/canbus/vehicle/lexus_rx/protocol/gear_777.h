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
 * @class Gear777
 *
 * @brief one of the protocol data of lexus_rx vehicle
 */
class Gear777 : public ::apollo::drivers::canbus::ProtocolData<
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


  virtual void Reset();

  /* Neutral */
  void set_gear_neutral();

  /* Reverse */
  void set_gear_reverse();

  /* Drive */
  void set_gear_drive();

  /* Parking */
  void set_gear_park();

  /* None */
  void set_gear_none();

  void set_gear_p( uint8_t *data, uint8_t gear_pos );

  void set_counter_p( uint8_t *data );

private:
  uint8_t gear_position_ = 0x03;

  uint64_t drive_position_counter_ = 0;
};

} // lexus_rx
} // canbus
} // apollo
