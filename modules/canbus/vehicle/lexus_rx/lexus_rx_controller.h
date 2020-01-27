/*
Khitsko Konstantin 14-10-18
*/

#pragma once

#include <memory>
#include <thread>

#include "gtest/gtest_prod.h"

#include "modules/canbus/vehicle/vehicle_controller.h"

#include "modules/canbus/proto/canbus_conf.pb.h"
#include "modules/canbus/proto/chassis.pb.h"
#include "modules/canbus/proto/chassis_detail.pb.h"
#include "modules/canbus/proto/vehicle_parameter.pb.h"
#include "cyber/common/macros.h"
#include "modules/common/proto/error_code.pb.h"
#include "modules/control/proto/control_cmd.pb.h"

#include "modules/canbus/vehicle/lexus_rx/protocol/steering_25.h"
#include "modules/canbus/vehicle/lexus_rx/protocol/steering_24.h"
#include "modules/canbus/vehicle/lexus_rx/protocol/steering_260.h"
#include "modules/canbus/vehicle/lexus_rx/protocol/steering_2e4.h"
#include "modules/canbus/vehicle/lexus_rx/protocol/gear_3bc.h"
#include "modules/canbus/vehicle/lexus_rx/protocol/gear_777.h"
#include "modules/canbus/vehicle/lexus_rx/protocol/brake_a6.h"
#include "modules/canbus/vehicle/lexus_rx/protocol/brake_226.h"
#include "modules/canbus/vehicle/lexus_rx/protocol/accel_228.h"
#include "modules/canbus/vehicle/lexus_rx/protocol/throttle_245.h"
#include "modules/canbus/vehicle/lexus_rx/protocol/throttle_343.h"
#include "modules/canbus/vehicle/lexus_rx/protocol/speed_b4.h"
#include "modules/canbus/vehicle/lexus_rx/protocol/wheelspeed_aa.h"
#include "modules/canbus/vehicle/lexus_rx/protocol/rpm_1c4.h"
#include "modules/canbus/vehicle/lexus_rx/protocol/mileage_611.h"
#include "modules/canbus/vehicle/lexus_rx/protocol/radar_2e6.h"
#include "modules/canbus/vehicle/lexus_rx/protocol/steering_41.h"
#include "modules/canbus/vehicle/lexus_rx/protocol/steering_7f0.h"
#include "modules/canbus/vehicle/lexus_rx/protocol/steering_7f8.h"
#include "modules/canbus/vehicle/lexus_rx/protocol/acc_1d3.h"
#include "modules/canbus/vehicle/lexus_rx/protocol/steering_7f1.h"
#include "modules/canbus/vehicle/lexus_rx/protocol/cruise_361.h"
#include "modules/canbus/vehicle/lexus_rx/lexus_rx_steer_controller.h"
#include "modules/canbus/vehicle/lexus_rx/protocol/steering_7fb.h"

/**
 * @namespace apollo::canbus::lexus_rx
 * @brief apollo::canbus::lexus_rx
 */
namespace apollo {
    namespace canbus {
        namespace lexus_rx {

            class LexusRxController final : public VehicleController {
            public:
            
                /**
                * @brief initialize the lexus_rx vehicle controller.
                * @return init error_code
                */
                common::ErrorCode Init(
                    const VehicleParameter &params,
                    CanSender<::apollo::canbus::ChassisDetail> *const can_sender,
                    MessageManager<::apollo::canbus::ChassisDetail> *const message_manager)
                override;

                /**
                * @brief start the vehicle controller.
                * @return true if successfully started.
                */
                bool Start() override;

                /**
                * @brief stop the vehicle controller.
                */
                void Stop() override;

                /**
                * @brief calculate and return the chassis.
                * @returns a copy of chassis. Use copy here to avoid multi-thread issues.
                */
                Chassis chassis() override;

                FRIEND_TEST(LexusRxControllerTest, SetDrivingMode);
  		FRIEND_TEST(LexusRxControllerTest, Status);
  		FRIEND_TEST(LexusRxControllerTest, UpdateDrivingMode);

