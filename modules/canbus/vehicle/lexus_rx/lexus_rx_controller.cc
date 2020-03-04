#include "modules/canbus/vehicle/lexus_rx/lexus_rx_controller.h"

#include "modules/common/proto/vehicle_signal.pb.h"

#include "modules/canbus/vehicle/lexus_rx/lexus_rx_message_manager.h"
#include "modules/canbus/vehicle/vehicle_controller.h"
#include "modules/common/kv_db/kv_db.h"
#include "cyber/common/log.h"
#include "modules/common/time/time.h"
#include "modules/drivers/canbus/can_comm/can_sender.h"
#include "modules/drivers/canbus/can_comm/protocol_data.h"


using namespace apollo::canbus::lexus_rx;

using ::apollo::drivers::canbus::ProtocolData;
using ::apollo::common::ErrorCode;
using ::apollo::control::ControlCommand;

namespace {

const int32_t kMaxFailAttempt = 10;
const int32_t CHECK_RESPONSE_STEER_UNIT_FLAG = 1;
const int32_t CHECK_RESPONSE_SPEED_UNIT_FLAG = 2;
}  // namespace

ErrorCode LexusRxController::Init(
    const VehicleParameter &params,
    CanSender<::apollo::canbus::ChassisDetail> *const can_sender,
    MessageManager<::apollo::canbus::ChassisDetail> *const message_manager) {

    if (is_initialized_) {
        AINFO << "LexusRxController has already been initiated.";
        return ErrorCode::CANBUS_ERROR;
    }

    vehicle_params_.CopyFrom(
      common::VehicleConfigHelper::Instance()->GetConfig().vehicle_param());

    params_.CopyFrom(params);

    if (!params_.has_driving_mode()) {
        AERROR << "Vehicle conf pb not set driving_mode.";
        return ErrorCode::CANBUS_ERROR;
    }

    if (can_sender == nullptr) {
        return ErrorCode::CANBUS_ERROR;
    }
    can_sender_ = can_sender;

    if (message_manager == nullptr) {
        AERROR << "protocol manager is null.";
       return ErrorCode::CANBUS_ERROR;
    }
    message_manager_ = message_manager;

    steering_24_ = dynamic_cast<Steering24 *>(
        message_manager_->GetMutableProtocolDataById(Steering24::ID));

    if (steering_24_ == nullptr) {
        AERROR << "Steering24 does not exist in the LexusRxMessageManager!";
        return ErrorCode::CANBUS_ERROR;
    }

    steering_25_ = dynamic_cast<Steering25 *>(
        message_manager_->GetMutableProtocolDataById(Steering25::ID));

    if (steering_25_ == nullptr) {
        AERROR << "Steering25 does not exist in the LexusRxMessageManager!";
        return ErrorCode::CANBUS_ERROR;
    }

    steering_260_ = dynamic_cast<Steering260 *>(
        message_manager_->GetMutableProtocolDataById(Steering260::ID));

    if (steering_260_ == nullptr) {
        AERROR << "Steering260 does not exist in the LexusRxMessageManager!";
        return ErrorCode::CANBUS_ERROR;
    }

    steering_2e4_ = dynamic_cast<Steering2E4 *>(
        message_manager_->GetMutableProtocolDataById(Steering2E4::ID));

    if (steering_2e4_ == nullptr) {
        AERROR << "Steering2E4 does not exist in the LexusRxMessageManager!";
        return ErrorCode::CANBUS_ERROR;
    }

    steering_41_ = dynamic_cast<Steering41 *>(
        message_manager_->GetMutableProtocolDataById(Steering41::ID));

    if (steering_41_ == nullptr) {
        AERROR << "Steering41 does not exist in the LexusRxMessageManager!";
        return ErrorCode::CANBUS_ERROR;
    }

    steering_7f0_ = dynamic_cast<Steering7F0 *>(
        message_manager_->GetMutableProtocolDataById(Steering7F0::ID));

    if (steering_7f0_ == nullptr) {
        AERROR << "Steering7F0 does not exist in the LexusRxMessageManager!";
        return ErrorCode::CANBUS_ERROR;
    }

    steering_7f1_ = dynamic_cast<Steering7F1 *>(
        message_manager_->GetMutableProtocolDataById(Steering7F1::ID));

    if (steering_7f1_ == nullptr) {
        AERROR << "Steering7F1 does not exist in the LexusRxMessageManager!";
        return ErrorCode::CANBUS_ERROR;
    }

    steering_7fb_ = dynamic_cast<Steering7FB *>(
        message_manager_->GetMutableProtocolDataById(Steering7FB::ID));

    if (steering_7fb_ == nullptr) {
        AERROR << "Steering7FB does not exist in the LexusRxMessageManager!";
        return ErrorCode::CANBUS_ERROR;
    }

    steering_7f8_ = dynamic_cast<Steering7F8 *>(
        message_manager_->GetMutableProtocolDataById(Steering7F8::ID));

    if (steering_7f8_ == nullptr) {
        AERROR << "Steering7F8 does not exist in the LexusRxMessageManager!";
        return ErrorCode::CANBUS_ERROR;
    }

    //gear_3bc_ = dynamic_cast<Gear3BC *>(
    //    message_manager_->GetMutableProtocolDataById(Gear3BC::ID));

    //if (gear_3bc_ == nullptr) {
    //    AERROR << "Gear3BC does not exist in the LexusRxMessageManager!";
    //    return ErrorCode::CANBUS_ERROR;
    //}

    gear_777_ = dynamic_cast<Gear777 *>(
        message_manager_->GetMutableProtocolDataById(Gear777::ID));

    if (gear_777_ == nullptr) {
        AERROR << "Gear777 does not exist in the LexusRxMessageManager!";
        return ErrorCode::CANBUS_ERROR;
    }

    brake_a6_ = dynamic_cast<BrakeA6 *>(
        message_manager_->GetMutableProtocolDataById(BrakeA6::ID));

    if (brake_a6_ == nullptr) {
        AERROR << "BrakeA6 does not exist in the LexusRxMessageManager!";
        return ErrorCode::CANBUS_ERROR;
    }

    brake_226_ = dynamic_cast<Brake226 *>(
        message_manager_->GetMutableProtocolDataById(Brake226::ID));

    if (brake_226_ == nullptr) {
        AERROR << "Brake226 does not exist in the LexusRxMessageManager!";
        return ErrorCode::CANBUS_ERROR;
    }

    accel_228_ = dynamic_cast<Accel228 *>(
        message_manager_->GetMutableProtocolDataById(Accel228::ID));

    if (accel_228_ == nullptr) {
        AERROR << "Accel228 does not exist in the LexusRxMessageManager!";
        return ErrorCode::CANBUS_ERROR;
    }

    throttle_245_ = dynamic_cast<Throttle245 *>(
        message_manager_->GetMutableProtocolDataById(Throttle245::ID));

    if (throttle_245_ == nullptr) {
        AERROR << "Throttle245 does not exist in the LexusRxMessageManager!";
        return ErrorCode::CANBUS_ERROR;
    }

    throttle_343_ = dynamic_cast<Throttle343 *>(
        message_manager_->GetMutableProtocolDataById(Throttle343::ID));

    if (throttle_343_ == nullptr) {
        AERROR << "Throttle343 does not exist in the LexusRxMessageManager!";
        return ErrorCode::CANBUS_ERROR;
    }

    speed_b4_ = dynamic_cast<SpeedB4 *>(
        message_manager_->GetMutableProtocolDataById(SpeedB4::ID));

    if (speed_b4_ == nullptr) {
        AERROR << "SpeedB4 does not exist in the LexusRxMessageManager!";
        return ErrorCode::CANBUS_ERROR;
    }

    wheelspeed_aa_ = dynamic_cast<WheelspeedAA *>(
        message_manager_->GetMutableProtocolDataById(WheelspeedAA::ID));

    if (wheelspeed_aa_ == nullptr) {
        AERROR << "WheelspeedAA does not exist in the LexusRxMessageManager!";
        return ErrorCode::CANBUS_ERROR;
    }

    rpm_1c4_ = dynamic_cast<Rpm1C4 *>(
        message_manager_->GetMutableProtocolDataById(Rpm1C4::ID));

    if (rpm_1c4_ == nullptr) {
        AERROR << "Rpm1C4 does not exist in the LexusRxMessageManager!";
        return ErrorCode::CANBUS_ERROR;
    }

    mileage_611_ = dynamic_cast<Mileage611 *>(
        message_manager_->GetMutableProtocolDataById(Mileage611::ID));

    if (mileage_611_ == nullptr) {
        AERROR << "Rpm1C4 does not exist in the LexusRxMessageManager!";
        return ErrorCode::CANBUS_ERROR;
    }

    radar_2e6_ = dynamic_cast<Radar2E6 *>(
        message_manager_->GetMutableProtocolDataById(Radar2E6::ID));

    if (radar_2e6_ == nullptr) {
        AERROR << "Radar2E6 does not exist in the LexusRxMessageManager!";
        return ErrorCode::CANBUS_ERROR;
    }

    acc_1d3_ = dynamic_cast<Acc1D3 *>(
        message_manager_->GetMutableProtocolDataById(Acc1D3::ID));

    if (acc_1d3_ == nullptr) {
        AERROR << "Acc1d3 does not exist in the LexusRxMessageManager!";
        return ErrorCode::CANBUS_ERROR;
    }

    cruise_361_ = dynamic_cast<Cruise361 *>(
        message_manager_->GetMutableProtocolDataById(Cruise361::ID));

    if (cruise_361_ == nullptr) {
        AERROR << "Cruise361 does not exist in the LexusRxMessageManager!";
        return ErrorCode::CANBUS_ERROR;
    }

    //steer_ = new SteeringController();
    //steer_->set_lka_steer( steering_2e4_ );
    //steer_->set_alter_steer( steering_7f0_ );

    //can_sender_->AddMessage(Steering2E4::ID, steering_2e4_, false);
    can_sender_->AddMessage(Steering7F0::ID, steering_7f0_, false);
    can_sender_->AddMessage(Steering7F1::ID, steering_7f1_, false);
    can_sender_->AddMessage(Throttle343::ID, throttle_343_, false);
    can_sender_->AddMessage(Radar2E6::ID, radar_2e6_, false);
    can_sender_->AddMessage(Steering7FB::ID, steering_7fb_, false);
    can_sender_->AddMessage(Gear777::ID, gear_777_, false);

    // need sleep to ensure all messages received
    AINFO << "Controller is initialized.";

    is_initialized_ = true;
    return ErrorCode::OK;
}

