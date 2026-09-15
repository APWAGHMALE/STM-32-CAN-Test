#ifndef FDCAN_H
#define FDCAN_H

#include "main.h"

void FDCAN_AppInit(void);

HAL_StatusTypeDef FDCAN_Send(
    uint32_t id,
    uint8_t *data,
    uint8_t len);

void FDCAN_ProcessRx(void);

extern uint8_t FDCAN_RxData[8];
extern uint32_t FDCAN_RxId;

#endif

