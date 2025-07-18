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
#include <stddef.h>

/* NRF24L01 Commands */
#define NRF24L01_CMD_R_REGISTER         0x00
#define NRF24L01_CMD_W_REGISTER         0x20
#define NRF24L01_CMD_R_RX_PAYLOAD       0x61
#define NRF24L01_CMD_W_TX_PAYLOAD       0xA0
#define NRF24L01_CMD_FLUSH_TX           0xE1
#define NRF24L01_CMD_FLUSH_RX           0xE2
#define NRF24L01_CMD_REUSE_TX_PL        0xE3
#define NRF24L01_CMD_R_RX_PL_WID        0x60
#define NRF24L01_CMD_W_ACK_PAYLOAD      0xA8
#define NRF24L01_CMD_W_TX_PAYLOAD_NO_ACK 0xB0
#define NRF24L01_CMD_NOP                0xFF

/* NRF24L01 Registers */
#define NRF24L01_REG_CONFIG             0x00
#define NRF24L01_REG_EN_AA              0x01
#define NRF24L01_REG_EN_RXADDR          0x02
#define NRF24L01_REG_SETUP_AW           0x03
#define NRF24L01_REG_SETUP_RETR         0x04
#define NRF24L01_REG_RF_CH              0x05
#define NRF24L01_REG_RF_SETUP           0x06
#define NRF24L01_REG_STATUS             0x07
#define NRF24L01_REG_OBSERVE_TX         0x08
#define NRF24L01_REG_RPD                0x09
#define NRF24L01_REG_RX_ADDR_P0         0x0A
#define NRF24L01_REG_RX_ADDR_P1         0x0B
#define NRF24L01_REG_RX_ADDR_P2         0x0C
#define NRF24L01_REG_RX_ADDR_P3         0x0D
#define NRF24L01_REG_RX_ADDR_P4         0x0E
#define NRF24L01_REG_RX_ADDR_P5         0x0F
#define NRF24L01_REG_TX_ADDR            0x10
#define NRF24L01_REG_RX_PW_P0           0x11
#define NRF24L01_REG_RX_PW_P1           0x12
#define NRF24L01_REG_RX_PW_P2           0x13
#define NRF24L01_REG_RX_PW_P3           0x14
#define NRF24L01_REG_RX_PW_P4           0x15
#define NRF24L01_REG_RX_PW_P5           0x16
#define NRF24L01_REG_FIFO_STATUS        0x17
#define NRF24L01_REG_DYNPD              0x1C
#define NRF24L01_REG_FEATURE            0x1D

/* CONFIG Register bits */
#define NRF24L01_CONFIG_MASK_RX_DR      0x40
#define NRF24L01_CONFIG_MASK_TX_DS      0x20
#define NRF24L01_CONFIG_MASK_MAX_RT     0x10
#define NRF24L01_CONFIG_EN_CRC          0x08
#define NRF24L01_CONFIG_CRCO            0x04
#define NRF24L01_CONFIG_PWR_UP          0x02
#define NRF24L01_CONFIG_PRIM_RX         0x01

/* STATUS Register bits */
#define NRF24L01_STATUS_RX_DR           0x40
#define NRF24L01_STATUS_TX_DS           0x20
#define NRF24L01_STATUS_MAX_RT          0x10
#define NRF24L01_STATUS_RX_P_NO         0x0E
#define NRF24L01_STATUS_TX_FULL         0x01

/* RF_SETUP Register bits */
#define NRF24L01_RF_SETUP_CONT_WAVE     0x80
#define NRF24L01_RF_SETUP_RF_DR_LOW     0x20
#define NRF24L01_RF_SETUP_PLL_LOCK      0x10
#define NRF24L01_RF_SETUP_RF_DR_HIGH    0x08
#define NRF24L01_RF_SETUP_RF_PWR        0x06

/* Constants */
#define NRF24L01_MAX_PAYLOAD_SIZE       32
#define NRF24L01_ADDRESS_WIDTH          5
#define NRF24L01_DEFAULT_CHANNEL        76
#define NRF24L01_DEFAULT_RETRIES        3
#define NRF24L01_DEFAULT_DELAY          5

/**
 * @brief NRF24L01 error codes
 */