bool LexusRxController::Start() {
    if (!is_initialized_) {
        AERROR << "LexusRxController has NOT been initiated.";
        return false;
    }
    const auto &update_func = [this] { SecurityDogThreadFunc(); };
    thread_.reset(new std::thread(update_func));

    return true;
}

void LexusRxController::Stop() {
    if (!is_initialized_) {
        AERROR << "LexusRxController stops or starts improperly!";
        return;
    }

    if (thread_ != nullptr && thread_->joinable()) {
        thread_->join();
        thread_.reset();
        AINFO << "LexusRxController stopped.";
    }
}

::apollo::canbus::Chassis LexusRxController::chassis() {
    chassis_.Clear();

  ChassisDetail chassis_detail;
  message_manager_->GetSensorData(&chassis_detail);

  can_sender_->Update();

  if (driving_mode() == Chassis::EMERGENCY_MODE) {
    set_chassis_error_code(Chassis::NO_ERROR);
  }

  chassis_.set_driving_mode(driving_mode());
  //chassis_.set_driving_mode( Chassis::COMPLETE_AUTO_DRIVE );
  chassis_.set_error_code(chassis_error_code());


  chassis_.set_engine_started(true);

  if (chassis_detail.has_ems() && chassis_detail.ems().has_engine_rpm()) {
    chassis_.set_engine_rpm( static_cast<float> ( chassis_detail.ems().engine_rpm() ) );
  } else {
    chassis_.set_engine_rpm(0);
  }

  if (chassis_detail.has_vehicle_spd() &&
      chassis_detail.vehicle_spd().has_vehicle_spd()) {
     auto speed = chassis_detail.vehicle_spd().vehicle_spd();
     chassis_.set_speed_mps( static_cast<float> ( speed ) );
     steering_7f0_->set_speed( speed );
    
     //steering_2e4_->set_speed( speed );
     //steer_->set_speed( speed );

  } else {
    chassis_.set_speed_mps(0);
  }

  if (chassis_detail.has_basic() && chassis_detail.basic().has_odo_meter()) {
    // odo_meter is in km
    chassis_.set_odometer_m( static_cast<float> ( chassis_detail.basic().odo_meter() * 1000 ) );
  } else {
    chassis_.set_odometer_m(0);
  }

  if ( chassis_detail.has_brake() && chassis_detail.brake().is_brake_pedal_pressed() ) {
      AINFO << "Break pedal is pressed. Disable steering";
      printf("Break pedal is pressed. Disable steering\n");
      steering_7f0_->set_disable();
      //steer_->set_disable();
      steering_7f1_->set_disable();
      throttle_343_->set_disable();
      alter_steering_avail_ = false;
      steering_7fb_->set_disable();
  }
  else {
      alter_steering_avail_ = true;
  }


  chassis_.set_fuel_range_m(0);

  if (chassis_detail.has_gas() && chassis_detail.gas().has_throttle_cmd()) {
     throttle_343_->set_throttle( chassis_detail.gas().throttle_cmd() );
  }

  if (chassis_detail.has_gas() && chassis_detail.gas().has_throttle_output()) {
    chassis_.set_throttle_percentage(static_cast<float> ( chassis_detail.gas().throttle_output() ) );

  } else {
     /* TODO: Fix after get throttle from can */
    chassis_.set_throttle_percentage(1.0);
  }

  if (chassis_detail.has_brake() && chassis_detail.brake().has_brake_output()) {
    chassis_.set_brake_percentage(static_cast<float> ( chassis_detail.brake().brake_output() ));
  } else {
    /* TODO: Fix after get brake from can */
    chassis_.set_brake_percentage(1.0);
  }

  if (chassis_detail.has_gear() && chassis_detail.gear().has_gear_state()) {
    chassis_.set_gear_location(chassis_detail.gear().gear_state());
  } else {
    /* TODO: Fix after get gear position from can */
    chassis_.set_gear_location(Chassis::GEAR_DRIVE);
  }

  if (chassis_detail.has_eps() && chassis_detail.eps().has_steering_angle()) {
    auto angle = static_cast<float>(chassis_detail.eps().steering_angle() *
                                     100.0 / vehicle_params_.max_steer_angle() * M_PI / 180.0 );

    //AERROR << "Steering angle: " << chassis_detail.eps().steering_angle();

    chassis_.set_steering_percentage(angle);
    //steering_2e4_->set_current_steering_angle(angle);
    steering_7f0_->set_current_steering_angle_percent(angle);
    steering_7f0_->set_current_steering_angle( chassis_detail.eps().steering_angle() );
    //steer_->set_current_steering_angle(angle);

  } else {
    chassis_.set_steering_percentage(0);
  }


  if (chassis_detail.has_eps() && chassis_detail.eps().has_epas_torque()) {
    chassis_.set_steering_torque_nm( static_cast<float>( chassis_detail.eps().epas_torque() ) );
    //steering_2e4_->set_current_torque( chassis_detail.eps().epas_torque() );
    //chassis_detail.mutable_eps()->set_auto_torque( steering_7f0_->getTorque() ); 
  } else {
    chassis_.set_steering_torque_nm(0);
  }

  if (chassis_detail.has_eps() &&
      chassis_detail.epb().has_parking_brake_status()) {
    chassis_.set_parking_brake(chassis_detail.epb().parking_brake_status() ==
                               Epb::PBRAKE_ON);
  } else {
    chassis_.set_parking_brake(false);
  }

  if ( chassis_detail.has_eps() && !chassis_detail.eps().channel_1_fault()  && !chassis_detail.eps().channel_2_fault() ) {
      if ((driving_mode() == Chassis::COMPLETE_AUTO_DRIVE ||
        driving_mode() == Chassis::AUTO_STEER_ONLY)) {
          //steering_2e4_->set_enable();
          steering_7f0_->set_enable();
          //steer_->set_enable();
      }
  }
  else {
     //steering_2e4_->set_disable();
     //steering_7f0_->set_disable();
     //steer_->set_disable();
  }


  if (chassis_detail.has_light() &&
      chassis_detail.light().has_turn_light_type() &&
      chassis_detail.light().turn_light_type() != Light::TURN_LIGHT_OFF) {
    if (chassis_detail.light().turn_light_type() == Light::TURN_LEFT_ON) {

      chassis_.mutable_signal()->set_turn_signal(
          common::VehicleSignal::TURN_LEFT);

    } else if (chassis_detail.light().turn_light_type() ==
               Light::TURN_RIGHT_ON) {

      chassis_.mutable_signal()->set_turn_signal(
          common::VehicleSignal::TURN_RIGHT);

    } else {

      chassis_.mutable_signal()->set_turn_signal(
          common::VehicleSignal::TURN_NONE);

    }
  } else {

    chassis_.mutable_signal()->set_turn_signal(
        common::VehicleSignal::TURN_NONE);

  }

  if (chassis_detail.has_light() && chassis_detail.light().has_is_horn_on() &&
      chassis_detail.light().is_horn_on()) {
    chassis_.mutable_signal()->set_horn(true);
  } else {
    chassis_.mutable_signal()->set_horn(false);
  }


  if (chassis_detail.has_eps() && chassis_detail.eps().has_timestamp_65()) {
    chassis_.set_steering_timestamp(chassis_detail.eps().timestamp_65());
  }

  if (chassis_error_mask_) {
    chassis_.set_chassis_error_mask(chassis_error_mask_);
  }
  if ( chassis_detail.has_basic() && chassis_detail.basic().has_acc_on_off_button() ) {
      
      if ( chassis_detail.basic().acc_on_off_button() && !prev_acc_on_mode_ ) {
          if ( driving_mode() == Chassis::COMPLETE_AUTO_DRIVE ) {
             AERROR << "Turn OFF Auto mode by press start button";
             DisableAutoMode();
          }
          else if ( driving_mode() == Chassis::COMPLETE_MANUAL ) {
              AERROR << "Turn ON Auto mode by press start button";
              EnableAutoMode();
          }
      }
      prev_acc_on_mode_ = chassis_detail.basic().acc_on_off_button();
  }
  /* TODO: FIX Acc button status */
  if ( chassis_detail.has_basic() && chassis_detail.basic().has_acc_on_button() && chassis_detail.basic().acc_on_button() ) {
      //if ((driving_mode() == Chassis::COMPLETE_AUTO_DRIVE ||
      //  driving_mode() == Chassis::AUTO_STEER_ONLY)) {
          //printf("Acc button is on. Enable steering\n");
          //steering_7f0_->set_enable();
          //steer_->set_enable();
          throttle_343_->set_enable();
          //steering_7f1_->set_enable();
          acc_mode = true;
          acc_cnt = 0;
      //}
  } else if ( chassis_detail.has_basic() && chassis_detail.basic().has_acc_off_button() && chassis_detail.basic().acc_off_button() ) {
      if ( acc_mode ) acc_mode = false;
      acc_cnt += 1;
      if ( acc_cnt > 5 ) {
          //printf("Acc button is off. Disable steering\n");
          //steering_7f0_->set_disable();
          //steer_->set_disable();
          throttle_343_->set_disable();
          //steering_7f1_->set_disable();
      }
  }

  if (chassis_detail.basic().has_gps_valid()) {
    chassis_.mutable_chassis_gps()->set_latitude(
        chassis_detail.basic().latitude());
    chassis_.mutable_chassis_gps()->set_longitude(
        chassis_detail.basic().longitude());
    chassis_.mutable_chassis_gps()->set_gps_valid(
        chassis_detail.basic().gps_valid());
    chassis_.mutable_chassis_gps()->set_year(chassis_detail.basic().year());
    chassis_.mutable_chassis_gps()->set_month(chassis_detail.basic().month());
    chassis_.mutable_chassis_gps()->set_day(chassis_detail.basic().day());
    chassis_.mutable_chassis_gps()->set_hours(chassis_detail.basic().hours());
    chassis_.mutable_chassis_gps()->set_minutes(
        chassis_detail.basic().minutes());
    chassis_.mutable_chassis_gps()->set_seconds(
        chassis_detail.basic().seconds());
    chassis_.mutable_chassis_gps()->set_compass_direction(
        chassis_detail.basic().compass_direction());
    chassis_.mutable_chassis_gps()->set_pdop(chassis_detail.basic().pdop());
    chassis_.mutable_chassis_gps()->set_is_gps_fault(
        chassis_detail.basic().is_gps_fault());
    chassis_.mutable_chassis_gps()->set_is_inferred(
        chassis_detail.basic().is_inferred());
    chassis_.mutable_chassis_gps()->set_altitude(
        chassis_detail.basic().altitude());
    chassis_.mutable_chassis_gps()->set_heading(
        chassis_detail.basic().heading());
    chassis_.mutable_chassis_gps()->set_hdop(chassis_detail.basic().hdop());
    chassis_.mutable_chassis_gps()->set_vdop(chassis_detail.basic().vdop());
    chassis_.mutable_chassis_gps()->set_quality(
        chassis_detail.basic().quality());
    chassis_.mutable_chassis_gps()->set_num_satellites(
        chassis_detail.basic().num_satellites());
    chassis_.mutable_chassis_gps()->set_gps_speed(
        chassis_detail.basic().gps_speed());
  } else {
    chassis_.mutable_chassis_gps()->set_gps_valid(false);
  }

  // vin number will be written into KVDB once.
  if (chassis_detail.license().has_vin() && !received_vin_) {
    apollo::common::KVDB::Put("apollo:canbus:vin",
                              chassis_detail.license().vin());
    received_vin_ = true;
  }

  // give engage_advice based on error_code and canbus feedback
  if (!chassis_error_mask_ && !chassis_.parking_brake() &&
      (chassis_.throttle_percentage() != 0.0) &&
      (chassis_.brake_percentage() != 0.0)) {
    chassis_.mutable_engage_advice()->set_advice(
        apollo::common::EngageAdvice::READY_TO_ENGAGE);
  } else {
     chassis_.mutable_engage_advice()->set_advice(
        apollo::common::EngageAdvice::READY_TO_ENGAGE);
    /* TODO: Fix parking brake status

    chassis_.mutable_engage_advice()->set_advice(
        apollo::common::EngageAdvice::DISALLOW_ENGAGE);
    chassis_.mutable_engage_advice()->set_reason(
        "CANBUS not ready, firmware error or emergency button pressed!");
    
    AERROR << "CANBUS not ready, firmware error or emergency button pressed!";
    */
  }
  return chassis_;

}

