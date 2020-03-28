#pragma once

#include <stdint.h>

namespace SPI {
void init();
void deinit();

void xfer(uint8_t send, uint8_t *recv);
void xfer(const uint8_t *send, uint8_t *recv, unsigned nmeb);
void send(uint8_t send);
void recv(uint8_t *in);
void wait_for_tx_complete();
} // namespace SPI
