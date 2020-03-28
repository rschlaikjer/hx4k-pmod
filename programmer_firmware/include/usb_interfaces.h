#ifndef USB_INTERFACES_H_
#define USB_INTERFACES_H_

// USB endpoint addresses have the following structure:
// ENDP[3:0] Endpoint number
// ENDP[6:4] Reserved (zero)
// ENDP[7] Direction (0 = OUT, 1 = IN)
// Note that OUT/IN is from the standpoint of the HOST, not the device

#define USB_EP_OUT(number) ((number & 0x0F) | (0 << 7))
#define USB_EP_IN(number) ((number & 0x0F) | (1 << 7))

#define USB_MAX_PACKET_SIZE 64
// CDC data endpoints
#define USB_EP_CDC_DATA_TX USB_EP_IN(2)
#define USB_EP_CDC_DATA_RX USB_EP_OUT(1)
// CDC control endpoint
#define USB_EP_CDC_COMM USB_EP_IN(3)
// Programming endpoint
#define USB_EP_FLASH_TX USB_EP_IN(4)
#define USB_EP_FLASH_RX USB_EP_OUT(2)

#endif // if not defined(USB_INTERFACES_H_)