void LexusRxController::Emergency() {
    set_driving_mode(Chassis::EMERGENCY_MODE);
    ResetProtocol();
    set_chassis_error_code(Chassis::CHASSIS_ERROR);
}

ErrorCode LexusRxController::EnableAutoMode() {
  AERROR << "Try to enable auto mode ";
  if (driving_mode() == Chassis::COMPLETE_AUTO_DRIVE) {
    AERROR << "already in COMPLETE_AUTO_DRIVE mode";
    return ErrorCode::OK;
  }
  
  AERROR << "Try to change modes";

  //steering_2e4_->set_enable();
  //steering_41_->set_enable();
  steering_7f0_->set_enable();
  //steer_->set_enable();
  steering_7f1_->set_enable();
  throttle_343_->set_enable();
  steering_7fb_->set_enable();


  can_sender_->Update();
  const int32_t flag =
      CHECK_RESPONSE_STEER_UNIT_FLAG | CHECK_RESPONSE_SPEED_UNIT_FLAG;
  if (!CheckResponse(flag, true)) {
    AERROR << "Failed to switch to COMPLETE_AUTO_DRIVE mode.";
    Emergency();
    return ErrorCode::CANBUS_ERROR;
  } else {
    set_driving_mode(Chassis::COMPLETE_AUTO_DRIVE);
    AERROR << "Switch to COMPLETE_AUTO_DRIVE mode ok.";
    return ErrorCode::OK;
  }
}

