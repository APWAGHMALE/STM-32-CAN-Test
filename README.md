# STM32G491RE FDCAN + MCP2515 SPI CAN Example

## Overview

This project demonstrates simultaneous CAN transmission using:

1. STM32G491RE internal FDCAN peripheral
2. MCP2515 SPI CAN controller

Both interfaces transmit the same CAN frame every second.

---

## Hardware

### MCU

- STM32 NUCLEO-G491RE

### CAN Transceivers

- MCP2551 (for FDCAN)
- MCP2551 (for MCP2515)

### SPI CAN Controller

- MCP2515
- Crystal: 8 MHz

---

## Pin Configuration

### FDCAN1

| Signal | STM32 Pin |
|----------|----------|
| FDCAN_RX | PA11 |
| FDCAN_TX | PA12 |

---

### SPI2

| Signal | STM32 Pin |
|----------|----------|
| SPI2_SCK | PB13 |
| SPI2_MISO | PB14 |
| SPI2_MOSI | PB15 |

---

### MCP2515 Control Pins

| Signal | STM32 Pin |
|----------|----------|
| CS | PA4 |
| INT | PB0 |

---

## CubeMX Configuration

### SYS

Debug:

```text
Serial Wire
```

---

### FDCAN1

Mode:

```text
Normal
```

Frame Format:

```text
Classic CAN
```

Auto Retransmission:

```text
Enable
```

Protocol Exception:

```text
Disable
```

---

### FDCAN Timing

500 kbps

```text
Nominal Prescaler = 17
Nominal Time Seg1 = 15
Nominal Time Seg2 = 4
Nominal SJW = 1
```

---

### FDCAN Message RAM

```text
Standard Filters = 1
Extended Filters = 0

Rx FIFO0 Elements = 8
Rx FIFO1 Elements = 0

Rx Buffers = 0

Tx Event FIFO Elements = 0

Tx FIFO Queue Elements = 8
```

---

### SPI2

Mode:

```text
Full Duplex Master
```

Parameters:

```text
Data Size = 8 Bits
First Bit = MSB First
NSS = Software
Clock Polarity = Low
Clock Phase = 1 Edge
Baud Rate Prescaler = 16
```

---

### PA4 (CS)

```text
GPIO Output
Push Pull
No Pull
Very High Speed
Initial Level High
```

---

### PB0 (INT)

```text
GPIO_EXTI0
Pull-Up
```

Enable:

```text
EXTI Line0 Interrupt
```

---

## Project Files

```text
Core/
├── Inc/
│   ├── fdcan_app.h
│   └── spican.h
│
├── Src/
│   ├── fdcan_app.c
│   └── spican.c
│
└── main.c
```

---

## Initialization

Add in main.c:

```c
#include "fdcan_app.h"
#include "spican.h"
```

After CubeMX initialization:

```c
MX_GPIO_Init();
MX_FDCAN1_Init();
MX_SPI2_Init();

FDCAN_AppInit();
MCP2515_Init();
```

---

## Example Transmission

```c
uint8_t txData[8] =
{
    0x11,
    0x22,
    0x33,
    0x44,
    0x55,
    0x66,
    0x77,
    0x88
};
```

Main loop:

```c
while(1)
{
    FDCAN_Send(
        0x123,
        txData,
        8);

    MCP2515_Send(
        0x123,
        txData,
        8);

    HAL_Delay(1000);
}
```

---

## Receive Support

### FDCAN

Received frame information is stored in:

```c
extern uint8_t FDCAN_RxData[8];
extern uint32_t FDCAN_RxId;
```

Example:

```c
if(FDCAN_RxId != 0)
{
    FDCAN_RxId = 0;
}
```

---

### MCP2515

Check for received frame:

```c
uint16_t rxId;
uint8_t rxData[8];
uint8_t rxLen;

if(MCP2515_Receive(
        &rxId,
        rxData,
        &rxLen) == HAL_OK)
{
    // Frame received
}
```

---

## Notes

### Important

MCP2515 supports:

```text
Classical CAN only
```

It does NOT support:

```text
CAN-FD
```

Therefore FDCAN is configured as:

```c
TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
```

---

### Power Supply

STM32:

```text
3.3V
```

MCP2551:

```text
5V
```

Common ground is mandatory.

---

## Tested Configuration

```text
STM32G491RE
FDCAN1
SPI2
MCP2515 (8 MHz)
MCP2551
500 kbps Classical CAN
```
