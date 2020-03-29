#include <flash.hpp>
#include <fpga.hpp>
#include <rgb.hpp>
#include <spi.hpp>
#include <usb.hpp>
#include <usb_protocol.hpp>

typedef void (*opcode_handler)(const uint8_t *, int);

static opcode_handler opcode_table[256];

void UsbProto::init() {
  // Default all opcode handlers to NOP so we don't do something unexpected on
  // an unimplemented opcode
  for (unsigned i = 0; i < 256; i++) {
    opcode_table[i] = &UsbProto::handle_nop;
  }

  // General
  opcode_table[static_cast<uint8_t>(UsbProto::Opcode::SET_RGB_LED)] =
      &UsbProto::handle_set_rgb_led;

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
    RGB::set_colour(128, 0, 0);
    return;
  }

  // Call the appropriate opcode handler
  opcode_table[buf[0]](&buf[1], len - 1);
}

void UsbProto::handle_set_rgb_led(const uint8_t *buf, int len) {
  // Need at least three bytes - R, G, B channels
  if (len < 3) {
    RGB::set_colour(128, 0, 0);
    return;
  }
  RGB::set_colour(buf[0], buf[1], buf[2]);
}

void UsbProto::handle_nop(const uint8_t *buf, int len) {
  // Do nothing...
}

void UsbProto::handle_fpga_reset_assert(const uint8_t *buf, int len) {
  // Disable the FPGA
  FPGA::assert_reset();
  // Take our hardware SPI out of tri-state so we can talk to the flash chip
  SPI::init();
  // Ensure the flash is in a sane state
  Flash::init();
  Flash::reset();
  Flash::wakeup();
}

void UsbProto::handle_fpga_reset_deassert(const uint8_t *buf, int len) {
  // Put our SPI back in high-Z mode
  Flash::deinit();
  SPI::deinit();
  // Allow the FPGA to run again
  FPGA::deassert_reset();
}

void UsbProto::handle_fpga_query_status(const uint8_t *buf, int len) {
  uint8_t status = 0;

  if (FPGA::is_reset())
    status |= (1 << 0);

  // Send response
  uint8_t resp[] = {status};
  USB::transmit_programming_packet(resp, sizeof(resp));
}

void UsbProto::handle_flash_identify(const uint8_t *buf, int len) {
  // Perform the SPI transaction to get the chip IDs
  uint8_t resp[10];
  // First two bytes: mfgr id, device ID
  Flash::read_mfgr_and_device_id(&resp[0], &resp[1]);
  // Next 8 bytes: unique chip ID
  Flash::read_unique_id(&resp[2]);

  // Send response
  USB::transmit_programming_packet(resp, sizeof(resp));
}

void UsbProto::handle_flash_erase_4k(const uint8_t *buf, int len) {
  // Need to be able to read another 4 bytes for the sector address
  if (len < 4) {
    RGB::set_colour(128, 0, 0);
    return;
  }

  // Get that sector address
  uint32_t sector_addr = (buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3]);

  // Mask off the lower 4k
  sector_addr &= ~(0xFFF);

  // Ask the flash to wipe it
  Flash::erase_4k(sector_addr);
}

void UsbProto::handle_flash_erase_32k(const uint8_t *buf, int len) {
  // Need to be able to read another 4 bytes for the sector address
  if (len < 4) {
    RGB::set_colour(128, 0, 0);
    return;
  }

  // Get that sector address
  uint32_t sector_addr = (buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3]);

  // Mask off the lower 32k
  sector_addr &= ~(0x7FFF);

  // Ask the flash to wipe it
  Flash::erase_32k(sector_addr);
}

void UsbProto::handle_flash_erase_64k(const uint8_t *buf, int len) {
  // Need to be able to read another 4 bytes for the sector address
  if (len < 4) {
    RGB::set_colour(128, 0, 0);
    return;
  }

  // Get that sector address
  uint32_t sector_addr = (buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3]);

  // Mask off the lower 32k
  sector_addr &= ~(0xFFFF);

  // Ask the flash to wipe it
  Flash::erase_64k(sector_addr);
}

void UsbProto::handle_flash_erase_chip(const uint8_t *buf, int len) {
  Flash::erase_chip();
}

void UsbProto::handle_flash_write(const uint8_t *buf, int len) {
  // Need to be able to read another
  //  - 4 bytes for the base addr
  //  - 1 byte for the amount to write
  if (len < 5) {
    RGB::set_colour(128, 0, 0);
    return;
  }

  // Decode args
  uint32_t addr = (buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3]);
  uint8_t write_size = buf[4];

  // If the size to write is more than the data that was received, can't do it
  if (write_size > (len - 5)) {
    RGB::set_colour(128, 0, 0);
    return;
  }

  // Perform the write
  Flash::write(addr, &buf[5], write_size);
}

void UsbProto::handle_flash_read(const uint8_t *buf, int len) {
  // Need to be able to read another
  //  - 4 bytes for the base addr
  //  - 1 byte for the amount to read
  if (len < 5) {
    RGB::set_colour(128, 0, 0);
    return;
  }

  // Decode args
  uint32_t addr = (buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3]);
  uint8_t read_size = buf[4];

  // If the read size is bigger than a usb FS packet, can't do it
  if (read_size > 64) {
    RGB::set_colour(128, 0, 0);
    return;
  }

  // Read into a reply buffer
  uint8_t resp_buf[64];
  Flash::read(addr, resp_buf, read_size);
  USB::transmit_programming_packet(resp_buf, read_size);
}

void UsbProto::handle_flash_query_status(const uint8_t *buf, int len) {
  uint8_t status = 0;

  // Is the flash still erasing / writing?
  if (Flash::is_busy())
    status |= (1 << 0);

  // Send response
  uint8_t resp[] = {status};
  USB::transmit_programming_packet(resp, sizeof(resp));
}