ErrorCode LexusRxController::DisableAutoMode() {
    AERROR << "Try to disable auto mode";
    ResetProtocol();

    //steering_2e4_->set_disable();
    steering_7f0_->set_disable();
    //steer_->set_disable();
    steering_7f1_->set_disable();
    throttle_343_->set_disable();
    steering_7fb_->set_disable();

    can_sender_->Update();
    set_driving_mode(Chassis::COMPLETE_MANUAL);
    set_chassis_error_code(Chassis::NO_ERROR);
    AINFO << "Switch to COMPLETE_MANUAL ok.";
    return ErrorCode::OK;
}

ErrorCode LexusRxController::EnableSteeringOnlyMode() {
    AINFO << "Enable steering only mode";
    if (driving_mode() == Chassis::COMPLETE_AUTO_DRIVE ||
        driving_mode() == Chassis::AUTO_STEER_ONLY) {
            set_driving_mode(Chassis::AUTO_STEER_ONLY);
            AINFO << "Already in AUTO_STEER_ONLY mode";
        return ErrorCode::OK;
    }

    //steering_2e4_->set_enable();
    steering_7f0_->set_disable();
    //steer_->set_disable();
    steering_7f1_->set_enable();
    throttle_343_->set_disable();
    steering_7fb_->set_enable();

    can_sender_->Update();
    if (CheckResponse(CHECK_RESPONSE_STEER_UNIT_FLAG, true) == false) {
        AERROR << "Failed to switch to AUTO_STEER_ONLY mode.";
        Emergency();
        return ErrorCode::CANBUS_ERROR;
    } else {
        set_driving_mode(Chassis::AUTO_STEER_ONLY);
        AINFO << "Switch to AUTO_STEER_ONLY mode ok.";
        return ErrorCode::OK;
    }
}

