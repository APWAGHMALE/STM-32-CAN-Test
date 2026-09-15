#include "spican.h"

extern SPI_HandleTypeDef hspi2;

#define MCP_CS_PORT GPIOA
#define MCP_CS_PIN  GPIO_PIN_4

#define MCP_RESET      0xC0
#define MCP_READ       0x03
#define MCP_WRITE      0x02
#define MCP_BITMOD     0x05

#define MCP_READ_STATUS 0xA0

#define MCP_RTS_TX0    0x81

#define CANCTRL        0x0F
#define CANSTAT        0x0E

#define CNF3           0x28
#define CNF2           0x29
#define CNF1           0x2A

#define TXB0CTRL       0x30
#define TXB0SIDH       0x31

#define RXB0CTRL       0x60
#define RXB0SIDH       0x61

static void MCP_CS_Low(void)
{
    HAL_GPIO_WritePin(
        MCP_CS_PORT,
        MCP_CS_PIN,
        GPIO_PIN_RESET);
}

static void MCP_CS_High(void)
{
    HAL_GPIO_WritePin(
        MCP_CS_PORT,
        MCP_CS_PIN,
        GPIO_PIN_SET);
}

static uint8_t MCP_SPI(uint8_t data)
{
    uint8_t rx;

    HAL_SPI_TransmitReceive(
        &hspi2,
        &data,
        &rx,
        1,
        HAL_MAX_DELAY);

    return rx;
}

static void MCP_Reset(void)
{
    MCP_CS_Low();

    MCP_SPI(MCP_RESET);

    MCP_CS_High();

    HAL_Delay(10);
}

static void MCP_WriteReg(
    uint8_t addr,
    uint8_t value)
{
    MCP_CS_Low();

    MCP_SPI(MCP_WRITE);
    MCP_SPI(addr);
    MCP_SPI(value);

    MCP_CS_High();
}

static uint8_t MCP_ReadReg(
    uint8_t addr)
{
    uint8_t value;

    MCP_CS_Low();

    MCP_SPI(MCP_READ);
    MCP_SPI(addr);

    value = MCP_SPI(0x00);

    MCP_CS_High();

    return value;
}

static void MCP_BitModify(
    uint8_t addr,
    uint8_t mask,
    uint8_t value)
{
    MCP_CS_Low();

    MCP_SPI(MCP_BITMOD);

    MCP_SPI(addr);
    MCP_SPI(mask);
    MCP_SPI(value);

    MCP_CS_High();
}

void MCP2515_Init(void)
{
    MCP_Reset();

    MCP_WriteReg(CNF1,0x00);
    MCP_WriteReg(CNF2,0xB8);
    MCP_WriteReg(CNF3,0x05);

    MCP_WriteReg(RXB0CTRL,0x60);

    MCP_BitModify(
        CANCTRL,
        0xE0,
        0x00);

    HAL_Delay(10);
}

HAL_StatusTypeDef MCP2515_Send(
    uint16_t id,
    uint8_t *data,
    uint8_t len)
{
    uint8_t i;

    MCP_CS_Low();

    MCP_SPI(MCP_WRITE);

    MCP_SPI(TXB0SIDH);

    MCP_SPI(id >> 3);

    MCP_SPI(id << 5);

    MCP_SPI(0x00);

    MCP_SPI(0x00);

    MCP_SPI(len);

    for(i=0;i<len;i++)
    {
        MCP_SPI(data[i]);
    }

    MCP_CS_High();

    MCP_CS_Low();

    MCP_SPI(MCP_RTS_TX0);

    MCP_CS_High();

    return HAL_OK;
}

uint8_t MCP2515_MessageAvailable(void)
{
    uint8_t status;

    MCP_CS_Low();

    MCP_SPI(MCP_READ_STATUS);

    status = MCP_SPI(0x00);

    MCP_CS_High();

    return (status & 0x01);
}

HAL_StatusTypeDef MCP2515_Receive(
    uint16_t *id,
    uint8_t *data,
    uint8_t *len)
{
    uint8_t i;
    uint8_t sidh;
    uint8_t sidl;

    if(!MCP2515_MessageAvailable())
    {
        return HAL_ERROR;
    }

    MCP_CS_Low();

    MCP_SPI(0x90);

    sidh = MCP_SPI(0x00);
    sidl = MCP_SPI(0x00);

    *id = (sidh << 3) | (sidl >> 5);

    MCP_SPI(0x00);
    MCP_SPI(0x00);

    *len = MCP_SPI(0x00) & 0x0F;

    for(i=0;i<*len;i++)
    {
        data[i] = MCP_SPI(0x00);
    }

    MCP_CS_High();

    return HAL_OK;
}
