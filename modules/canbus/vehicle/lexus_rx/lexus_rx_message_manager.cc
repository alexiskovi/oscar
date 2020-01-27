#include "modules/canbus/vehicle/lexus_rx/lexus_rx_message_manager.h"
#include "modules/canbus/vehicle/lexus_rx/protocol/steering_25.h"
#include "modules/canbus/vehicle/lexus_rx/protocol/steering_24.h"
#include "modules/canbus/vehicle/lexus_rx/protocol/steering_260.h"
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

#include "modules/canbus/vehicle/lexus_rx/protocol/steering_2e4.h"
#include "modules/canbus/vehicle/lexus_rx/protocol/steering_262.h"
#include "modules/canbus/vehicle/lexus_rx/protocol/mileage_611.h"
#include "modules/canbus/vehicle/lexus_rx/protocol/turnsignal_614.h"
#include "modules/canbus/vehicle/lexus_rx/protocol/radar_2e6.h"
#include "modules/canbus/vehicle/lexus_rx/protocol/steering_41.h"
#include "modules/canbus/vehicle/lexus_rx/protocol/steering_7f0.h"
#include "modules/canbus/vehicle/lexus_rx/protocol/steering_7f8.h"
#include "modules/canbus/vehicle/lexus_rx/protocol/acc_1d3.h"
#include "modules/canbus/vehicle/lexus_rx/protocol/steering_7f1.h"
#include "modules/canbus/vehicle/lexus_rx/protocol/cruise_361.h"
#include "modules/canbus/vehicle/lexus_rx/protocol/steering_7fb.h"

namespace apollo {
    namespace canbus {
        namespace lexus_rx {

	    LexusRxMessageManager::LexusRxMessageManager() {
                AddRecvProtocolData<Steering24, true>();
                AddRecvProtocolData<Steering25, true>();
                AddRecvProtocolData<Steering260, true>();
                AddRecvProtocolData<Steering262, true>();
                AddRecvProtocolData<Steering2E4, true>();
                //AddRecvProtocolData<Gear3BC, true>();
                AddRecvProtocolData<BrakeA6, true>();
                AddRecvProtocolData<Brake226, true>();
                AddRecvProtocolData<Accel228, true>();
                AddRecvProtocolData<Throttle245, true>();
                AddRecvProtocolData<Throttle343, true>();
                AddRecvProtocolData<SpeedB4, true>();
                AddRecvProtocolData<WheelspeedAA, true>();
                AddRecvProtocolData<Rpm1C4, true>();
                AddRecvProtocolData<Mileage611, true>();
                AddRecvProtocolData<Turnsignal614, true>();
                AddRecvProtocolData<Steering7F8, true>();
                AddRecvProtocolData<Acc1D3, true>();
                AddRecvProtocolData<Cruise361, true>();

                //AddSendProtocolData<Steering2E4, true>();
                AddSendProtocolData<Steering41, true>();
                AddSendProtocolData<Throttle343, true>();
                AddSendProtocolData<Radar2E6, true>();
                AddSendProtocolData<Steering7F0, true>();
                AddSendProtocolData<Steering7F1, true>();
                AddSendProtocolData<Steering7FB, true>();
                AddSendProtocolData<Gear777, true>();
            }

            LexusRxMessageManager::~LexusRxMessageManager() {}

        }  // namespace lexus_rx
    }  // namespace canbus
}  // namespace apollo
