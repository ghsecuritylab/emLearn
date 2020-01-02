#ifndef __DRV_SDRAM_H
#define __DRV_SDRAM_H

#include <board.h>

#define SDRAM_SIZE          0x2000000
#define SDRAM_BANK_ADDR     ((rt_uint32_t)0xD0000000)
#define SDRAM_TIMEOUT       ((rt_uint32_t)0xFFFF)

#define SDRAM_MODEREG_BURST_LENGTH_1                ((rt_uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2                ((rt_uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4                ((rt_uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8                ((rt_uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL         ((rt_uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED        ((rt_uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2                 ((rt_uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3                 ((rt_uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD       ((rt_uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED    ((rt_uint16_t)0x0000) 
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE        ((rt_uint16_t)0x0200)

void SDRAM_Init(void);

#endif
