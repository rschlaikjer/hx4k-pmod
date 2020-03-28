#include <fpga.hpp>
#include <spi.hpp>
#include <usb_protocol.hpp>

typedef void (*opcode_handler)(const uint8_t *, int);

static opcode_handler opcode_table[256];

void UsbProto::init() {
  // Default all opcode handlers to NOP so we don't do something unexpected on
  // an unimplemented opcode
  for (unsigned i = 0; i < 256; i++) {
    opcode_table[i] = &UsbProto::handle_nop;
  }

  // FPGA
  opcode_table[static_cast<uint8_t>(UsbProto::Opcode::FPGA_RESET_ASSERT)] =
      &UsbProto::handle_fpga_reset_assert;
  opcode_table[static_cast<uint8_t>(UsbProto::Opcode::FPGA_RESET_DEASSERT)] =
      &UsbProto::handle_fpga_reset_deassert;
  opcode_table[static_cast<uint8_t>(UsbProto::Opcode::FPGA_QUERY_STATUS)] =
      &UsbProto::handle_fpga_query_status;

  // Flash
  opcode_table[static_cast<uint8_t>(UsbProto::Opcode::FLASH_IDENTIFY)] =
      &UsbProto::handle_flash_identify;
  opcode_table[static_cast<uint8_t>(UsbProto::Opcode::FLASH_ERASE_4K)] =
      &UsbProto::handle_flash_erase_4k;
  opcode_table[static_cast<uint8_t>(UsbProto::Opcode::FLASH_ERASE_32K)] =
      &UsbProto::handle_flash_erase_32k;
  opcode_table[static_cast<uint8_t>(UsbProto::Opcode::FLASH_ERASE_64K)] =
      &UsbProto::handle_flash_erase_64k;
  opcode_table[static_cast<uint8_t>(UsbProto::Opcode::FLASH_ERASE_CHIP)] =
      &UsbProto::handle_flash_erase_chip;
  opcode_table[static_cast<uint8_t>(UsbProto::Opcode::FLASH_WRITE)] =
      &UsbProto::handle_flash_write;
  opcode_table[static_cast<uint8_t>(UsbProto::Opcode::FLASH_READ)] =
      &UsbProto::handle_flash_read;
  opcode_table[static_cast<uint8_t>(UsbProto::Opcode::FLASH_QUERY_STATUS)] =
      &UsbProto::handle_flash_query_status;
}

void UsbProto::handle_packet(const uint8_t *buf, int len) {
  // Packet must have at least an opcode
  if (len < 1) {
    return;
  }

  // Call the appropriate opcode handler
  opcode_table[buf[0]](&buf[1], len - 1);
}

void UsbProto::handle_nop(const uint8_t *buf, int len) {
  // Do nothing...
}

void UsbProto::handle_fpga_reset_assert(const uint8_t *buf, int len) {
  // Disable the FPGA
  FPGA::assert_reset();
  // Take our hardware SPI out of tri-state so we can talk to the flash chip
  SPI::init();
}

void UsbProto::handle_fpga_reset_deassert(const uint8_t *buf, int len) {
  // Put our SPI back in high-Z mode
  SPI::deinit();
  // Allow the FPGA to run again
  FPGA::deassert_reset();
}
void UsbProto::handle_fpga_query_status(const uint8_t *buf, int len) {
  uint8_t resp = 0;

  if (FPGA::is_reset())
    resp |= (1 << 0);

  // Send response
  // TODO
}

void UsbProto::handle_flash_identify(const uint8_t *buf, int len) {}
void UsbProto::handle_flash_erase_4k(const uint8_t *buf, int len) {}
void UsbProto::handle_flash_erase_32k(const uint8_t *buf, int len) {}
void UsbProto::handle_flash_erase_64k(const uint8_t *buf, int len) {}
void UsbProto::handle_flash_erase_chip(const uint8_t *buf, int len) {}
void UsbProto::handle_flash_write(const uint8_t *buf, int len) {}
void UsbProto::handle_flash_read(const uint8_t *buf, int len) {}
void UsbProto::handle_flash_query_status(const uint8_t *buf, int len) {}