ErrorCode LexusRxController::EnableSpeedOnlyMode() {
    if (driving_mode() == Chassis::COMPLETE_AUTO_DRIVE ||
        driving_mode() == Chassis::AUTO_SPEED_ONLY) {
        set_driving_mode(Chassis::AUTO_SPEED_ONLY);
        AINFO << "Already in AUTO_SPEED_ONLY mode";
        return ErrorCode::OK;
    }

    //steering_2e4_->set_disable();
    steering_7f0_->set_disable();
    //steer_->set_disable();
    steering_7f1_->set_disable();
    throttle_343_->set_enable();
    steering_7fb_->set_disable();

    can_sender_->Update();
    if (CheckResponse(CHECK_RESPONSE_SPEED_UNIT_FLAG, true) == false) {
        AERROR << "Failed to switch to AUTO_STEER_ONLY mode.";
        Emergency();
        return ErrorCode::CANBUS_ERROR;
    } else {
        set_driving_mode(Chassis::AUTO_SPEED_ONLY);
        AINFO << "Switch to AUTO_SPEED_ONLY mode ok.";
        return ErrorCode::OK;
    }
}

// NEUTRAL, REVERSE, DRIVE
void LexusRxController::Gear(Chassis::GearPosition gear_position) {
    if (!(driving_mode() == Chassis::COMPLETE_AUTO_DRIVE ||
        driving_mode() == Chassis::AUTO_SPEED_ONLY)) {
        AINFO << "this drive mode no need to set gear.";
        return;
    }
    // enable steering to enable shifting
    // actually, if we wanna shift from parking
    // to some other state
    // we need to apply a brake
    // which needs to be done by human or
    // some canbus cmd
    switch (gear_position) {
        case Chassis::GEAR_NEUTRAL: {
            gear_777_->set_gear_neutral();
        break;
        }
        case Chassis::GEAR_REVERSE: {
            gear_777_->set_gear_reverse();
            break;
        }
        case Chassis::GEAR_DRIVE: {
            gear_777_->set_gear_drive();
            break;
        }
        case Chassis::GEAR_PARKING: {
            gear_777_->set_gear_park();
            break;
        }
        case Chassis::GEAR_LOW: {
            //gear_777_->set_gear_low();
            break;
        }
        case Chassis::GEAR_NONE: {
            gear_777_->set_gear_none();
            break;
        }
        case Chassis::GEAR_INVALID: {
            gear_777_->set_gear_none();
            break;
        }
        default: {
            gear_777_->set_gear_none();
        break;
    }
    
  }
}