typedef enum {
    KLBN_NRF24L01_OK = 0,
    KLBN_NRF24L01_ERROR_TIMEOUT,
    KLBN_NRF24L01_ERROR_NOT_FOUND,
    KLBN_NRF24L01_ERROR_TX_FAILED,
    KLBN_NRF24L01_ERROR_RX_EMPTY,
    KLBN_NRF24L01_ERROR_INVALID_PARAM,
    KLBN_NRF24L01_ERROR_NOT_INITIALIZED
} klbn_nrf24l01_error_t;

/**
 * @brief NRF24L01 power levels
 */
typedef enum {
    KLBN_NRF24L01_POWER_M18DBM = 0,    // -18 dBm
    KLBN_NRF24L01_POWER_M12DBM,        // -12 dBm  
    KLBN_NRF24L01_POWER_M6DBM,         // -6 dBm
    KLBN_NRF24L01_POWER_0DBM           // 0 dBm
} klbn_nrf24l01_power_t;

/**
 * @brief NRF24L01 data rates
 */
typedef enum {
    KLBN_NRF24L01_DATARATE_250KBPS = 0,
    KLBN_NRF24L01_DATARATE_1MBPS,
    KLBN_NRF24L01_DATARATE_2MBPS
} klbn_nrf24l01_datarate_t;

/**
 * @brief NRF24L01 operating modes
 */
typedef enum {
    KLBN_NRF24L01_MODE_POWER_DOWN = 0,
    KLBN_NRF24L01_MODE_STANDBY,
    KLBN_NRF24L01_MODE_TX,
    KLBN_NRF24L01_MODE_RX
} klbn_nrf24l01_mode_t;

/**
 * @brief NRF24L01 configuration structure
 */
typedef struct {
    uint8_t channel;                        // RF channel (0-127)
    klbn_nrf24l01_power_t power;           // TX power level
    klbn_nrf24l01_datarate_t datarate;     // Data rate
    uint8_t retries;                        // Auto retransmit count (0-15)
    uint8_t delay;                          // Auto retransmit delay (0-15)
    bool auto_ack;                          // Enable auto acknowledgment
    bool dynamic_payload;                   // Enable dynamic payload length
    uint8_t payload_size;                   // Fixed payload size (1-32)
} klbn_nrf24l01_config_t;

/**
 * @brief Initialize NRF24L01 module
 * @param config Configuration structure (NULL for default)
 * @return Error code
 */
klbn_nrf24l01_error_t klbn_nrf24l01_init(const klbn_nrf24l01_config_t *config);

/**
 * @brief Deinitialize NRF24L01 module
 * @return Error code
 */
klbn_nrf24l01_error_t klbn_nrf24l01_deinit(void);

/**
 * @brief Set operating mode
 * @param mode Operating mode
 * @return Error code
 */
klbn_nrf24l01_error_t klbn_nrf24l01_set_mode(klbn_nrf24l01_mode_t mode);

/**
 * @brief Set TX address
 * @param address Address buffer (5 bytes)
 * @return Error code
 */
klbn_nrf24l01_error_t klbn_nrf24l01_set_tx_address(const uint8_t *address);

/**
 * @brief Set RX address for pipe 0
 * @param address Address buffer (5 bytes)
 * @return Error code
 */
klbn_nrf24l01_error_t klbn_nrf24l01_set_rx_address(const uint8_t *address);

/**
 * @brief Transmit data
 * @param data Data buffer
 * @param length Data length (max 32 bytes)
 * @return Error code
 */
klbn_nrf24l01_error_t klbn_nrf24l01_transmit(const uint8_t *data, uint8_t length);

/**
 * @brief Receive data
 * @param data Data buffer
 * @param length Pointer to data length
 * @return Error code
 */
klbn_nrf24l01_error_t klbn_nrf24l01_receive(uint8_t *data, uint8_t *length);

/**
 * @brief Check if data is available
 * @return true if data available, false otherwise
 */
bool klbn_nrf24l01_data_available(void);

/**
 * @brief Check if transmission is complete
 * @return true if complete, false otherwise
 */
bool klbn_nrf24l01_tx_complete(void);

/**
 * @brief Get status register
 * @return Status register value
 */
uint8_t klbn_nrf24l01_get_status(void);

/**
 * @brief Clear status flags
 * @param flags Status flags to clear
 */
void klbn_nrf24l01_clear_status(uint8_t flags);

/**
 * @brief Test if NRF24L01 is present
 * @return true if present, false otherwise
 */
bool klbn_nrf24l01_test(void);

#endif /* KLBN_NRF24L01_H */