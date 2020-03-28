#ifndef USB_INTERFACES_H_
#define USB_INTERFACES_H_

// USB endpoint addresses have the following structure:
// ENDP[3:0] Endpoint number
// ENDP[6:4] Reserved (zero)
// ENDP[7] Direction (0 = OUT, 1 = IN)

#define USB_EP_OUT(number) ((number & 0x0F) | (0 << 7))
#define USB_EP_IN(number) ((number & 0x0F) | (1 << 7))

#define USB_MAX_PACKET_SIZE 64
// CDC data endpoints
#define USB_EP_CDC_DATA_RX USB_EP_IN(1)  // 0x01
#define USB_EP_CDC_DATA_TX USB_EP_OUT(2) // 0x82
// CDC control endpoint
#define USB_EP_CDC_COMM USB_EP_OUT(3) // 0x83
// Programming endpoint
#define USB_EP_FLASH_IN USB_EP_IN(2)   // 0x02
#define USB_EP_FLASH_OUT USB_EP_OUT(4) // 0x84

#endif // if not defined(USB_INTERFACES_H_)