// brake with new acceleration
// acceleration:0.00~99.99, unit:%
// acceleration:0.0 ~ 7.0, unit:m/s^2
// acceleration_spd:60 ~ 100, suggest: 90
// -> pedal
void LexusRxController::Brake(double pedal) {
     /*
    if (!(driving_mode() == Chassis::COMPLETE_AUTO_DRIVE ||
        driving_mode() == Chassis::AUTO_SPEED_ONLY)) {
        AINFO << "The current drive mode does not need to set acceleration.";
        return;
    }
    */
    //pedal *= 1.5;
    if ( pedal >= 0.01 ) {
       AERROR << "Receive breaking pedal: " << pedal;
       throttle_343_->set_pedal(-pedal);
    }
    //else
    //   AINFO << "Break value is zero. Ignore";
}

// drive with old acceleration
// gas:0.00~99.99 unit:%
void LexusRxController::Throttle(double pedal) {
    //set_driving_mode(Chassis::COMPLETE_AUTO_DRIVE);
    /*
    if (!(driving_mode() == Chassis::COMPLETE_AUTO_DRIVE ||
        driving_mode() == Chassis::AUTO_SPEED_ONLY)) {
        AINFO << "The current drive mode does not need to set acceleration.";
        return;
    }
    */
    if ( pedal >= 1.0 && pedal <= 100.0) {
	AERROR << "Receive throttle: " << pedal;
        throttle_343_->set_pedal(pedal);
    }
}

