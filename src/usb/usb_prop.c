#include "usb_lib.h"

#define wVal(x) ((x)&0xFF), (((x) >> 8) & 0xFF)
#define CFGDES_SIZE (9)

// desc

static uint8_t devdesc[18] = {
    18,         // bLength
    0x01,       // bDescriptorType
    0x00, 0x02, // bcdUSB
    0x00,       // bDeviceClass
    0x00,       // bDeviceSubClass
    0x00,       // bDeviceProtocol
    64,         // bMaxPacketSize0
    0x34, 0x12, // idVentor
    0x78, 0x56, // idProduct
    0x01, 0x00, // bcdDevice
    0,          // iManufacturer
    0,          // iProduct
    0,          // iSerialNumber
    1,          // bNumConfiguration
};

static uint8_t confdesc[9] = {
    9,                 // bLength
    0x02,              // bDescriptorType
    wVal(CFGDES_SIZE), // wTotalLength
    0,                 // bNumInterfaces
    1,                 // bConfigurationValue
    0,                 // iConfiguration
    0x80,              // bmAttributes
    250,               // bMaxPower
};

static ONE_DESCRIPTOR dev_desc = {
    .Descriptor = devdesc,
    .Descriptor_Size = sizeof(devdesc),
};

static ONE_DESCRIPTOR conf_desc = {
    .Descriptor = confdesc,
    .Descriptor_Size = sizeof(confdesc),
};

// function

static void Init()
{
    // reset
    pInformation->Current_Configuration = 0;
    _SetCNTR(CNTR_FRES);
    // mask all interrupt
    wInterrupt_Mask = 0;
    _SetCNTR(wInterrupt_Mask);
    // clear interrupt
    SetISTR(0);
    // enable interrupt
    wInterrupt_Mask = CNTR_RESETM | CNTR_SUSPM | CNTR_WKUPM;
    _SetCNTR(wInterrupt_Mask);
    // init
    USB_SIL_Init();
}

static void Reset()
{
    pInformation->Current_Configuration = 0;
    pInformation->Current_Interface = 0;
    pInformation->Current_Feature = confdesc[7];
    // init ep
    SetEPType(ENDP0, EP_CONTROL);
    SetEPTxStatus(ENDP0, EP_TX_STALL);
    SetEPRxAddr(ENDP0, ENDP0_RXADDR);
    SetEPTxAddr(ENDP0, ENDP0_TXADDR);
    Clear_Status_Out(ENDP0);
    SetEPRxCount(ENDP0, Device_Property.MaxPacketSize);
    SetEPRxValid(ENDP0);
    SetDeviceAddress(0);
}

static uint8_t *get_devdesc(uint16_t len)
{
    return Standard_GetDescriptorData(len, &dev_desc);
}

static uint8_t *get_confdesc(uint16_t len)
{
    return Standard_GetDescriptorData(len, &conf_desc);
}

static uint8_t *get_strdesc(uint16_t len)
{
    return NULL;
}

static RESULT Data_Setup(uint8_t req)
{
    return USB_UNSUPPORT;
}

static RESULT NoData_Setup(uint8_t req)
{
    return USB_UNSUPPORT;
}

static RESULT Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting)
{
    return ((Interface == 0) && (AlternateSetting == 0)) ? USB_SUCCESS : USB_UNSUPPORT;
}

// table

DEVICE Device_Table = {
    .Total_Endpoint = 1,
    .Total_Configuration = 1,
};

DEVICE_PROP Device_Property = {
    .Init = Init,
    .Reset = Reset,
    .Process_Status_IN = NOP_Process,
    .Process_Status_OUT = NOP_Process,
    .Class_Data_Setup = Data_Setup,
    .Class_NoData_Setup = NoData_Setup,
    .Class_Get_Interface_Setting = Get_Interface_Setting,
    .GetDeviceDescriptor = get_devdesc,
    .GetConfigDescriptor = get_confdesc,
    .GetStringDescriptor = get_strdesc,
    .RxEP_buffer = 0,
    .MaxPacketSize = 64,
};

USER_STANDARD_REQUESTS User_Standard_Requests = {
    .User_GetConfiguration = NOP_Process,
    .User_SetConfiguration = NOP_Process,
    .User_GetInterface = NOP_Process,
    .User_SetInterface = NOP_Process,
    .User_GetStatus = NOP_Process,
    .User_ClearFeature = NOP_Process,
    .User_SetEndPointFeature = NOP_Process,
    .User_SetDeviceFeature = NOP_Process,
    .User_SetDeviceAddress = NOP_Process,
};
