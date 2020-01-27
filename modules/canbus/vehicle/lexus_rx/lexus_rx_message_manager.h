/*
Khitsko Konstantin 2018-10-14
*/

/**
 * @file lexus_rx_message_manager.h
 * @brief the class of LexusRxMessageManager
 */

#pragma once

#include "modules/canbus/proto/chassis_detail.pb.h"
#include "modules/drivers/canbus/can_comm/message_manager.h"

/**
 * @namespace apollo::canbus::lexus_rx
 * @brief apollo::canbus::lexus_rx
 */
namespace apollo {
    namespace canbus {
        namespace lexus_rx {

            using ::apollo::drivers::canbus::MessageManager;

            /**
            * @class LexusRxMessageManager
            *
            * @brief implementation of MessageManager for lexus_rx vehicle
            */
            class LexusRxMessageManager
                : public MessageManager<::apollo::canbus::ChassisDetail> {
            public:
                /**
                * @brief construct a lexus_rx message manager. protocol data for send and
                * receive are added in the contruction.
                */
                LexusRxMessageManager();
                virtual ~LexusRxMessageManager();
            };

        }  // namespace lexus_rx
    }  // namespace canbus
}  // namespace apollo