// drive with acceleration/deceleration
// acc:-7.0 ~ 5.0, unit:m/s^2
void LexusRxController::Acceleration(double acc) {
	AERROR << "Receive acceleration command: " << acc;
  /*
  if (driving_mode() != Chassis::COMPLETE_AUTO_DRIVE &&
      driving_mode() != Chassis::AUTO_SPEED_ONLY) {
    AINFO << "The current drive mode does not need to set acceleration.";
    return;
  }
  */
  // None
}


// Lexus default, -500 ~ 5000, left:+, right:-
// need to be compatible with control module, so reverse
// steering with old angle speed
// angle:-99.99~0.00~99.99, unit:%, left:-, right:+
void LexusRxController::Steer(double angle) {
    /*
    if (!(driving_mode() == Chassis::COMPLETE_AUTO_DRIVE ||
        driving_mode() == Chassis::AUTO_STEER_ONLY)) {
        AINFO << "The current driving mode does not need to set steer.";
        return;
    }
    */

    const double real_angle =
      vehicle_params_.max_steer_angle() / M_PI * 180 * angle / 100.0;

    // reverse sign
    steering_7f0_->set_goal_steering_angle(real_angle);
    //steer_->set_goal_steering_angle(real_angle);
    steering_7f0_->set_goal_steering_angle_percent(angle);
    //AERROR << "Receive steering control command to set angle " << real_angle << " ( percent " << angle << " )";
}

// steering with new angle speed
// angle:-99.99~0.00~99.99, unit:%, left:-, right:+
// angle_spd:0.00~99.99, unit:deg/s
void LexusRxController::Steer(double angle, double angle_spd) {
    /*
    if (!(driving_mode() == Chassis::COMPLETE_AUTO_DRIVE ||
        driving_mode() == Chassis::AUTO_STEER_ONLY)) {
        AINFO << "The current driving mode does not need to set steer.";
        return;
    }
    */
    const double real_angle = vehicle_params_.max_steer_angle() / M_PI * 180 * angle / 100.0;
    /*
    const double real_angle_spd =
      ProtocolData<::apollo::canbus::ChassisDetail>::BoundedValue(
          vehicle_params_.min_steer_angle_spd(), vehicle_params_.max_steer_angle_spd(),
          vehicle_params_.max_steer_angle_spd() * angle_spd / 100.0);
    */
    steering_7f0_->set_goal_steering_angle(real_angle);
    //steer_->set_goal_steering_angle(real_angle);
    steering_7f0_->set_goal_steering_angle_percent(angle);
    //AERROR << "Receive steering control command to set angle " << real_angle << " ( percent " << angle << " )";
}

void LexusRxController::SetEpbBreak(const ControlCommand &command) {
    if (command.parking_brake()) {
        // None
    } else {
        // None
    }
}

void LexusRxController::SetBeam(const ControlCommand &command) {
    if (command.signal().high_beam()) {
        AERROR << "Process High Beam command";
    } else if (command.signal().low_beam()) {
        AERROR << "Process Low Beam command";
    } else {
        // None
    }
}

void LexusRxController::SetHorn(const ControlCommand &command) {
    if (command.signal().horn()) {
        AERROR<< "Process Horn command";
    } else {
        // None
    }
}

void LexusRxController::SetTurningSignal(const ControlCommand &command) {
    // Set Turn Signal
    auto signal = command.signal().turn_signal();
    if (signal == common::VehicleSignal::TURN_LEFT) {
        AERROR << "Process TURN_LEFT command";
    } else if (signal == common::VehicleSignal::TURN_RIGHT) {
        AERROR << "Process TURN_RIGHT command";
    } else {
        //Do nothing
    }
}

void LexusRxController::ResetProtocol() {
    message_manager_->ResetSendMessages();
}

bool LexusRxController::CheckChassisError() {
    return false;
}

