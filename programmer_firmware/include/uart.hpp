#pragma once

namespace UART {
void init();
void send(const uint8_t *buf, unsigned count);
unsigned receive(uint8_t *buf, unsigned count);
bool available();
} // namespace UART
