#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include <assert.h>

void SwapOpt(void* point1, void* point2, int typeSize)
{
    uint64_t temp = 0;
    int pos = 0;

    //assert(sizeof(uint64_t) == 8);

    while(typeSize >> 3)
    {
        memcpy(&temp, ((uint8_t *)point1 + pos), sizeof(uint64_t));
        memcpy(((uint8_t *)point1 + pos), ((uint8_t *)point2 + pos), sizeof(uint64_t));
        memcpy(((uint8_t *)point2 + pos), &temp, sizeof(uint64_t));
        pos += 8;
        typeSize -= 8;    
    }
    if (typeSize & 0x04)
    {
        memcpy(&temp, ((uint8_t *)point1 + pos), sizeof(uint32_t));
        memcpy(((uint8_t *)point1 + pos), ((uint8_t *)point2 + pos), sizeof(uint32_t));
        memcpy(((uint8_t *)point2 + pos), &temp, sizeof(uint32_t));
        pos += 4;
    }
    if (typeSize & 0x02)
    {
        memcpy(&temp, ((uint8_t *)point1 + pos), sizeof(uint16_t));
        memcpy(((uint8_t *)point1 + pos), ((uint8_t *)point2 + pos), sizeof(uint16_t));
        memcpy(((uint8_t *)point2 + pos), &temp, sizeof(uint16_t));
        pos += 2;
    }
    if (typeSize & 0x01)
    {
        memcpy(&temp, ((uint8_t *)point1 + pos), sizeof(uint8_t));
        memcpy(((uint8_t *)point1 + pos), ((uint8_t *)point2 + pos), sizeof(uint8_t));
        memcpy(((uint8_t *)point2 + pos), &temp, sizeof(uint8_t));        
    }
}