void LexusRxController::SecurityDogThreadFunc() {
    if (can_sender_ == nullptr) {
        AERROR << "Fail to run SecurityDogThreadFunc() because can_sender_ is "
              "nullptr.";
        return;
    }
    while (!can_sender_->IsRunning()) {
        std::this_thread::yield();
    }

    std::chrono::duration<double, std::micro> default_period{50000};
    int64_t start = absl::ToUnixMicros(common::time::Clock::Now());

    int32_t speed_ctrl_fail = 0;
    int32_t steer_ctrl_fail = 0;

    while (can_sender_->IsRunning()) {
        const Chassis::DrivingMode mode = driving_mode();
        bool emergency_mode = false;

        // 1. steer control check
        if ((mode == Chassis::COMPLETE_AUTO_DRIVE ||
            mode == Chassis::AUTO_STEER_ONLY) &&
            CheckResponse(CHECK_RESPONSE_STEER_UNIT_FLAG, false) == false) {
            ++steer_ctrl_fail;
            if (steer_ctrl_fail >= kMaxFailAttempt) {
                emergency_mode = true;
                set_chassis_error_code(Chassis::MANUAL_INTERVENTION);
            }
        } else {
            steer_ctrl_fail = 0;
        }

        // 2. speed control check
        if ((mode == Chassis::COMPLETE_AUTO_DRIVE ||
            mode == Chassis::AUTO_SPEED_ONLY) &&
            CheckResponse(CHECK_RESPONSE_SPEED_UNIT_FLAG, false) == false) {
            ++speed_ctrl_fail;
            if (speed_ctrl_fail >= kMaxFailAttempt) {
                emergency_mode = true;
                set_chassis_error_code(Chassis::MANUAL_INTERVENTION);
            }
        } else {
            speed_ctrl_fail = 0;
        }
        if (CheckChassisError()) {
            set_chassis_error_code(Chassis::CHASSIS_ERROR);
            emergency_mode = true;
        }

        if (emergency_mode && mode != Chassis::EMERGENCY_MODE) {
            Emergency();
        }
        int64_t end = absl::ToUnixMicros(common::time::Clock::Now());
    
        std::chrono::duration<double, std::micro> elapsed{end - start};
        if (elapsed < default_period) {
            std::this_thread::sleep_for(default_period - elapsed);
            start = absl::ToUnixMicros(common::time::Clock::Now());
        } else {
            AERROR_EVERY(100)
                << "Too much time consumption in LexusRxController looping process:"
                << elapsed.count();
            start = end;
        }
    }
}

bool LexusRxController::CheckResponse(const int32_t flags, bool need_wait) {
    return true;
    // for Lexus, CheckResponse commonly takes 300ms. We leave a 100ms buffer
    // for it.
    int32_t retry_num = 20;
    ChassisDetail chassis_detail;
    bool is_eps_online = false;
    bool is_vcu_online = false;
    bool is_esp_online = false;

    do {
        if (message_manager_->GetSensorData(&chassis_detail) != ErrorCode::OK) {
            AERROR_EVERY(100) << "get chassis detail failed.";
            return false;
        }
        bool check_ok = true;
        if (flags & CHECK_RESPONSE_STEER_UNIT_FLAG) {
            is_eps_online = chassis_detail.has_check_response() &&
                      chassis_detail.check_response().has_is_eps_online() &&
                      chassis_detail.check_response().is_eps_online();
            check_ok = check_ok && is_eps_online;
        }

        if (flags & CHECK_RESPONSE_SPEED_UNIT_FLAG) {
            is_vcu_online = chassis_detail.has_check_response() &&
                      chassis_detail.check_response().has_is_vcu_online() &&
                      chassis_detail.check_response().is_vcu_online();
            is_esp_online = chassis_detail.has_check_response() &&
                      chassis_detail.check_response().has_is_esp_online() &&
                      chassis_detail.check_response().is_esp_online();
            check_ok = check_ok && is_vcu_online && is_esp_online;
        }
        if (check_ok) {
            return true;
        } else {
            AINFO << "Need to check response again.";
        }
        if (need_wait) {
            --retry_num;
            std::this_thread::sleep_for(
                std::chrono::duration<double, std::milli>(20));
        }
    } while (need_wait && retry_num);

    AINFO << "check_response fail: is_eps_online:" << is_eps_online
        << ", is_vcu_online:" << is_vcu_online
        << ", is_esp_online:" << is_esp_online;
    return false;
}

void LexusRxController::set_chassis_error_mask(const int32_t mask) {
    std::lock_guard<std::mutex> lock(chassis_mask_mutex_);
    chassis_error_mask_ = mask;
}

int32_t LexusRxController::chassis_error_mask() {
    std::lock_guard<std::mutex> lock(chassis_mask_mutex_);
    return chassis_error_mask_;
}

::apollo::canbus::Chassis::ErrorCode LexusRxController::chassis_error_code() {
    std::lock_guard<std::mutex> lock(chassis_error_code_mutex_);
    return chassis_error_code_;
}

void LexusRxController::set_chassis_error_code(
    const Chassis::ErrorCode &error_code) {
    std::lock_guard<std::mutex> lock(chassis_error_code_mutex_);
    chassis_error_code_ = error_code;
}
