#include "fdcan.h"

extern FDCAN_HandleTypeDef hfdcan1;

uint8_t FDCAN_RxData[8];
uint32_t FDCAN_RxId = 0;

static FDCAN_RxHeaderTypeDef RxHeader;

void FDCAN_AppInit(void)
{
    FDCAN_FilterTypeDef sFilterConfig;

    sFilterConfig.IdType = FDCAN_STANDARD_ID;
    sFilterConfig.FilterIndex = 0;
    sFilterConfig.FilterType = FDCAN_FILTER_MASK;
    sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;

    /* Accept all standard IDs */
    sFilterConfig.FilterID1 = 0x000;
    sFilterConfig.FilterID2 = 0x000;

    if(HAL_FDCAN_ConfigFilter(
            &hfdcan1,
            &sFilterConfig) != HAL_OK)
    {
        Error_Handler();
    }

    if(HAL_FDCAN_Start(&hfdcan1) != HAL_OK)
    {
        Error_Handler();
    }

    if(HAL_FDCAN_ActivateNotification(
            &hfdcan1,
            FDCAN_IT_RX_FIFO0_NEW_MESSAGE,
            0) != HAL_OK)
    {
        Error_Handler();
    }
}

HAL_StatusTypeDef FDCAN_Send(
    uint32_t id,
    uint8_t *data,
    uint8_t len)
{
    FDCAN_TxHeaderTypeDef TxHeader;

    TxHeader.Identifier = id;
    TxHeader.IdType = FDCAN_STANDARD_ID;

    TxHeader.TxFrameType = FDCAN_DATA_FRAME;

    switch(len)
    {
        case 0:
            TxHeader.DataLength = FDCAN_DLC_BYTES_0;
            break;

        case 1:
            TxHeader.DataLength = FDCAN_DLC_BYTES_1;
            break;

        case 2:
            TxHeader.DataLength = FDCAN_DLC_BYTES_2;
            break;

        case 3:
            TxHeader.DataLength = FDCAN_DLC_BYTES_3;
            break;

        case 4:
            TxHeader.DataLength = FDCAN_DLC_BYTES_4;
            break;

        case 5:
            TxHeader.DataLength = FDCAN_DLC_BYTES_5;
            break;

        case 6:
            TxHeader.DataLength = FDCAN_DLC_BYTES_6;
            break;

        case 7:
            TxHeader.DataLength = FDCAN_DLC_BYTES_7;
            break;

        default:
            TxHeader.DataLength = FDCAN_DLC_BYTES_8;
            break;
    }

    TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
    TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
    TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    TxHeader.MessageMarker = 0;

    return HAL_FDCAN_AddMessageToTxFifoQ(
                &hfdcan1,
                &TxHeader,
                data);
}

void FDCAN_ProcessRx(void)
{
    if(HAL_FDCAN_GetRxMessage(
            &hfdcan1,
            FDCAN_RX_FIFO0,
            &RxHeader,
            FDCAN_RxData) == HAL_OK)
    {
        FDCAN_RxId = RxHeader.Identifier;
    }
}

void HAL_FDCAN_RxFifo0Callback(
        FDCAN_HandleTypeDef *hfdcan,
        uint32_t RxFifo0ITs)
{
    if((RxFifo0ITs &
        FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != 0U)
    {
        FDCAN_ProcessRx();
    }
}

