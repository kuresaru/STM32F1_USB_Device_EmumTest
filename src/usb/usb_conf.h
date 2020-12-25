#ifndef __USB_CONF
#define __USB_CONF

#define IMR_MSK (CNTR_CTRM | CNTR_WKUPM | CNTR_SUSPM | CNTR_ERRM  | CNTR_SOFM | CNTR_ESOFM | CNTR_RESETM)

#define BTABLE_ADDRESS      (0x00)
#define ENDP0_RXADDR        (0x180)
#define ENDP0_TXADDR        (0x1C0)

#endif