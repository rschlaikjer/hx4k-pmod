#include <config.hpp>
#include <flash.hpp>
#include <spi.hpp>

void Flash::init() {
  rcc_periph_clock_enable(config::bitstream_flash_cs_rcc);
  gpio_mode_setup(config::bitstream_flash_cs_port, GPIO_MODE_OUTPUT,
                  GPIO_PUPD_NONE, config::bitstream_flash_cs_pins);
}

void Flash::deinit() {
  gpio_mode_setup(config::bitstream_flash_cs_port, GPIO_MODE_INPUT,
                  GPIO_PUPD_NONE, config::bitstream_flash_cs_pins);
}

void Flash::select() {
  gpio_clear(config::bitstream_flash_cs_port, config::bitstream_flash_cs_pins);
}
void Flash::deselect() {
  gpio_set(config::bitstream_flash_cs_port, config::bitstream_flash_cs_pins);
}

void Flash::reset() {
  uint8_t buffer[2] = {0x66, 0x99};
  select();
  SPI::xfer(buffer, nullptr, sizeof(buffer));
  deselect();
}

void Flash::wakeup() {
  uint8_t buffer[1] = {0xAB};
  select();
  SPI::xfer(buffer, nullptr, sizeof(buffer));
  deselect();
}

void Flash::read_mfgr_and_device_id(uint8_t *mfgr, uint8_t *device) {
  uint8_t buffer[6] = {0, 0, 0, 0, 0, 0};
  buffer[0] = static_cast<uint8_t>(FlashCommand::GET_MFGR_DEVICE_ID);
  select();
  SPI::xfer(buffer, buffer, sizeof(buffer));
  deselect();
  *mfgr = buffer[4];
  *device = buffer[5];
}

void Flash::read_unique_id(uint8_t unique_id_out[8]) {
  uint8_t buffer[5] = {0};
  buffer[0] = static_cast<uint8_t>(FlashCommand::GET_UNIQUE_ID);
  select();
  SPI::xfer(buffer, buffer, sizeof(buffer));
  SPI::xfer(nullptr, unique_id_out, 8);
  deselect();
}

void Flash::write_enable() {
  uint8_t buffer[] = {static_cast<uint8_t>(FlashCommand::WRITE_ENABLE)};
  select();
  SPI::xfer(buffer, buffer, sizeof(buffer));
  deselect();
}

void extract_24bit_addr(uint32_t addr, uint8_t *out) {
  out[0] = (addr >> 16) & 0xFF;
  out[1] = (addr >> 8) & 0xFF;
  out[2] = (addr >> 0) & 0xFF;
}

void Flash::erase_4k(uint32_t addr) {
  write_enable();
  // Send erase command
  uint8_t buffer[4] = {static_cast<uint8_t>(FlashCommand::SECTOR_ERASE_4K)};
  extract_24bit_addr(addr, &buffer[1]);
  select();
  SPI::xfer(buffer, nullptr, sizeof(buffer));
  deselect();
}

void Flash::erase_32k(uint32_t addr) {
  write_enable();
  // Send erase command
  uint8_t buffer[4] = {static_cast<uint8_t>(FlashCommand::BLOCK_ERASE_32K)};
  extract_24bit_addr(addr, &buffer[1]);
  select();
  SPI::xfer(buffer, nullptr, sizeof(buffer));
  deselect();
}

void Flash::erase_64k(uint32_t addr) {
  write_enable();
  // Send erase command
  uint8_t buffer[4] = {static_cast<uint8_t>(FlashCommand::BLOCK_ERASE_64K)};
  extract_24bit_addr(addr, &buffer[1]);
  select();
  SPI::xfer(buffer, nullptr, sizeof(buffer));
  deselect();
}

void Flash::erase_chip() {
  write_enable();
  // Send erase command
  uint8_t buffer[] = {static_cast<uint8_t>(FlashCommand::CHIP_ERASE)};
  select();
  SPI::xfer(buffer, nullptr, sizeof(buffer));
  deselect();
}

void Flash::read_status_register_1(uint16_t *out) {
  uint8_t buffer[3] = {0, 0, 0};
  buffer[0] = static_cast<uint8_t>(FlashCommand::READ_STATUS_REGISTER_1);
  select();
  SPI::xfer(buffer, buffer, sizeof(buffer));
  deselect();
  *out = buffer[1] << 8 | buffer[2];
}

bool Flash::is_busy() {
  uint16_t status_reg_1;
  read_status_register_1(&status_reg_1);
  return status_reg_1 & FLASH_SR1_BUSY;
}

void Flash::write(uint32_t addr, const uint8_t *data, unsigned size) {
  write_enable();
  uint8_t buffer[4] = {static_cast<uint8_t>(FlashCommand::PAGE_PROGRAM)};
  extract_24bit_addr(addr, &buffer[1]);
  select();
  SPI::xfer(buffer, nullptr, sizeof(buffer));
  SPI::xfer(data, nullptr, size);
  deselect();
}

void Flash::read(uint32_t addr, uint8_t *data, unsigned size) {
  // Note this is 5 bytes, need an extra dummy byte
  uint8_t buffer[5] = {static_cast<uint8_t>(FlashCommand::FAST_READ)};
  extract_24bit_addr(addr, &buffer[1]);
  select();
  SPI::xfer(buffer, nullptr, sizeof(buffer));
  SPI::xfer(nullptr, data, size);
  deselect();
}
