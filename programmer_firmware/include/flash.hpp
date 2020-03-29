#pragma once

#include <stdint.h>

namespace Flash {

enum class FlashCommand : uint8_t {
  WRITE_ENABLE = 0x06,
  WRITE_DISABLE = 0x04,
  READ_STATUS_REGISTER_1 = 0x05,
  READ_STATUS_REGISTER_2 = 0x35,
  WRITE_STATUS_REGISTER = 0x01,
  PAGE_PROGRAM = 0x02,
  SECTOR_ERASE_4K = 0x20,
  BLOCK_ERASE_32K = 0x52,
  BLOCK_ERASE_64K = 0xD8,
  CHIP_ERASE = 0xC7,
  READ_DATA = 0x03,
  FAST_READ = 0x0B,
  GET_MFGR_DEVICE_ID = 0x90,
  GET_JEDEC_ID = 0x9F,
  GET_UNIQUE_ID = 0x4B,
};

static const uint16_t FLASH_SR1_BUSY = (1 << 0);

// IO asserting/deasserting
void init();
void deinit();

// CS control
void select();
void deselect();

// General flash commands
void reset();
void wakeup();
void read_mfgr_and_device_id(uint8_t *mfgr, uint8_t *device);
void read_unique_id(uint8_t unique_id_out[8]);
void read_status_register_1(uint16_t *out);
bool is_busy();

// Erasing
void write_enable();
void erase_4k(uint32_t addr);
void erase_32k(uint32_t addr);
void erase_64k(uint32_t addr);
void erase_chip();

} // namespace Flash
