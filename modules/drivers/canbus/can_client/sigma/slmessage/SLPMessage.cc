#include "SLPMessage.h"
#include <stdio.h>

static int SLP_HEADER_SIZE = 8;

inline uint16_t Reverse16(uint16_t value)
{
    return (((value & 0x00ff) << 8) |
            ((value & 0xff00) >> 8));
}

inline uint32_t Reverse32(uint32_t value) 
{
    return (((value & 0x000000ff) << 24) |
            ((value & 0x0000ff00) <<  8) |
            ((value & 0x00ff0000) >>  8) |
            ((value & 0xff000000) >> 24));
}


inline uint16_t LittleEndian16(uint16_t value)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return value;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return Reverse16(value);
#else
#    error unsupported endianness
#endif
}

inline uint16_t BigEndian16(uint16_t value)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return Reverse16(value);
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return value;
#else
#    error unsupported endianness
#endif
}

inline uint32_t LittleEndian32(uint32_t value)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return value;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return Reverse32(value);
#else
#    error unsupported endianness
#endif
}

inline uint32_t BigEndian32(uint32_t value)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return Reverse32(value);
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return value;
#else
#    error unsupported endianness
#endif
}

SLPMessage::SLPMessage(uint8_t *data, size_t size):
    is_valid(true), can_msg_id(0), can_data( 0 )
{
    struct SLPMsg * msg = ( struct SLPMsg * )data;

    struct SLPHeader * header =  ( struct SLPHeader *)data;

    auto header_crc = crc8( data, SLP_HEADER_SIZE - 1 );

    if ( header_crc != header->crc ) is_valid = false;

    auto message_crc = crc16( data, size - 2 );

    if ( message_crc != msg->crc ) is_valid = false;

    if ( msg->cmd_p != 0x01 && msg->cmd_p != 0x02 ) is_valid = false;
    
    can_msg_id = msg->payload.can_id;

    can_data = msg->payload.data;

    can_data_ptr = data + 16;

    can_num = msg->payload.can_num;
}


SLPMessage::SLPMessage( uint8_t data[8], size_t size, uint32_t can_id_ )
{
    //auto msg_len = 26;
    auto payload_len = size + 8;
    buffer.reserve(26);
    std::vector<uint8_t> header = {0xaa, 0x01, 0xff, 0x10, 0x00, 0x00, 0x00, 0x00 };
    std::vector<uint8_t> canIdVec = std::vector<uint8_t> ( (uint8_t*)&can_id_, (uint8_t*)&(can_id_) + sizeof(uint32_t));
    std::vector<uint8_t> dataVec = std::vector<uint8_t>( (uint8_t*)&data[0], (uint8_t*)&data[0] + size ); 
    
    std::vector<uint8_t> payload = {0x01};
    if ( can_id_ == 0x41 ) payload = {0x02};
    payload.insert( payload.end(), canIdVec.begin(), canIdVec.end() );
    payload.push_back( 0x00 );
    payload.push_back( (uint8_t)size );
    payload.insert( payload.end(), dataVec.begin(), dataVec.end() );
    
    header[5] = payload_len & 0xff;
    header[6] = (payload_len & 0x00ff) >> 8;
    header[7] = crc8( &header[0], 7 );

    buffer.insert( buffer.begin(), header.begin(), header.end() );
    buffer.push_back( 0x01 );
    buffer.insert( buffer.end(), payload.begin(), payload.end() );
    

    auto crc16_data = crc16( buffer.begin()+8, buffer.end() );
    //auto crc_l = crc16_data & 0xff;
    //auto crc_h = (crc16_data & 0xff00) >> 8;
    buffer.push_back( crc16_data & 0xff );
    buffer.push_back( (crc16_data & 0xff00) >> 8 );
    if ( buffer.size() < 26 ) {
        //buffer.insert(buffer.end(), 26 - buffer.size, 0x00);
        std::fill_n (std::back_inserter(buffer), 26 - buffer.size(), 0x00);
    }
}

SLPMessage::~SLPMessage() {

}


uint8_t SLPMessage::crc8(uint8_t *src, size_t len) {
    std::uint8_t crc = 0xFF;
    std::uint8_t i;
    size_t cnt = 0;
    for ( cnt = 0; cnt < len; ++cnt )
    {
        crc ^= *src++;

        for (i = 0; i < 8; i++)
            crc = crc & 0x80 ? (crc << 1) ^ 0x07 : crc << 1;
    }
    return crc;
}


uint16_t SLPMessage::crc16(uint8_t *src, size_t len) {
    uint16_t crc = 0xFFFF;
      
    while (len--)
    {
        uint8_t i;
        crc ^= *src++ << 8;
        for (i = 0; i < 8; i++)
            crc = crc & 0x8000 ? (crc << 1) ^ 0x1021 : crc << 1;
    }
  
    return crc;

}


uint16_t SLPMessage::crc16(const std::vector<uint8_t> &src ) {
    auto len = src.size();
    std::uint8_t crc = 0xFF;
    std::uint8_t i;

    size_t cnt = 0;
    for ( cnt = 0; cnt < len; ++cnt )
    {
        crc ^= src[cnt];
        for (i = 0; i < 8; i++)
            crc = crc & 0x80 ? (crc << 1) ^ 0x07 : crc << 1;
    }
    return crc;
}


std::uint16_t SLPMessage::crc16(std::vector<std::uint8_t>::iterator begin, std::vector<std::uint8_t>::iterator end)
    {
        std::uint16_t crc = 0xFFFF;
        std::uint8_t i;

        while (begin != end)
        {
            crc ^= *begin++ << 8;

            for (i = 0; i < 8; i++)
                crc = crc & 0x8000 ? (crc << 1) ^ 0x1021 : crc << 1;
        }

        return crc;
    }

