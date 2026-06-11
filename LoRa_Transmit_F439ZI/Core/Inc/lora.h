/*
 * sx1278.h
 *
 *  Created on: 05-Jun-2026
 *      Author: Amrutha
 */
#ifndef __LORA_H
#define __LORA_H

#include "main.h"

extern SPI_HandleTypeDef hspi1;

#define LORA_NSS_PORT      GPIOA
#define LORA_NSS_PIN       GPIO_PIN_4

#define LORA_RESET_PORT    GPIOB
#define LORA_RESET_PIN     GPIO_PIN_0

#define LORA_DIO0_PORT     GPIOB
#define LORA_DIO0_PIN      GPIO_PIN_1

#define REG_FIFO                 0x00
#define REG_OP_MODE              0x01
#define REG_FRF_MSB              0x06
#define REG_FRF_MID              0x07
#define REG_FRF_LSB              0x08
#define REG_PA_CONFIG            0x09
#define REG_LNA                  0x0C
#define REG_FIFO_ADDR_PTR        0x0D
#define REG_FIFO_TX_BASE_ADDR    0x0E
#define REG_FIFO_RX_BASE_ADDR    0x0F
#define REG_IRQ_FLAGS            0x12
#define REG_RX_NB_BYTES          0x13
#define REG_PKT_RSSI_VALUE       0x1A
#define REG_MODEM_CONFIG_1       0x1D
#define REG_MODEM_CONFIG_2       0x1E
#define REG_PREAMBLE_MSB         0x20
#define REG_PREAMBLE_LSB         0x21
#define REG_PAYLOAD_LENGTH       0x22
#define REG_MODEM_CONFIG_3       0x26
#define REG_SYNC_WORD            0x39
#define REG_DIO_MAPPING_1        0x40
#define REG_VERSION              0x42

#define MODE_LONG_RANGE_MODE     0x80
#define MODE_SLEEP               0x00
#define MODE_STDBY               0x01
#define MODE_TX                  0x03
#define MODE_RX_CONTINUOUS       0x05

#define PA_BOOST                 0x80

#define IRQ_TX_DONE_MASK         0x08
#define IRQ_RX_DONE_MASK         0x40

uint8_t LoRa_Init(long frequency);



void LoRa_Reset(void);

void LoRa_SetFrequency(long frequency);

void LoRa_SetTxPower(uint8_t power);

void LoRa_Send(uint8_t *data,uint8_t length);

uint8_t LoRa_Receive(uint8_t *buffer);

int16_t LoRa_GetRSSI(void);

uint8_t LoRa_ReadRegister(uint8_t addr);

void LoRa_WriteRegister(uint8_t addr,uint8_t value);

#endif
