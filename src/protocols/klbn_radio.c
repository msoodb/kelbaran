/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Kelbaran.
 *
 * Kelbaran is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "klbn_radio.h"
#include "klbn_nrf24l01.h"
#include "libc_stubs.h"

// Static variables
static klbn_radio_state_t current_state = KLBN_RADIO_STATE_IDLE;
static klbn_radio_mode_t current_mode = KLBN_RADIO_MODE_RX;
static uint32_t message_counter = 0;

void klbn_radio_init(void) {
    // Initialize NRF24L01
    klbn_nrf24l01_init();
    
    // Check if radio is present
    if (!klbn_nrf24l01_is_present()) {
        current_state = KLBN_RADIO_STATE_ERROR;
        return;
    }
    
    // Set initial mode to RX
    current_mode = KLBN_RADIO_MODE_RX;
    klbn_nrf24l01_set_mode(current_mode);
    current_state = KLBN_RADIO_STATE_RX_MODE;
}

void klbn_radio_switch_mode(void) {
    if (current_state == KLBN_RADIO_STATE_ERROR) {
        return;
    }
    
    // Toggle between TX and RX modes
    if (current_mode == KLBN_RADIO_MODE_RX) {
        current_mode = KLBN_RADIO_MODE_TX;
        current_state = KLBN_RADIO_STATE_TX_MODE;
    } else {
        current_mode = KLBN_RADIO_MODE_RX;
        current_state = KLBN_RADIO_STATE_RX_MODE;
    }
    
    // Set the new mode on the radio
    klbn_nrf24l01_set_mode(current_mode);
}

klbn_radio_state_t klbn_radio_get_current_state(void) {
    return current_state;
}

bool klbn_radio_send_data(const uint8_t *data, uint8_t length) {
    if (current_state == KLBN_RADIO_STATE_ERROR || !data || length == 0) {
        return false;
    }
    
    // Ensure we're in TX mode
    if (current_mode != KLBN_RADIO_MODE_TX) {
        current_mode = KLBN_RADIO_MODE_TX;
        klbn_nrf24l01_set_mode(current_mode);
        current_state = KLBN_RADIO_STATE_TX_MODE;
    }
    
    // Prepare packet
    klbn_radio_packet_t packet;
    packet.length = (length > NRF24_MAX_PAYLOAD_SIZE) ? NRF24_MAX_PAYLOAD_SIZE : length;
    memcpy(packet.data, data, packet.length);
    
    // Add message counter to first 4 bytes if there's space
    if (packet.length >= 4) {
        packet.data[0] = (message_counter >> 24) & 0xFF;
        packet.data[1] = (message_counter >> 16) & 0xFF;
        packet.data[2] = (message_counter >> 8) & 0xFF;
        packet.data[3] = message_counter & 0xFF;
        message_counter++;
    }
    
    // Transmit
    current_state = KLBN_RADIO_STATE_TRANSMITTING;
    bool success = klbn_nrf24l01_transmit(&packet);
    
    if (success) {
        current_state = KLBN_RADIO_STATE_TX_MODE;
    } else {
        current_state = KLBN_RADIO_STATE_ERROR;
    }
    
    return success;
}

bool klbn_radio_is_data_available(void) {
    if (current_state == KLBN_RADIO_STATE_ERROR) {
        return false;
    }
    
    // Ensure we're in RX mode
    if (current_mode != KLBN_RADIO_MODE_RX) {
        current_mode = KLBN_RADIO_MODE_RX;
        klbn_nrf24l01_set_mode(current_mode);
        current_state = KLBN_RADIO_STATE_RX_MODE;
    }
    
    klbn_radio_status_t status = klbn_nrf24l01_get_status();
    return (status == KLBN_RADIO_STATUS_RX_READY);
}

bool klbn_radio_receive_data(uint8_t *data, uint8_t *length) {
    if (current_state == KLBN_RADIO_STATE_ERROR || !data || !length) {
        return false;
    }
    
    // Ensure we're in RX mode
    if (current_mode != KLBN_RADIO_MODE_RX) {
        current_mode = KLBN_RADIO_MODE_RX;
        klbn_nrf24l01_set_mode(current_mode);
        current_state = KLBN_RADIO_STATE_RX_MODE;
    }
    
    klbn_radio_packet_t packet;
    current_state = KLBN_RADIO_STATE_LISTENING;
    
    bool success = klbn_nrf24l01_receive(&packet);
    
    if (success) {
        *length = packet.length;
        memcpy(data, packet.data, packet.length);
        current_state = KLBN_RADIO_STATE_RX_MODE;
    } else {
        current_state = KLBN_RADIO_STATE_RX_MODE;
    }
    
    return success;
}

const char* klbn_radio_get_mode_string(void) {
    switch (current_mode) {
        case KLBN_RADIO_MODE_TX:
            return "TX";
        case KLBN_RADIO_MODE_RX:
            return "RX";
        default:
            return "ERROR";
    }
}