#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "Swap.h"

void Bsort(void* arrPoint, int arrSize, int typeSize, int (*Comparator)(const void*, const void*))
{
    for (int pass = 0; pass < arrSize - 1; pass++)
        for (int i = 0; i < arrSize - pass - 1; i++)
            if (Comparator((uint8_t *)arrPoint + i * typeSize, (uint8_t *)arrPoint + (i + 1) * typeSize) > 0)  
                SwapOpt(((uint8_t *)arrPoint + i * typeSize), ((uint8_t *)arrPoint + (i + 1) * typeSize), typeSize);         
}