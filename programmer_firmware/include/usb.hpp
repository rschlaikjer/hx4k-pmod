#pragma once

namespace USB {
void init();
void poll();
bool endpoint_ready_to_send(uint8_t endpoint);
void transmit_programming_packet(uint8_t *data, unsigned len);
bool can_transmit_serial_packet();
void transmit_serial_packet(uint8_t *data, unsigned len);
} // namespace USB
