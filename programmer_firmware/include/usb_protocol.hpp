#pragma once

#include <stdint.h>

namespace UsbProto {

enum class Opcode : uint8_t {
  NOP = 0x0,
  // FPGA interface
  FPGA_RESET_ASSERT = 0x01,
  FPGA_RESET_DEASSERT = 0x02,
  FPGA_QUERY_STATUS = 0x03,
  // Flash interface
  FLASH_IDENTIFY = 0x10,
  FLASH_ERASE_4K = 0x11,
  FLASH_ERASE_32K = 0x12,
  FLASH_ERASE_64K = 0x13,
  FLASH_ERASE_CHIP = 0x14,
  FLASH_WRITE = 0x15,
  FLASH_READ = 0x16,
  FLASH_QUERY_STATUS = 0x17,
};

// Initializes our opcode jump table
void init();

// Entry point for packets coming in off usb
void handle_packet(const uint8_t *buf, int len);
void handle_nop(const uint8_t *buf, int len);

// FPGA
void handle_fpga_reset_assert(const uint8_t *buf, int len);
void handle_fpga_reset_deassert(const uint8_t *buf, int len);
void handle_fpga_query_status(const uint8_t *buf, int len);

// Flash
void handle_flash_identify(const uint8_t *buf, int len);
void handle_flash_erase_4k(const uint8_t *buf, int len);
void handle_flash_erase_32k(const uint8_t *buf, int len);
void handle_flash_erase_64k(const uint8_t *buf, int len);
void handle_flash_erase_chip(const uint8_t *buf, int len);
void handle_flash_write(const uint8_t *buf, int len);
void handle_flash_read(const uint8_t *buf, int len);
void handle_flash_query_status(const uint8_t *buf, int len);

} // namespace UsbProto
