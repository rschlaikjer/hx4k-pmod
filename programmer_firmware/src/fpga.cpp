#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#include <config.hpp>
#include <fpga.hpp>

void FPGA::init() {
  // Configure the FPGA reset pin as an output, and by default allow the FPGA to
  // run.
  rcc_periph_clock_enable(config::fpga_en_rcc);
  gpio_mode_setup(config::fpga_en_port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
                  config::fpga_en_pin);
  deassert_reset();
}

void FPGA::assert_reset() {
  gpio_clear(config::fpga_en_port, config::fpga_en_pin);
}

void FPGA::deassert_reset() {
  gpio_set(config::fpga_en_port, config::fpga_en_pin);
}
