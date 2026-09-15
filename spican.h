#ifndef SPICAN_H
#define SPICAN_H

#include "main.h"

void MCP2515_Init(void);

HAL_StatusTypeDef MCP2515_Send(
    uint16_t id,
    uint8_t *data,
    uint8_t len);

uint8_t MCP2515_MessageAvailable(void);

HAL_StatusTypeDef MCP2515_Receive(
    uint16_t *id,
    uint8_t *data,
    uint8_t *len);

#endif
