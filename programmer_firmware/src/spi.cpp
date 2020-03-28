
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>

#include <config.hpp>
#include <spi.hpp>

void SPI::init() {
  // Enable SPI & GPIO clocks
  rcc_periph_clock_enable(config::bitstream_flash_gpio_rcc);
  rcc_periph_clock_enable(config::bitstream_flash_spi_rcc);

  // Configure IOs
  gpio_mode_setup(config::bitstream_flash_spi_port, GPIO_MODE_AF,
                  GPIO_PUPD_NONE, config::bitstream_flash_spi_pins);
  gpio_set_af(config::bitstream_flash_spi_port, config::bitstream_flash_spi_af,
              config::bitstream_flash_spi_pins);

  // Main spi config
  spi_reset(config::bitstream_flash_spi);
  spi_init_master(config::bitstream_flash_spi, SPI_CR1_BAUDRATE_FPCLK_DIV_16,
                  SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
                  SPI_CR1_CPHA_CLK_TRANSITION_1, SPI_CR1_MSBFIRST);

  // Software slave management
  spi_enable_software_slave_management(config::bitstream_flash_spi);
  spi_set_nss_high(config::bitstream_flash_spi);

  // Data transfers are bidirectional
  spi_set_unidirectional_mode(config::bitstream_flash_spi);

  // Data is 8 bits
  spi_set_data_size(config::bitstream_flash_spi, SPI_CR2_DS_8BIT);
  spi_fifo_reception_threshold_8bit(config::bitstream_flash_spi);

  // Enable periph
  spi_enable(config::bitstream_flash_spi);
}

void SPI::deinit() {
  spi_disable(config::bitstream_flash_spi);
  gpio_mode_setup(config::bitstream_flash_spi_port, GPIO_MODE_INPUT,
                  GPIO_PUPD_NONE, config::bitstream_flash_spi_pins);
}

void SPI::xfer(uint8_t send, uint8_t *recv) {
  // Wait for TX available
  while (!(SPI_SR(config::bitstream_flash_spi) & SPI_SR_TXE)) {
  }

  // Set the data to send
  SPI_DR8(config::bitstream_flash_spi) = send;

  // Wait for RX complete
  while (!(SPI_SR(config::bitstream_flash_spi) & SPI_SR_RXNE)) {
  }

  // Read the response data
  *recv = SPI_DR8(config::bitstream_flash_spi);
}

void SPI::send(uint8_t send) {
  uint8_t dummy;
  SPI::xfer(send, &dummy);
}

void SPI::recv(uint8_t *in) { SPI::xfer(0, in); }

void SPI::xfer(const uint8_t *send, uint8_t *recv, unsigned nmeb) {
  // Send all data in the send buffer, receive data into the recv buffer
  // If send/recv is null, this is unidirectional - either send zeros, or
  // throw away received data
  uint8_t dummy = 0x0;
  for (unsigned i = 0; i < nmeb; i++) {
    if (send == nullptr) {
      xfer(dummy, &recv[i]);
    } else if (recv == nullptr) {
      xfer(send[i], &dummy);
    } else {
      xfer(send[i], &recv[i]);
    }
  }
}

void SPI::wait_for_tx_complete() {
  while (SPI_SR(config::bitstream_flash_spi) & SPI_SR_BSY) {
  }
}
