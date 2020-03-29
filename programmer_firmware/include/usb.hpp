#pragma once

namespace USB {
void init();
void poll();
void transmit_programming_packet(uint8_t *data, unsigned len);
} // namespace USB
