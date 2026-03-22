#ifndef DATA_PACKET_H
#define DATA_PACKET_H

#include <stdint.h>
#include <stddef.h>

#pragma pack(push, 1)

typedef struct{
    uint8_t sender_id;
    uint32_t index_num;
    uint16_t start_bit;
    uint8_t payload[1024];
    uint16_t payload_len;
    uint16_t auth_token;
    uint32_t checksum;
}packet;

#pragma pack(pop)

static inline uint32_t calculate_crc32(const void* data, size_t len){
    const uint8_t *bytes = (const uint8_t*)data;
    uint32_t crc = 0xFFFFFFFF;

    for(size_t i = 0; i < len; i++){
        crc ^= bytes[i];
        for(int j = 0; j < 8; j++){
            if(crc&1) crc = (crc >> 1) ^ 0xEDB88320;
            else crc >>= 1;
        }
    }
    return ~crc;
}

static inline uint32_t calculate_packet_crc(const packet* pkt){
    size_t len = sizeof(packet) - 4;
    return calculate_crc32(pkt, len);
}

static inline void encrypt_decrypt_payload(uint8_t* payload, size_t len, uint8_t key){
    for(size_t i = 0; i < len; i++) payload[i] ^= key;
}

#endif 