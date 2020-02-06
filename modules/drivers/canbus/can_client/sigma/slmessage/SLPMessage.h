#pragma once

#include <cstdlib>
#include <vector>
#include "SigmaProtocol.h"


class SLPMessage {
private:
    bool is_valid;

    uint32_t can_msg_id;

    uint64_t can_data;

    uint8_t * can_data_ptr;

    std::vector<uint8_t> buffer;

    uint8_t can_num;

protected:

    uint8_t crc8( uint8_t * src, size_t len );

    uint16_t crc16( uint8_t * src, size_t len );

    uint16_t crc16( const std::vector<uint8_t> &src );

    std::uint16_t crc16(std::vector<std::uint8_t>::iterator begin, std::vector<std::uint8_t>::iterator end);

public:
    SLPMessage( uint8_t * data, size_t size );

    SLPMessage( uint8_t data[8], size_t size, uint32_t can_id );

    ~SLPMessage();

    bool isValid( ) const {  return is_valid; }

    uint32_t getCanId() const { return can_msg_id; }

    uint64_t getCanData() const { return can_data; }

    uint8_t * getCanDataPtr() const { return can_data_ptr; }

    std::vector<uint8_t> getBuffer() const { return buffer; }

    uint8_t getCanNum() const { return can_num; }

};

