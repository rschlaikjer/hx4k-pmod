#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/usb/cdc.h>
#include <libopencm3/usb/usbd.h>

#include <config.hpp>
#include <usb.hpp>
#include <usb_interfaces.h>
#include <usb_protocol.hpp>

namespace {
// Handle to HAL usb device
static usbd_device *usb_handle;
// Buffer for the HAL to handle control requests with
uint8_t usbd_control_buffer[128];
} // namespace

// Device descriptor.
// Since we will expose a usb serial endpoint that is connected to our UART,
// we will set the device class to be a CDC device.
// This still allows us to have our custom endpoints for uploading bitstreams.
static const struct usb_device_descriptor descriptor_device = {
    .bLength = USB_DT_DEVICE_SIZE,
    .bDescriptorType = USB_DT_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = USB_CLASS_CDC,
    .bDeviceSubClass = 0,
    .bDeviceProtocol = 0,
    .bMaxPacketSize0 = USB_MAX_PACKET_SIZE,
    .idVendor = config::usb_id_vendor,
    .idProduct = config::usb_id_product,
    .bcdDevice = config::usb_id_bcd_device_revision,
    .iManufacturer = 1,
    .iProduct = 2,
    .iSerialNumber = 3,
    .bNumConfigurations = 1,
};

// Descriptor configuration. Important attributes
// - We have three interfaces
//    - CDC communication
//    - CDC data
//    - Custom programmer
// - We aren't sure what the power status of the device is, so request the
// standard USB maximum of 500mA
// - To take advantage of C struct rules, our interface informatuion in in the
// usb_interfaces.c file, note the extern.
extern const struct usb_interface usb_interfaces[];
static const struct usb_config_descriptor descriptor_config = {
    .bLength = USB_DT_CONFIGURATION_SIZE,
    .bDescriptorType = USB_DT_CONFIGURATION,
    .wTotalLength = 0,
    .bNumInterfaces = 3,
    .bConfigurationValue = 1,
    .iConfiguration = 0,
    .bmAttributes = 0x80,
    .bMaxPower = 250, // x2 mA
    .interface = usb_interfaces,
};

// Buffer for the serial number of this device, as a hexadecimal string.
char usb_string_serial[25] = {0};

// String table for the USB descriptors
static const char *descriptor_strings[] = {
    config::usb_string_manufacturer,
    config::usb_string_product,
    usb_string_serial,
};

void uint32_to_hex(uint32_t v, char *out) {
  // Convert a uint32 to a hex string, most significant nibble first
  for (int i = 0; i < 8; i++) {
    const uint8_t nibble = (v >> (i * 4)) & 0xF;
    out[7 - i] = nibble < 10 ? nibble + '0' : nibble + 'A';
  }
}

static enum usbd_request_return_codes cdcacm_control_request(
    usbd_device *usbd_dev, struct usb_setup_data *req, uint8_t **buf,
    uint16_t *len,
    void (**complete)(usbd_device *usbd_dev, struct usb_setup_data *req)) {
  (void)complete;
  (void)buf;
  (void)usbd_dev;

  switch (req->bRequest) {
  case USB_CDC_REQ_SET_CONTROL_LINE_STATE: {
    /*
     * This Linux cdc_acm driver requires this to be implemented
     * even though it's optional in the CDC spec, and we don't
     * advertise it in the ACM functional descriptor.
     */
    return USBD_REQ_HANDLED;
  }
  case USB_CDC_REQ_SET_LINE_CODING:
    if (*len < sizeof(struct usb_cdc_line_coding))
      return USBD_REQ_NOTSUPP;
    return USBD_REQ_HANDLED;
  }
  return USBD_REQ_NOTSUPP;
}

void cdc_data_ep_rx_cb(usbd_device *usbd_dev, uint8_t ep) {
  // TODO(ross)
}

void flash_ep_rx_cb(usbd_device *usbd_dev, uint8_t ep) {
  // Read the packet from the endpoint
  uint8_t buf[USB_MAX_PACKET_SIZE];
  int len = usbd_ep_read_packet(usbd_dev, ep, buf, USB_MAX_PACKET_SIZE);
  // Submit it to the usb state machine
  UsbProto::handle_packet(buf, len);
}

void usb_config_cb(usbd_device *usbd_dev,
                   uint16_t __attribute__((unused)) wValue) {
  // CDC data
  usbd_ep_setup(usbd_dev, USB_EP_CDC_DATA_RX, USB_ENDPOINT_ATTR_BULK,
                USB_MAX_PACKET_SIZE, cdc_data_ep_rx_cb);
  usbd_ep_setup(usbd_dev, USB_EP_CDC_DATA_TX, USB_ENDPOINT_ATTR_BULK,
                USB_MAX_PACKET_SIZE, nullptr);

  // CDC control
  usbd_ep_setup(usbd_dev, USB_EP_CDC_COMM, USB_ENDPOINT_ATTR_INTERRUPT, 16,
                nullptr);
  usbd_register_control_callback(
      usbd_dev, USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE,
      USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT, cdcacm_control_request);

  // Flash programming
  usbd_ep_setup(usbd_dev, USB_EP_FLASH_RX, USB_ENDPOINT_ATTR_BULK,
                USB_MAX_PACKET_SIZE, flash_ep_rx_cb);
  usbd_ep_setup(usbd_dev, USB_EP_FLASH_TX, USB_ENDPOINT_ATTR_BULK,
                USB_MAX_PACKET_SIZE, nullptr);
}

void USB::init() {
  // Generate our serial number based on the unique ID registers of the STM32.
  uint32_to_hex(DESIG_UNIQUE_ID0, &usb_string_serial[0]);
  uint32_to_hex(DESIG_UNIQUE_ID1, &usb_string_serial[8]);
  uint32_to_hex(DESIG_UNIQUE_ID2, &usb_string_serial[16]);

  // Enable USB clocks
  rcc_periph_clock_enable(RCC_USB);
  rcc_set_usbclk_source(RCC_PLL);

  // Initialize the HAL USB driver
  usb_handle = usbd_init(&st_usbfs_v2_usb_driver, &descriptor_device,
                         &descriptor_config, descriptor_strings, 3,
                         usbd_control_buffer, sizeof(usbd_control_buffer));

  // Register our USB configuration callback
  usbd_register_set_config_callback(usb_handle, usb_config_cb);
}

void USB::poll() { usbd_poll(usb_handle); }
