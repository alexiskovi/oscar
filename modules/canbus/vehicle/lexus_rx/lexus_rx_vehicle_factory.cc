/*
Khitsko Konstantin 2018-10-14
*/

#include "modules/canbus/vehicle/lexus_rx/lexus_rx_vehicle_factory.h"

#include "modules/canbus/vehicle/lexus_rx/lexus_rx_controller.h"
#include "modules/canbus/vehicle/lexus_rx/lexus_rx_message_manager.h"
#include "cyber/common/log.h"
#include "modules/common/util/util.h"

namespace apollo {
    namespace canbus {

        std::unique_ptr<VehicleController>
        LexusRxVehicleFactory::CreateVehicleController() {
            return std::unique_ptr<VehicleController>(new lexus_rx::LexusRxController());
        }

        std::unique_ptr<MessageManager<::apollo::canbus::ChassisDetail>>
        LexusRxVehicleFactory::CreateMessageManager() {
            return std::unique_ptr<MessageManager<::apollo::canbus::ChassisDetail>>(
            new lexus_rx::LexusRxMessageManager());
        }

    }  // namespace canbus
}  // namespace apollo
