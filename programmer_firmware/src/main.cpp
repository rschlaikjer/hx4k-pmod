#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/iwdg.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/syscfg.h>

#include <config.hpp>

#include <fpga.hpp>
#include <rgb.hpp>
#include <uart.hpp>
#include <usb.hpp>
#include <usb_protocol.hpp>

void clock_setup();
void event_loop();
int main() {
  // Enable watchdog
  iwdg_set_period_ms(50);
  iwdg_reset();
  iwdg_start();

  // Move from default clock source to 48MHz PLL
  clock_setup();

  // Initialize always-on io/peripherals
  FPGA::init();
  UART::init();
  RGB::init();
  UsbProto::init();
  USB::init();

  // Run event loop forever
  while (true) {
    event_loop();
  }
}

void clock_setup() {
  //// Set clock at 48MHz from shared 12MHz oscillator
  // First need to enable the external oscillator, and switch to it
  // as our system clock source
  rcc_osc_on(RCC_HSE);
  rcc_wait_for_osc_ready(RCC_HSE);
  rcc_set_sysclk_source(RCC_HSE);

  // No AHB / APB prescaler, since they can run at 48MHz
  rcc_set_hpre(RCC_CFGR_HPRE_NODIV);
  rcc_set_ppre(RCC_CFGR_PPRE_NODIV);

  // Need to enable flash prefetching for 48MHz operation
  flash_prefetch_enable();
  flash_set_ws(FLASH_ACR_LATENCY_024_048MHZ);

  // PLL config: 12MHz * 4 = 48MHz
  rcc_set_pll_multiplication_factor(RCC_CFGR_PLLMUL_MUL4);
  rcc_set_pll_source(RCC_CFGR_PLLSRC_HSE_CLK);
  rcc_set_pllxtpre(RCC_CFGR_PLLXTPRE_HSE_CLK);

  // Enable the PLL, wait for it to lock and change over to it as
  // the system clock source
  rcc_osc_on(RCC_PLL);
  rcc_wait_for_osc_ready(RCC_PLL);
  rcc_set_sysclk_source(RCC_PLL);

  // These globals are used in opencm3, need to make sure we set them to the
  // correct system clock speed
  rcc_apb1_frequency = 48'000'000;
  rcc_ahb_frequency = 48'000'000;
}

void event_loop() {
  // Poll for new USB events
  USB::poll();

  // Tickle watchdog
  iwdg_reset();
}
