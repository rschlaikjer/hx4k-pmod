#include <fpga.hpp>
#include <usb_fsm.hpp>

/*
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
*/

typedef void (*opcode_handler)(const uint8_t *, int);

static opcode_handler opcode_table[256];

void UsbFsm::init() {
  // Default all opcode handlers to NOP so we don't do something unexpected on
  // an unimplemented opcode
  for (unsigned i = 0; i < 256; i++) {
    opcode_table[i] = &UsbFsm::handle_nop;
  }

  // FPGA
  opcode_table[static_cast<uint8_t>(UsbFsm::Opcode::FPGA_RESET_ASSERT)] =
      &UsbFsm::handle_fpga_reset_assert;
  opcode_table[static_cast<uint8_t>(UsbFsm::Opcode::FPGA_RESET_DEASSERT)] =
      &UsbFsm::handle_fpga_reset_deassert;
  opcode_table[static_cast<uint8_t>(UsbFsm::Opcode::FPGA_QUERY_STATUS)] =
      &UsbFsm::handle_fpga_query_status;

  // Flash
  opcode_table[static_cast<uint8_t>(UsbFsm::Opcode::FLASH_IDENTIFY)] =
      &UsbFsm::handle_flash_identify;
  opcode_table[static_cast<uint8_t>(UsbFsm::Opcode::FLASH_ERASE_4K)] =
      &UsbFsm::handle_flash_erase_4k;
  opcode_table[static_cast<uint8_t>(UsbFsm::Opcode::FLASH_ERASE_32K)] =
      &UsbFsm::handle_flash_erase_32k;
  opcode_table[static_cast<uint8_t>(UsbFsm::Opcode::FLASH_ERASE_64K)] =
      &UsbFsm::handle_flash_erase_64k;
  opcode_table[static_cast<uint8_t>(UsbFsm::Opcode::FLASH_ERASE_CHIP)] =
      &UsbFsm::handle_flash_erase_chip;
  opcode_table[static_cast<uint8_t>(UsbFsm::Opcode::FLASH_WRITE)] =
      &UsbFsm::handle_flash_write;
  opcode_table[static_cast<uint8_t>(UsbFsm::Opcode::FLASH_READ)] =
      &UsbFsm::handle_flash_read;
  opcode_table[static_cast<uint8_t>(UsbFsm::Opcode::FLASH_QUERY_STATUS)] =
      &UsbFsm::handle_flash_query_status;
}

void UsbFsm::handle_packet(const uint8_t *buf, int len) {
  // Packet must have at least an opcode
  if (len < 1) {
    return;
  }

  // Call the appropriate opcode handler
  opcode_table[buf[0]](&buf[1], len - 1);
}

void UsbFsm::handle_nop(const uint8_t *buf, int len) {
  // Do nothing...
}

void UsbFsm::handle_fpga_reset_assert(const uint8_t *buf, int len) {
  FPGA::assert_reset();
}
void UsbFsm::handle_fpga_reset_deassert(const uint8_t *buf, int len) {
  FPGA::deassert_reset();
}
void UsbFsm::handle_fpga_query_status(const uint8_t *buf, int len) {
  uint8_t resp = 0;

  if (FPGA::is_reset())
    resp |= (1 << 0);

  // Send response
  // TODO
}

void UsbFsm::handle_flash_identify(const uint8_t *buf, int len) {}
void UsbFsm::handle_flash_erase_4k(const uint8_t *buf, int len) {}
void UsbFsm::handle_flash_erase_32k(const uint8_t *buf, int len) {}
void UsbFsm::handle_flash_erase_64k(const uint8_t *buf, int len) {}
void UsbFsm::handle_flash_erase_chip(const uint8_t *buf, int len) {}
void UsbFsm::handle_flash_write(const uint8_t *buf, int len) {}
void UsbFsm::handle_flash_read(const uint8_t *buf, int len) {}
void UsbFsm::handle_flash_query_status(const uint8_t *buf, int len) {}

