#pragma once

#include <cstdint>

#pragma pack (push, 1)
struct SLPHeader {
    uint8_t sof;
    uint8_t sa;
    uint8_t da;
    uint8_t cmd;
    uint8_t res;
    uint16_t len;
    uint8_t crc;
};
#pragma pack (pop)

#pragma pack (push, 1)
struct SLPPayload {
    uint8_t can_num;
    uint32_t can_id;
    uint8_t rtr;
    uint8_t dlc;
    uint64_t data;
};
#pragma pack (pop)

#pragma pack (push, 1)
struct SLPMsg {

    SLPHeader hdr;
    uint8_t cmd_p;
    SLPPayload payload;
    uint16_t crc;
};
#pragma pack (pop)
