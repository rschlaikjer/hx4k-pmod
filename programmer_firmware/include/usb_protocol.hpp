#pragma once

#include <stdint.h>

namespace UsbProto {

enum class Opcode : uint8_t {
  // General
  NOP = 0x00,
  SET_RGB_LED = 0x01,
  // FPGA interface
  FPGA_RESET_ASSERT = 0x10,
  FPGA_RESET_DEASSERT = 0x11,
  FPGA_QUERY_STATUS = 0x012,
  // Flash interface
  FLASH_IDENTIFY = 0x20,
  FLASH_ERASE_4K = 0x21,
  FLASH_ERASE_32K = 0x22,
  FLASH_ERASE_64K = 0x23,
  FLASH_ERASE_CHIP = 0x24,
  FLASH_WRITE = 0x25,
  FLASH_READ = 0x26,
  FLASH_QUERY_STATUS = 0x27,
};

// Initializes our opcode jump table
void init();

// Entry point for packets coming in off usb
void handle_packet(const uint8_t *buf, int len);

// Generic
void handle_nop(const uint8_t *buf, int len);
void handle_set_rgb_led(const uint8_t *buf, int len);

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
