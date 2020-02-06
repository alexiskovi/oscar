#pragma once

#include <string>
#include <vector>

#include <string>
#include <vector>
#include "gflags/gflags.h"
#include "modules/common/proto/error_code.pb.h"
#include "modules/drivers/canbus/can_client/can_client.h"
#include "modules/drivers/canbus/common/canbus_consts.h"
#include "modules/drivers/canbus/proto/can_card_parameter.pb.h"
#include "serial/serial.h"
#include "slmessage/SLPMessage.h"
#include <vector>

/**
 * @namespace apollo::drivers::canbus::can
 * @brief apollo::drivers::canbus::can
 */
namespace apollo {
    namespace drivers {
        namespace canbus {
            namespace can {
            class SigmaCanClient : public CanClient {
            public:

                /**
                 * @brief Initialize the Example CAN client by specified CAN card parameters.
                 * @param parameter CAN card parameters to initialize the CAN client.
                 * @return If the initialization is successful.
                 */
                bool Init(const CANCardParameter& parameter) override;

                /**
                 * @brief Destructor
                 */
                virtual ~SigmaCanClient();

                /**
                 * @brief Start the Example CAN client.
                 * @return The status of the start action which is defined by
                 *         apollo::common::ErrorCode.
                 */
                apollo::common::ErrorCode Start() override;

                /**
                 * @brief Stop the Example CAN client.
                 */
                void Stop() override;

                /**
                 * @brief Send messages
                 * @param frames The messages to send.
                 * @param frame_num The amount of messages to send.
                 * @return The status of the sending action which is defined by
                 *         apollo::common::ErrorCode.
                 */
                apollo::common::ErrorCode Send(const std::vector<CanFrame>& frames,
                                 int32_t* const frame_num) override;

               /**
                * @brief Receive messages
                * @param frames The messages to receive.
                * @param frame_num The amount of messages to receive.
                * @return The status of the receiving action which is defined by
                *         apollo::common::ErrorCode.
                */
               apollo::common::ErrorCode Receive(std::vector<CanFrame>* const frames,
                                    int32_t* const frame_num) override;

               /**
                * @brief Get the error string.
                * @param status The status to get the error string.
                */
               std::string GetErrorString(const int32_t status) override;


               void setVBuffer( const std::vector<uint8_t> &buff );

               /* Check buffer message */
               std::vector<uint8_t> getCanMessage( );

               /* Check buffer message */
               std::vector<uint8_t> getCanSendMessage( uint8_t data[8], size_t size, uint32_t can_id );

               uint32_t getCanId() const { return canId; }

            private:

               CANCardParameter::CANChannelId port_;
               serial::Serial *m_port;

               bool is_open;

               void Open();

               bool isOpen();

               uint32_t canId;

               uint8_t cmd_type;

               std::vector<uint8_t> vbuffer;

            };
            }
        }
    }
}
