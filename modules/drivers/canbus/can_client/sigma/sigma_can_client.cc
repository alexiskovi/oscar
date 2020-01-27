#include "modules/drivers/canbus/can_client/sigma/sigma_can_client.h"
#include <algorithm>

namespace apollo {
namespace drivers {
namespace canbus {
namespace can {

using apollo::common::ErrorCode;

static const int MESSAGE_MIN_SIZE = 8;

bool SigmaCanClient::Init(const CANCardParameter &parameter) {
  if (!parameter.has_channel_id()) {
    AERROR << "Init CAN failed: parameter does not have channel id. The "
              "parameter is "
           << parameter.DebugString();
    return false;
  }
  port_ = parameter.channel_id();
  /* Open serial port */ 
  Open();
  cmd_type = -1;
  return true;
}

SigmaCanClient::~SigmaCanClient() {
    Stop();
}

ErrorCode SigmaCanClient::Start() {
  return ErrorCode::OK;
}

void SigmaCanClient::Stop() {
}


// Synchronous transmission of CAN messages
ErrorCode SigmaCanClient::Send(const std::vector<CanFrame> &frames,
                             int32_t *const frame_num) {
  for (size_t i = 0; i < frames.size() && i < MAX_CAN_SEND_FRAME_LEN; ++i) {
     //printf("Try to write can message. Can id: %ld; len: %ld\n", frames[i].id, frames[i].len );
     uint8_t data[8];
     std::memcpy(data, frames[i].data, frames[i].len);
     int frame_len = frames[i].len;
     if ( frames[i].id == 0x2e4 ) frame_len = 5;
     auto toSend = getCanSendMessage( data, frame_len, frames[i].id );
     m_port->write(toSend);
     /*
     printf("\n>>> Write %d bytes to can. Can id: %02x\n", res, frames[i].id );
     for ( auto &s: toSend ) printf("%02x ", s);
     printf("\n");
     */
  }

  return ErrorCode::OK;

}

void canReadCalc( int32_t *len, int32_t ss ) {
    len = &ss;
}

// buf size must be 8 bytes, every time, we receive only one frame
ErrorCode SigmaCanClient::Receive(std::vector<CanFrame> *const frames,
                                int32_t *const frame_num) {

    auto bufferSize = m_port->available();
    if ( bufferSize > 0 ) {
        std::vector<uint8_t> canMsgs = {};
        //printf("<<<< Read  %ld bytes from can\n", bufferSize );
        while( vbuffer.size() < 30 ) {
            uint8_t buf = 0;
            m_port->read( &buf, 1 );
            vbuffer.push_back( buf );
            //printf("%02x ", buf );
            canMsgs = getCanMessage();
            if ( !canMsgs.empty() ) break;
            if ( ( cmd_type == 0x90 ) && ( vbuffer.size() >= 8 ) ) break;
        }
        //printf("\n");
        *frame_num = 1;
        for (int32_t i = 0; i < *frame_num && i < MAX_CAN_RECV_FRAME_LEN; ++i) {
            CanFrame cf;
            cf.id = getCanId();
            cf.len = static_cast<uint8_t>( canMsgs.size() );
            std::memcpy(cf.data, &canMsgs[0], canMsgs.size());
            frames->push_back(cf);
        }
        vbuffer.clear();
    }
    else {
        *frame_num = 0;
    }
    return ErrorCode::OK;
}

std::string SigmaCanClient::GetErrorString(const int32_t status) {
    return "";
}

void SigmaCanClient::Open() {
    /* TODO: get port from config */
    m_port = new serial::Serial("/dev/ttyACM0", 115200, serial::Timeout(50,50,0,50,0) );

    if ( m_port->isOpen() ) {
        is_open = true;
        AINFO << "Successfully open serial port";
        AERROR << "Port baudrate: " << m_port->getBaudrate();
        AERROR << "Port bytesize: " << m_port->getBytesize();
        AERROR << "Port parity: " << m_port->getParity();
        AERROR << "Port flowcontrol: " << m_port->getFlowcontrol();
    }
    else {
        is_open = false;
        AERROR << "Error while open serial port";
    }
}


void SigmaCanClient::setVBuffer( const std::vector<uint8_t> &buff ) {
    vbuffer = buff;
}

std::vector<uint8_t> SigmaCanClient::getCanMessage( ) {
    canId = 0;
    /* First of all check buffer size
     * size of buffer must be at least of 22 bytes
     */
    std::vector<uint8_t> canMsg = {};
    auto buffSize = vbuffer.size();

    if ( buffSize < MESSAGE_MIN_SIZE )  return canMsg;
    std::vector<uint8_t> header_start = {0xaa, 0xff};
    /* Check new message first element */
    auto pos = std::search(vbuffer.begin(), vbuffer.end(), header_start.begin(), header_start.end() );
    if ( pos != vbuffer.end() ) {
        auto startPos = std::distance(vbuffer.begin(), pos);

        uint8_t restSize = static_cast<uint8_t>( buffSize - startPos );
        if ( restSize <  MESSAGE_MIN_SIZE ) return canMsg;
        /* TODO: make other buffer checks */
        /* Try to get CAN message size */
        cmd_type = *(pos+3);

        uint8_t msg_size_l = *(pos+5);
        uint8_t msg_size_h = *(pos+6);
        int msgSize = (msg_size_h << 8)|msg_size_l;

        uint8_t resSize = static_cast<uint8_t>( msgSize + 10 );
        // Result length of message is header_size + msgSize + 2
        if ( cmd_type == 0x10 ) {
            if ( restSize < resSize ) return canMsg;
            
            uint8_t dlcSize = *(pos+15);
            // Can data payload starts at pos+16 and ends at position pos+16+dlcSize
            
            if (  dlcSize > 8 ) return canMsg;
            if ( restSize < (16 + dlcSize + 2 ) ) return canMsg;

	    canMsg = std::vector<uint8_t>( pos+16, pos + 16 + dlcSize);
            uint8_t can_num = *(pos+9);
            //printf("Can num: %02x\n", can_num );
            if ( can_num == 0x01  ) {
                canId = *(pos+10)|(*(pos+11))<<8|(*(pos+12))<<16|(*(pos+13))<<24;
                //printf("\nCan num: %02x; Can id: %02x\n", can_num, canId);
                return canMsg;
            }
        }
        else if ( cmd_type == 0x90 ) {
            /* Feedback from sigma that control command successfully sended */
           //AERROR << "Receive errror from sigma device";
        }
    }
    return canMsg;
}

std::vector<uint8_t> SigmaCanClient::getCanSendMessage( uint8_t data[8], size_t size, uint32_t can_id ) {
    SLPMessage *can_msg = new SLPMessage( data, size, can_id);
    auto res = can_msg->getBuffer();
    delete can_msg;
    return res;
}


}
}
}
}

