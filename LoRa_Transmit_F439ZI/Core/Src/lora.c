/*
 * sx1278.c
 *
 *  Created on: 05-Jun-2026
 *      Author: Amrutha
 */

#include "lora.h"
#include "stdio.h"



void LoRa_Reset(void)
{
    HAL_GPIO_WritePin(
        LORA_RESET_PORT,
        LORA_RESET_PIN,
        GPIO_PIN_RESET);

    HAL_Delay(10);

    HAL_GPIO_WritePin(
        LORA_RESET_PORT,
        LORA_RESET_PIN,
        GPIO_PIN_SET);

    HAL_Delay(10);
}

uint8_t LoRa_ReadRegister(uint8_t addr)
{
    uint8_t tx[2];
    uint8_t rx[2];

    tx[0] = addr & 0x7F;
    tx[1] = 0;

    HAL_GPIO_WritePin(
        LORA_NSS_PORT,
        LORA_NSS_PIN,
        GPIO_PIN_RESET);

    HAL_SPI_TransmitReceive(
        &hspi1,
        tx,
        rx,
        2,
        HAL_MAX_DELAY);

    HAL_GPIO_WritePin(
        LORA_NSS_PORT,
        LORA_NSS_PIN,
        GPIO_PIN_SET);

    return rx[1];
}

void LoRa_WriteRegister(uint8_t addr,uint8_t value)
{
    uint8_t tx[2];

    tx[0] = addr | 0x80;
    tx[1] = value;

    HAL_GPIO_WritePin(
        LORA_NSS_PORT,
        LORA_NSS_PIN,
        GPIO_PIN_RESET);

    HAL_SPI_Transmit(
        &hspi1,
        tx,
        2,
        HAL_MAX_DELAY);

    HAL_GPIO_WritePin(
        LORA_NSS_PORT,
        LORA_NSS_PIN,
        GPIO_PIN_SET);
}

void LoRa_SetFrequency(long frequency)
{
    uint64_t frf;

    frf = ((uint64_t)frequency << 19) / 32000000;

    LoRa_WriteRegister(REG_FRF_MSB,(uint8_t)(frf >> 16));
    LoRa_WriteRegister(REG_FRF_MID,(uint8_t)(frf >> 8));
    LoRa_WriteRegister(REG_FRF_LSB,(uint8_t)(frf));
}

void LoRa_SetTxPower(uint8_t power)
{
    if(power > 17)
        power = 17;

    LoRa_WriteRegister(
        REG_PA_CONFIG,
        PA_BOOST | (power - 2));
}

uint8_t LoRa_Init(long frequency)
{
    LoRa_Reset();

    uint8_t version;

    version = LoRa_ReadRegister(REG_VERSION);

    if(version != 0x12)
        return 0;

    LoRa_WriteRegister(
        REG_OP_MODE,
        MODE_LONG_RANGE_MODE | MODE_SLEEP);

    HAL_Delay(10);

    LoRa_SetFrequency(frequency);

    LoRa_WriteRegister(REG_FIFO_TX_BASE_ADDR,0);
    LoRa_WriteRegister(REG_FIFO_RX_BASE_ADDR,0);

    LoRa_WriteRegister(
        REG_LNA,
        LoRa_ReadRegister(REG_LNA) | 0x03);

    LoRa_WriteRegister(REG_MODEM_CONFIG_3,0x04);

    LoRa_SetTxPower(17);

    LoRa_WriteRegister(
        REG_OP_MODE,
        MODE_LONG_RANGE_MODE | MODE_STDBY);

    return 1;
}

void LoRa_Send(uint8_t *data,uint8_t length)
{
    uint8_t i;

    LoRa_WriteRegister(REG_OP_MODE,
            MODE_LONG_RANGE_MODE |
            MODE_STDBY);

    LoRa_WriteRegister(REG_FIFO_ADDR_PTR,0);
    LoRa_WriteRegister(REG_PAYLOAD_LENGTH,0);

    for(i=0;i<length;i++)
    {
        LoRa_WriteRegister(REG_FIFO,data[i]);
    }

    LoRa_WriteRegister(
        REG_PAYLOAD_LENGTH,
        length);

    LoRa_WriteRegister(
        REG_OP_MODE,
        MODE_LONG_RANGE_MODE |
        MODE_TX);



    while((LoRa_ReadRegister(REG_IRQ_FLAGS)
            & IRQ_TX_DONE_MASK)==0);

    printf("TX IRQ = 0x%02X\r\n",
           LoRa_ReadRegister(REG_IRQ_FLAGS));

    LoRa_WriteRegister(
        REG_IRQ_FLAGS,
        IRQ_TX_DONE_MASK);
}

uint8_t LoRa_Receive(uint8_t *buffer)
{
    uint8_t len;
    uint8_t i;

    if(!(LoRa_ReadRegister(REG_IRQ_FLAGS)
            & IRQ_RX_DONE_MASK))
        return 0;

    len = LoRa_ReadRegister(REG_RX_NB_BYTES);

    LoRa_WriteRegister(REG_FIFO_ADDR_PTR,0);

    for(i=0;i<len;i++)
    {
        buffer[i] =
            LoRa_ReadRegister(REG_FIFO);
    }

   LoRa_WriteRegister(
        REG_IRQ_FLAGS,
        IRQ_RX_DONE_MASK);

   /* LoRa_WriteRegister(
        REG_FIFO_ADDR_PTR,
        LoRa_ReadRegister(REG_FIFO_RX_CURRENT_ADDR)); */

    return len;
}

int16_t LoRa_GetRSSI(void)
{
    return LoRa_ReadRegister(
            REG_PKT_RSSI_VALUE) - 164;
}
