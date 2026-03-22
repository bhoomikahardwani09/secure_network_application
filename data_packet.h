#ifndef DATA_PACKET_H
#define DATA_PACKET_H

#include <stdint.h>
#include <stddef.h>

typedef struct{
    uint8_t sender_id;
    uint32_t index_num;
    uint16_t start_bit;
    uint8_t payload[1024];
    uint16_t payload_len;
    uint32_t checksum;
}packet;

#pragma pack(pop)

static inline uint32_t calculate_crc32(const void* data, size_t len){
    const uint8_t *bytes = (const uint8_t*)data;
    int32_t crc = 0xFFFFFFFF;

    for(int i = 0; i < len; i++){
        crc ^= bytes[i];
        for(int j = 0; j < 8; j++){
            if(crc&1) crc = (crc >> 1) ^ 0xEDB88320;
            else crc >>= 1;
        }
    }
    return ~crc;
}

static inline uint32_t calculate_packet_crc(const packet* packet){
    size_t len = sizeof(packet) - 4;
    return calculate_crc32(packet, len);
}



#endif 