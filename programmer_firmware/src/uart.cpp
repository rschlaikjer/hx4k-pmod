#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>

#include <config.hpp>
#include <uart.hpp>

static const unsigned buffer_size = 256;

class RingBuffer {
public:
  bool write(uint8_t c) {
    if (full())
      return false;
    _data[_write_index++] = c;
    _write_index = _write_index % buffer_size;
    return true;
  }
  bool read(uint8_t *c) {
    if (!available())
      return false;
    *c = _data[_read_index++];
    _read_index = _read_index % buffer_size;
    return true;
  }
  unsigned write(const uint8_t *in_data, unsigned count) {
    unsigned written = 0;
    while (written < count && write(in_data[written])) {
      written++;
    }
    return written;
  }
  int read(uint8_t *in_data, unsigned count) {
    unsigned nread = 0;
    while (nread < count && read(&in_data[nread])) {
      nread++;
    }
    return nread;
  }
  bool full() { return ((_write_index + 1) % buffer_size) == _read_index; }
  bool available() { return _write_index != _read_index; }

private:
  uint8_t _data[buffer_size];
  unsigned _write_index = 0;
  unsigned _read_index = 0;
};

namespace {
RingBuffer _rx_buffer;
RingBuffer _tx_buffer;
} // namespace

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

void UART::send(const uint8_t *buf, unsigned count) {
  // Copy the data to send into a buffer
  _tx_buffer.write(buf, count);

  // Ensure TXE interrupt is on so that we flush the tx buffer
  usart_enable_tx_interrupt(config::fpga_uart);
}

unsigned UART::receive(uint8_t *buf, unsigned count) {
  // Just call through to our rx buffer
  return _rx_buffer.read(buf, count);
}

bool UART::available() { return _rx_buffer.available(); }

void usart1_isr(void) {
  // Have we received data?
  if (usart_get_flag(config::fpga_uart, USART_FLAG_RXNE)) {
    _rx_buffer.write(usart_recv(config::fpga_uart));
  }

  // Are we clear to transmit more data?
  if (usart_get_flag(config::fpga_uart, USART_FLAG_TXE)) {
    uint8_t to_tx;
    if (!_tx_buffer.read(&to_tx)) {
      // Nothing left in tx buffer, disable this interrupt
      usart_disable_tx_interrupt(config::fpga_uart);
    } else {
      // Copy the data to the tx register
      usart_send(config::fpga_uart, to_tx);
    }
  }
}
