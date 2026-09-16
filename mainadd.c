#include "fdcan.h"

//After MX initialization:

MX_FDCAN1_Init();

CAN_Init();

//Inside while(1):

static uint8_t counter = 0;

CAN_SendData(counter++);

HAL_Delay(1000);