            private:
                // main logical function for operation the car enter or exit the auto driving
                void Emergency() override;
                common::ErrorCode EnableAutoMode() override;
                common::ErrorCode DisableAutoMode() override;
                common::ErrorCode EnableSteeringOnlyMode() override;
                common::ErrorCode EnableSpeedOnlyMode() override;

                // NEUTRAL, REVERSE, DRIVE
                void Gear(Chassis::GearPosition state) override;

                // brake with new acceleration
                // acceleration:0.00~99.99, unit:%
                // acceleration_spd: 60 ~ 100, suggest: 90
                void Brake(double acceleration) override;

                // drive with acceleration/deceleration
                // acc:-7.0~5.0 unit:m/s^2
                void Acceleration(double acc) override;

                // drive with old acceleration
                // gas:0.00~99.99 unit:%
                void Throttle(double throttle) override;

                // steering with old angle speed
                // angle:-99.99~0.00~99.99, unit:%, left:+, right:-
                void Steer(double angle) override;

                // steering with new angle speed
                // angle:-99.99~0.00~99.99, unit:%, left:+, right:-
                // angle_spd:0.00~99.99, unit:deg/s
                void Steer(double angle, double angle_spd) override;

                // set Electrical Park Brake
                void SetEpbBreak(const control::ControlCommand &command) override;
                void SetBeam(const control::ControlCommand &command) override;
                void SetHorn(const control::ControlCommand &command) override;
                void SetTurningSignal(const control::ControlCommand &command) override;

                void ResetProtocol();
                bool CheckChassisError();


                void SecurityDogThreadFunc();
                virtual bool CheckResponse(const int32_t flags, bool need_wait);
                void set_chassis_error_mask(const int32_t mask);
                int32_t chassis_error_mask();
                Chassis::ErrorCode chassis_error_code();
                void set_chassis_error_code(const Chassis::ErrorCode &error_code);

                // control protocol
                Steering2E4 *steering_2e4_ = nullptr;
                Steering41 *steering_41_ = nullptr;
                Throttle343 *throttle_343_ = nullptr;
                Steering7F0 *steering_7f0_ = nullptr;
                Steering7F1 *steering_7f1_ = nullptr;
                Steering7FB *steering_7fb_ = nullptr;

		Steering24 *steering_24_ = nullptr;
                Steering25 *steering_25_ = nullptr;
                Steering260 *steering_260_ = nullptr;
                //Gear3BC * gear_3bc_ = nullptr;
                Gear777 * gear_777_ = nullptr;
                BrakeA6 * brake_a6_ = nullptr;
                Brake226 * brake_226_ = nullptr;
                Accel228 * accel_228_ = nullptr;
                Throttle245 * throttle_245_ = nullptr;
                SpeedB4 * speed_b4_ = nullptr;
                WheelspeedAA * wheelspeed_aa_ = nullptr;
                Rpm1C4 * rpm_1c4_ = nullptr;
                Mileage611 * mileage_611_ = nullptr;
                Radar2E6 * radar_2e6_ = nullptr;
                Steering7F8 *steering_7f8_ = nullptr;
                Acc1D3 *acc_1d3_ = nullptr;
                Cruise361 *cruise_361_ = nullptr;

                SteeringController * steer_ = nullptr;

                bool alter_steering_avail_ = false;

                Chassis chassis_;
                std::unique_ptr<std::thread> thread_;
                bool is_chassis_error_ = false;

                std::mutex chassis_error_code_mutex_;
                Chassis::ErrorCode chassis_error_code_ = Chassis::NO_ERROR;

                std::mutex chassis_mask_mutex_;
                int32_t chassis_error_mask_ = 0;

                bool acc_mode = false;
                bool prev_acc_on_mode_ = false;
                size_t acc_cnt = 0;

                bool received_vin_ = false;
            };
        
        } //lexus_rx
    } //canbus
} //apollo
