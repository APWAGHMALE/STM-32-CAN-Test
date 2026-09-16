#include "fdcan.h"
#include <stdio.h>

extern FDCAN_HandleTypeDef hfdcan1;

static FDCAN_TxHeaderTypeDef TxHeader;
static FDCAN_RxHeaderTypeDef RxHeader;

uint8_t TxData[8];
uint8_t RxData[8];

void CAN_Init(void)
{
    FDCAN_FilterTypeDef sFilterConfig;

    sFilterConfig.IdType = FDCAN_STANDARD_ID;
    sFilterConfig.FilterIndex = 0;

    sFilterConfig.FilterType = FDCAN_FILTER_MASK;

    sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;

    sFilterConfig.FilterID1 = 0x000;
    sFilterConfig.FilterID2 = 0x000;

    HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig);

    HAL_FDCAN_Start(&hfdcan1);

    HAL_FDCAN_ActivateNotification(
            &hfdcan1,
            FDCAN_IT_RX_FIFO0_NEW_MESSAGE,
            0);

    TxHeader.Identifier = 0x123;
    TxHeader.IdType = FDCAN_STANDARD_ID;
    TxHeader.TxFrameType = FDCAN_DATA_FRAME;
    TxHeader.DataLength = FDCAN_DLC_BYTES_8;

    TxHeader.ErrorStateIndicator =
            FDCAN_ESI_ACTIVE;

    TxHeader.BitRateSwitch =
            FDCAN_BRS_OFF;

    TxHeader.FDFormat =
            FDCAN_CLASSIC_CAN;

    TxHeader.TxEventFifoControl =
            FDCAN_NO_TX_EVENTS;

    TxHeader.MessageMarker = 0;

    printf("\r\nFDCAN Started\r\n");
}

void CAN_SendData(uint8_t counter)
{
    TxData[0] = counter;
    TxData[1] = counter + 1;
    TxData[2] = counter + 2;
    TxData[3] = counter + 3;
    TxData[4] = counter + 4;
    TxData[5] = counter + 5;
    TxData[6] = counter + 6;
    TxData[7] = counter + 7;

    if(HAL_FDCAN_AddMessageToTxFifoQ(
            &hfdcan1,
            &TxHeader,
            TxData) == HAL_OK)
    {
        printf("TX ID:0x%03lX Data:",
                TxHeader.Identifier);

        for(int i=0;i<8;i++)
        {
            printf("%02X ", TxData[i]);
        }

        printf("\r\n");
    }
}

void HAL_FDCAN_RxFifo0Callback(
        FDCAN_HandleTypeDef *hfdcan,
        uint32_t RxFifo0ITs)
{
    if(RxFifo0ITs &
       FDCAN_IT_RX_FIFO0_NEW_MESSAGE)
    {
        HAL_FDCAN_GetRxMessage(
                hfdcan,
                FDCAN_RX_FIFO0,
                &RxHeader,
                RxData);

        printf("RX ID:0x%03lX Data:",
                RxHeader.Identifier);

        for(int i=0;i<8;i++)
        {
            printf("%02X ", RxData[i]);
        }

        printf("\r\n");
    }
}
