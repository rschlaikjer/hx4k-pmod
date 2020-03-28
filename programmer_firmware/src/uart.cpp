#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>

#include <config.hpp>
#include <uart.hpp>

void UART::init() {
  // Enable clock
  rcc_periph_clock_enable(config::fpga_uart_gpio_rcc);
  rcc_periph_clock_enable(config::fpga_uart_rcc);

  // Setup USART pins as alternate function
  gpio_mode_setup(config::fpga_uart_port, GPIO_MODE_AF, GPIO_PUPD_NONE,
                  config::fpga_uart_pins);
  gpio_set_af(config::fpga_uart_port, config::fpga_uart_af,
              config::fpga_uart_pins);

  // Setup USART parameters
  usart_set_baudrate(config::fpga_uart, config::fpga_uart_baudrate);
  usart_set_databits(config::fpga_uart, 8);
  usart_set_parity(config::fpga_uart, USART_PARITY_NONE);
  usart_set_stopbits(config::fpga_uart, USART_CR2_STOPBITS_1);
  usart_set_mode(config::fpga_uart, USART_MODE_TX_RX);
  usart_set_flow_control(config::fpga_uart, USART_FLOWCONTROL_NONE);

  // Enable USART
  usart_enable(config::fpga_uart);

  // Enable interrupts for this usart
  nvic_enable_irq(config::fpga_uart_irq);

  // Enable RX interrupts
  usart_enable_rx_interrupt(config::fpga_uart);
}

void usart1_isr(void) {
  if (usart_get_flag(config::fpga_uart, USART_FLAG_RXNE)) {
    const uint8_t data = usart_recv(config::fpga_uart);
    // TODO(ross)
  }
}
