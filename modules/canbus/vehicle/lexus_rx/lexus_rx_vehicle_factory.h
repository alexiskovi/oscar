/*
Khitsko Konstantin 2018-10-14
*/

/**
 * @file lexus_rx_vehicle_factory.h
 */

#pragma once 

#include <memory>

#include "modules/canbus/proto/vehicle_parameter.pb.h"
#include "modules/canbus/vehicle/abstract_vehicle_factory.h"
#include "modules/canbus/vehicle/vehicle_controller.h"
#include "modules/drivers/canbus/can_comm/message_manager.h"

/**
 * @namespace apollo::canbus
 * @brief apollo::canbus
 */
namespace apollo {
    namespace canbus {

        /**
        * @class LexusRxVehicleFactory
        *
        * @brief this class is inherited from AbstractVehicleFactory. It can be used to
        * create controller and message manager for lexus_rx vehicle.
        */
        class LexusRxVehicleFactory : public AbstractVehicleFactory {
        public:
            /**
            * @brief destructor
            */
            virtual ~LexusRxVehicleFactory() = default;

            /**
            * @brief create lexus_rx vehicle controller
            * @returns a unique_ptr that points to the created controller
            */
            std::unique_ptr<VehicleController> CreateVehicleController() override;

            /**
            * @brief create lexus_rx message manager
            * @returns a unique_ptr that points to the created message manager
            */
            std::unique_ptr<MessageManager<::apollo::canbus::ChassisDetail>>
            CreateMessageManager() override;
        };
    }  // namespace canbus
}  // namespace apollo
