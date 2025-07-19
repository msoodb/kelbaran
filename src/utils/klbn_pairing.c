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
#include "klbn_types.h"
#include "FreeRTOS.h"
#include "task.h"
#include "libc_stubs.h"

#define PAIRING_TIMEOUT_MS 30000  // 30 seconds
#define PAIRING_RETRY_MS 1000     // 1 second
#define PAIRING_MAGIC 0xABCD      // Magic number for pairing messages

static klbn_pair_state_t current_state = KLBN_PAIR_STATE_NORMAL;
static uint8_t my_board_id[4];
static uint8_t my_address[5];
static uint8_t paired_address[5];
static bool is_paired = false;

// Hardcoded addresses for testing
static const uint8_t board_addresses[][5] = {
    {0x01, 0x02, 0x03, 0x04, 0x05},  // Board 1 address
    {0x06, 0x07, 0x08, 0x09, 0x0A},  // Board 2 address
};

#define NUM_KNOWN_BOARDS (sizeof(board_addresses) / sizeof(board_addresses[0]))

/**
 * @brief Generate unique board ID and assign hardcoded address
 */
static void generate_board_id(void) {
    // Use STM32 unique ID as seed
    uint32_t *uid = (uint32_t*)0x1FFFF7E8;  // STM32F103 unique ID register
    
    my_board_id[0] = (uid[0] >> 0) & 0xFF;
    my_board_id[1] = (uid[0] >> 8) & 0xFF;
    my_board_id[2] = (uid[1] >> 0) & 0xFF;
    my_board_id[3] = (uid[1] >> 8) & 0xFF;
    
    // Determine which board this is based on unique ID
    uint32_t board_hash = uid[0] ^ uid[1];
    uint8_t board_index = board_hash % NUM_KNOWN_BOARDS;
    
    // Use hardcoded address for this board
    memcpy(my_address, board_addresses[board_index], 5);
    
    // Set paired address to the other board
    uint8_t other_board = (board_index + 1) % NUM_KNOWN_BOARDS;
    memcpy(paired_address, board_addresses[other_board], 5);
}

// Send pairing message function removed for hardcoded pairing

// LED update function removed for hardcoded pairing

void klbn_pairing_init(void) {
    generate_board_id();
    current_state = KLBN_PAIR_STATE_PAIRED;  // Always paired
    is_paired = true;  // Always paired
    
    // Set up radio immediately
    klbn_nrf24l01_set_tx_address(paired_address);
    klbn_nrf24l01_set_rx_address(my_address);
    klbn_nrf24l01_set_mode(KLBN_NRF24L01_MODE_RX);
}

void klbn_pairing_start(void) {
    // Do nothing - always paired
}

void klbn_pairing_tick(void) {
    // For hardcoded pairing, just maintain LED state
    if (current_state == KLBN_PAIR_STATE_PAIRED) {
        // Keep LED solid on when paired
        klbn_gpio_set_pin((uint32_t)KLBN_LED_DEBUG_PORT, KLBN_LED_DEBUG_PIN);
    }
}

bool klbn_pairing_process_message(const klbn_radio_data_t *data) {
    // For hardcoded pairing, no message processing needed
    (void)data;
    return false;
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
    
    // Reset radio to my own address
    klbn_nrf24l01_set_tx_address(my_address);
    klbn_nrf24l01_set_rx_address(my_address);
    klbn_nrf24l01_set_mode(KLBN_NRF24L01_MODE_RX);
    
    // Turn off debug LED
    klbn_gpio_clear_pin((uint32_t)KLBN_LED_DEBUG_PORT, KLBN_LED_DEBUG_PIN);
}