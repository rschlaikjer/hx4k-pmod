#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>

#include <config.hpp>
#include <rgb.hpp>

void RGB::init() {
  // Enable clock to gpio/timer
  rcc_periph_clock_enable(config::rgb_led_gpio_rcc);
  rcc_periph_clock_enable(config::rgb_led_timer_rcc);

  // Configure LED pins to alternate function
  const uint16_t led_pins =
      config::rgb_led_pin_r | config::rgb_led_pin_g | config::rgb_led_pin_b;
  gpio_mode_setup(config::rgb_led_port, GPIO_MODE_AF, GPIO_PUPD_NONE, led_pins);
  gpio_set_af(config::rgb_led_port, config::rgb_led_timer_af, led_pins);

  // Set timer mode to center mode, count up
  timer_set_mode(config::rgb_led_timer, TIM_CR1_CKD_CK_INT,
                 TIM_CR1_CMS_CENTER_1, TIM_CR1_DIR_UP);

  // Set output compare mode to PWM2 (since our LEDs are active low)
  timer_set_oc_mode(config::rgb_led_timer, TIM_OC1, TIM_OCM_PWM2);
  timer_set_oc_mode(config::rgb_led_timer, TIM_OC2, TIM_OCM_PWM2);
  timer_set_oc_mode(config::rgb_led_timer, TIM_OC3, TIM_OCM_PWM2);

  // Enable hardware ouput for the output compare channels
  timer_enable_oc_output(config::rgb_led_timer, TIM_OC1);
  timer_enable_oc_output(config::rgb_led_timer, TIM_OC2);
  timer_enable_oc_output(config::rgb_led_timer, TIM_OC3);

  // Enable break output (necessary for advanced control timers)
  timer_enable_break_main_output(config::rgb_led_timer);

  // Set the prescalar such that we have 8 bits of precision at a
  // frequency of 1kHz.
  const unsigned prescaler = (rcc_apb1_frequency / 1000 / 255) - 1;
  timer_set_prescaler(config::rgb_led_timer, prescaler);

  // Initialize the OC values to zero
  timer_set_oc_value(config::rgb_led_timer, TIM_OC1, 0);
  timer_set_oc_value(config::rgb_led_timer, TIM_OC2, 0);
  timer_set_oc_value(config::rgb_led_timer, TIM_OC3, 0);

  // Set the period to 255
  timer_set_period(config::rgb_led_timer, 255);

  // Enable the timer
  timer_enable_counter(config::rgb_led_timer);
}

void RGB::set_colour(uint8_t r, uint8_t g, uint8_t b) {
  timer_set_oc_value(config::rgb_led_timer, TIM_OC1, b);
  timer_set_oc_value(config::rgb_led_timer, TIM_OC2, g);
  timer_set_oc_value(config::rgb_led_timer, TIM_OC3, r);
}
