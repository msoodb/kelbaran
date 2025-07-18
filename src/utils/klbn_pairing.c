/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Kelbaran.
 *
 * Kelbaran is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "klbn_pairing.h"
#include "klbn_nrf24l01_module.h"
#include "klbn_nrf24l01.h"
#include "klbn_gpio.h"
#include "klbn_pins.h"
#include "FreeRTOS.h"
#include "task.h"
#include "libc_stubs.h"

#define PAIRING_TIMEOUT_MS 30000  // 30 seconds
#define PAIRING_RETRY_MS 1000     // 1 second
#define PAIRING_MAGIC 0xABCD      // Magic number for pairing messages

static klbn_pair_state_t current_state = KLBN_PAIR_STATE_NORMAL;
static uint32_t pairing_start_time = 0;
static uint32_t last_retry_time = 0;
static uint8_t my_board_id[4];
static uint8_t my_address[5];
static uint8_t paired_address[5];
static bool is_paired = false;

// Default pairing address (all boards listen on this during pairing)
static const uint8_t pairing_broadcast_addr[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

/**
 * @brief Generate unique board ID
 */
static void generate_board_id(void) {
    // Use STM32 unique ID as seed
    uint32_t *uid = (uint32_t*)0x1FFFF7E8;  // STM32F103 unique ID register
    
    my_board_id[0] = (uid[0] >> 0) & 0xFF;
    my_board_id[1] = (uid[0] >> 8) & 0xFF;
    my_board_id[2] = (uid[1] >> 0) & 0xFF;
    my_board_id[3] = (uid[1] >> 8) & 0xFF;
    
    // Generate unique address from board ID
    my_address[0] = 0x01;  // Fixed prefix
    my_address[1] = my_board_id[0];
    my_address[2] = my_board_id[1];
    my_address[3] = my_board_id[2];
    my_address[4] = my_board_id[3];
}

/**
 * @brief Send pairing message
 */
static void send_pairing_message(klbn_pair_msg_type_t type, const uint8_t *target_addr) {
    klbn_pair_message_t msg;
    klbn_radio_command_t cmd;
    
    // Setup message
    msg.type = type;
    memcpy(msg.board_id, my_board_id, 4);
    memcpy(msg.address, my_address, 5);
    
    // Setup radio command
    cmd.data[0] = (PAIRING_MAGIC >> 8) & 0xFF;  // Magic number high
    cmd.data[1] = PAIRING_MAGIC & 0xFF;         // Magic number low
    memcpy(&cmd.data[2], &msg, sizeof(msg));
    cmd.length = 2 + sizeof(msg);
    
    // Set target address and send
    if (target_addr) {
        klbn_nrf24l01_set_tx_address(target_addr);
    } else {
        klbn_nrf24l01_set_tx_address(pairing_broadcast_addr);
    }
    
    klbn_nrf24l01_module_apply(&cmd);
}

/**
 * @brief Update LED pattern based on pairing state
 */
static void update_pairing_leds(void) {
    static uint32_t last_blink = 0;
    static bool blink_state = false;
    uint32_t now = xTaskGetTickCount();
    
    switch (current_state) {
        case KLBN_PAIR_STATE_WAITING:
            // Slow blink debug LED
            if (now - last_blink > pdMS_TO_TICKS(500)) {
                blink_state = !blink_state;
                if (blink_state) {
                    klbn_gpio_set_pin((uint32_t)KLBN_LED_DEBUG_PORT, KLBN_LED_DEBUG_PIN);
                } else {
                    klbn_gpio_clear_pin((uint32_t)KLBN_LED_DEBUG_PORT, KLBN_LED_DEBUG_PIN);
                }
                last_blink = now;
            }
            break;
            
        case KLBN_PAIR_STATE_MASTER:
        case KLBN_PAIR_STATE_SLAVE:
            // Fast blink debug LED
            if (now - last_blink > pdMS_TO_TICKS(200)) {
                blink_state = !blink_state;
                if (blink_state) {
                    klbn_gpio_set_pin((uint32_t)KLBN_LED_DEBUG_PORT, KLBN_LED_DEBUG_PIN);
                } else {
                    klbn_gpio_clear_pin((uint32_t)KLBN_LED_DEBUG_PORT, KLBN_LED_DEBUG_PIN);
                }
                last_blink = now;
            }
            break;
            
        case KLBN_PAIR_STATE_PAIRED:
            // Solid on debug LED
            klbn_gpio_set_pin((uint32_t)KLBN_LED_DEBUG_PORT, KLBN_LED_DEBUG_PIN);
            break;
            
        default:
            // Debug LED off
            klbn_gpio_clear_pin((uint32_t)KLBN_LED_DEBUG_PORT, KLBN_LED_DEBUG_PIN);
            break;
    }
}

void klbn_pairing_init(void) {
    generate_board_id();
    current_state = KLBN_PAIR_STATE_NORMAL;
    is_paired = false;
}

void klbn_pairing_start(void) {
    if (current_state != KLBN_PAIR_STATE_NORMAL) {
        return;  // Already in pairing process
    }
    
    // Test if NRF24L01 is responding
    if (!klbn_nrf24l01_test()) {
        // NRF24L01 not responding, show error with very fast blink
        for (int i = 0; i < 10; i++) {
            klbn_gpio_set_pin((uint32_t)KLBN_LED_DEBUG_PORT, KLBN_LED_DEBUG_PIN);
            vTaskDelay(pdMS_TO_TICKS(50));
            klbn_gpio_clear_pin((uint32_t)KLBN_LED_DEBUG_PORT, KLBN_LED_DEBUG_PIN);
            vTaskDelay(pdMS_TO_TICKS(50));
        }
        return;
    }
    
    current_state = KLBN_PAIR_STATE_WAITING;
    pairing_start_time = xTaskGetTickCount();
    last_retry_time = 0;
    
    // Set radio to listen on pairing broadcast address
    klbn_nrf24l01_set_rx_address(pairing_broadcast_addr);
    klbn_nrf24l01_set_mode(KLBN_NRF24L01_MODE_RX);
}

void klbn_pairing_tick(void) {
    uint32_t now = xTaskGetTickCount();
    
    // Update LED patterns
    update_pairing_leds();
    
    // Check timeout
    if (current_state != KLBN_PAIR_STATE_NORMAL && current_state != KLBN_PAIR_STATE_PAIRED) {
        if (now - pairing_start_time > pdMS_TO_TICKS(PAIRING_TIMEOUT_MS)) {
            klbn_pairing_reset();
            return;
        }
    }
    
    // State machine
    switch (current_state) {
        case KLBN_PAIR_STATE_WAITING:
            // After 2 seconds, become master and start broadcasting
            if (now - pairing_start_time > pdMS_TO_TICKS(2000)) {
                current_state = KLBN_PAIR_STATE_MASTER;
                send_pairing_message(KLBN_PAIR_MSG_REQUEST, NULL);
                last_retry_time = now;
            }
            break;
            
        case KLBN_PAIR_STATE_MASTER:
            // Retry broadcasting every second
            if (now - last_retry_time > pdMS_TO_TICKS(PAIRING_RETRY_MS)) {
                send_pairing_message(KLBN_PAIR_MSG_REQUEST, NULL);
                last_retry_time = now;
            }
            break;
            
        case KLBN_PAIR_STATE_SLAVE:
            // Slave waits for master confirmation
            break;
            
        default:
            break;
    }
}

bool klbn_pairing_process_message(const klbn_radio_data_t *data) {
    if (!data || data->length < 2 + sizeof(klbn_pair_message_t)) {
        return false;
    }
    
    // Check magic number
    uint16_t magic = (data->data[0] << 8) | data->data[1];
    if (magic != PAIRING_MAGIC) {
        return false;
    }
    
    // Extract pairing message
    klbn_pair_message_t msg;
    memcpy(&msg, &data->data[2], sizeof(msg));
    
    // Don't process our own messages
    bool same_board = true;
    for (int i = 0; i < 4; i++) {
        if (msg.board_id[i] != my_board_id[i]) {
            same_board = false;
            break;
        }
    }
    if (same_board) {
        return true;
    }
    
    switch (current_state) {
        case KLBN_PAIR_STATE_WAITING:
            if (msg.type == KLBN_PAIR_MSG_REQUEST) {
                // Another board is master, we become slave
                current_state = KLBN_PAIR_STATE_SLAVE;
                memcpy(paired_address, msg.address, 5);
                
                // Send accept message to master
                send_pairing_message(KLBN_PAIR_MSG_ACCEPT, paired_address);
            }
            break;
            
        case KLBN_PAIR_STATE_MASTER:
            if (msg.type == KLBN_PAIR_MSG_ACCEPT) {
                // Slave accepted, send confirmation
                memcpy(paired_address, msg.address, 5);
                send_pairing_message(KLBN_PAIR_MSG_CONFIRM, paired_address);
                
                // Complete pairing
                current_state = KLBN_PAIR_STATE_PAIRED;
                is_paired = true;
                
                // Set radio to communicate with paired board
                klbn_nrf24l01_set_tx_address(paired_address);
                klbn_nrf24l01_set_rx_address(my_address);
                klbn_nrf24l01_set_mode(KLBN_NRF24L01_MODE_RX);
            }
            break;
            
        case KLBN_PAIR_STATE_SLAVE:
            if (msg.type == KLBN_PAIR_MSG_CONFIRM) {
                // Master confirmed, complete pairing
                current_state = KLBN_PAIR_STATE_PAIRED;
                is_paired = true;
                
                // Set radio to communicate with paired board
                klbn_nrf24l01_set_tx_address(paired_address);
                klbn_nrf24l01_set_rx_address(my_address);
                klbn_nrf24l01_set_mode(KLBN_NRF24L01_MODE_RX);
            }
            break;
            
        default:
            break;
    }
    
    return true;
}

klbn_pair_state_t klbn_pairing_get_state(void) {
    return current_state;
}

bool klbn_pairing_is_paired(void) {
    return is_paired;
}

bool klbn_pairing_get_paired_address(uint8_t *address) {
    if (!address || !is_paired) {
        return false;
    }
    
    memcpy(address, paired_address, 5);
    return true;
}

void klbn_pairing_reset(void) {
    current_state = KLBN_PAIR_STATE_NORMAL;
    is_paired = false;
    
    // Reset radio to default broadcast address
    const uint8_t default_addr[5] = {0x01, 0x02, 0x03, 0x04, 0x05};
    klbn_nrf24l01_set_tx_address(default_addr);
    klbn_nrf24l01_set_rx_address(default_addr);
    klbn_nrf24l01_set_mode(KLBN_NRF24L01_MODE_RX);
    
    // Turn off debug LED
    klbn_gpio_clear_pin((uint32_t)KLBN_LED_DEBUG_PORT, KLBN_LED_DEBUG_PIN);
}