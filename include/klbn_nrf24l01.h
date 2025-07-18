/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Kelbaran.
 *
 * Kelbaran is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef KLBN_NRF24L01_H
#define KLBN_NRF24L01_H

#include <stdint.h>
#include <stdbool.h>

// NRF24L01 Register definitions
#define NRF24_REG_CONFIG        0x00
#define NRF24_REG_EN_AA         0x01
#define NRF24_REG_EN_RXADDR     0x02
#define NRF24_REG_SETUP_AW      0x03
#define NRF24_REG_SETUP_RETR    0x04
#define NRF24_REG_RF_CH         0x05
#define NRF24_REG_RF_SETUP      0x06
#define NRF24_REG_STATUS        0x07
#define NRF24_REG_OBSERVE_TX    0x08
#define NRF24_REG_CD            0x09
#define NRF24_REG_RX_ADDR_P0    0x0A
#define NRF24_REG_RX_ADDR_P1    0x0B
#define NRF24_REG_RX_ADDR_P2    0x0C
#define NRF24_REG_RX_ADDR_P3    0x0D
#define NRF24_REG_RX_ADDR_P4    0x0E
#define NRF24_REG_RX_ADDR_P5    0x0F
#define NRF24_REG_TX_ADDR       0x10
#define NRF24_REG_RX_PW_P0      0x11
#define NRF24_REG_RX_PW_P1      0x12
#define NRF24_REG_RX_PW_P2      0x13
#define NRF24_REG_RX_PW_P3      0x14
#define NRF24_REG_RX_PW_P4      0x15
#define NRF24_REG_RX_PW_P5      0x16
#define NRF24_REG_FIFO_STATUS   0x17
#define NRF24_REG_DYNPD         0x1C
#define NRF24_REG_FEATURE       0x1D

// Commands
#define NRF24_CMD_R_REGISTER    0x00
#define NRF24_CMD_W_REGISTER    0x20
#define NRF24_CMD_R_RX_PAYLOAD  0x61
#define NRF24_CMD_W_TX_PAYLOAD  0xA0
#define NRF24_CMD_FLUSH_TX      0xE1
#define NRF24_CMD_FLUSH_RX      0xE2
#define NRF24_CMD_REUSE_TX_PL   0xE3
#define NRF24_CMD_NOP           0xFF

// Config register bits
#define NRF24_CONFIG_PWR_UP     0x02
#define NRF24_CONFIG_PRIM_RX    0x01
#define NRF24_CONFIG_EN_CRC     0x08
#define NRF24_CONFIG_CRCO       0x04
#define NRF24_CONFIG_MASK_MAX_RT 0x10
#define NRF24_CONFIG_MASK_TX_DS 0x20
#define NRF24_CONFIG_MASK_RX_DR 0x40

// Status register bits
#define NRF24_STATUS_RX_DR      0x40
#define NRF24_STATUS_TX_DS      0x20
#define NRF24_STATUS_MAX_RT     0x10
#define NRF24_STATUS_RX_P_NO    0x0E
#define NRF24_STATUS_TX_FULL    0x01

// RF Setup register bits
#define NRF24_RF_SETUP_RF_PWR_0DBM  0x06
#define NRF24_RF_SETUP_RF_DR_2MBPS  0x08

// Maximum payload size
#define NRF24_MAX_PAYLOAD_SIZE  32

// Radio modes
typedef enum {
    KLBN_RADIO_MODE_TX = 0,
    KLBN_RADIO_MODE_RX = 1
} klbn_radio_mode_t;

// Radio status
typedef enum {
    KLBN_RADIO_STATUS_IDLE = 0,
    KLBN_RADIO_STATUS_TX_SUCCESS,
    KLBN_RADIO_STATUS_TX_FAILED,
    KLBN_RADIO_STATUS_RX_READY,
    KLBN_RADIO_STATUS_ERROR
} klbn_radio_status_t;

// Radio data packet
typedef struct {
    uint8_t data[NRF24_MAX_PAYLOAD_SIZE];
    uint8_t length;
} klbn_radio_packet_t;

// Function declarations
void klbn_nrf24l01_init(void);
bool klbn_nrf24l01_is_present(void);
void klbn_nrf24l01_set_mode(klbn_radio_mode_t mode);
klbn_radio_mode_t klbn_nrf24l01_get_mode(void);
bool klbn_nrf24l01_transmit(const klbn_radio_packet_t *packet);
bool klbn_nrf24l01_receive(klbn_radio_packet_t *packet);
klbn_radio_status_t klbn_nrf24l01_get_status(void);
void klbn_nrf24l01_flush_buffers(void);
void klbn_nrf24l01_power_up(void);
void klbn_nrf24l01_power_down(void);

#endif /* KLBN_NRF24L01_H */