#include "usb_lib.h"

volatile uint8_t bIntPackSOF = 0;   /* SOFs received between 2 consecutive packets */
volatile uint32_t esof_counter = 0; /* expected SOF counter */
volatile uint16_t wIstr;            /* ISTR register last read value */
volatile uint32_t wCNTR = 0;

void (*pEpInt_IN[7])(void) = {
    NOP_Process,
    NOP_Process,
    NOP_Process,
    NOP_Process,
    NOP_Process,
    NOP_Process,
    NOP_Process,
};

void (*pEpInt_OUT[7])(void) = {
    NOP_Process,
    NOP_Process,
    NOP_Process,
    NOP_Process,
    NOP_Process,
    NOP_Process,
    NOP_Process,
};

void usb_istr()
{
    uint32_t i = 0;
    __IO uint32_t EP[8];

    wIstr = _GetISTR();

#if (IMR_MSK & ISTR_CTR)
    if (wIstr & ISTR_CTR & wInterrupt_Mask)
    {
        /* servicing of the endpoint correct transfer interrupt */
        /* clear of the CTR flag into the sub */
        CTR_LP();
#ifdef CTR_CALLBACK
        CTR_Callback();
#endif
    }
#endif

    /*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#if (IMR_MSK & ISTR_RESET)
    if (wIstr & ISTR_RESET & wInterrupt_Mask)
    {
        _SetISTR((uint16_t)CLR_RESET);
        Device_Property.Reset();
#ifdef RESET_CALLBACK
        RESET_Callback();
#endif
    }
#endif

    /*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#if (IMR_MSK & ISTR_DOVR)
    if (wIstr & ISTR_DOVR & wInterrupt_Mask)
    {
        _SetISTR((uint16_t)CLR_DOVR);
#ifdef DOVR_CALLBACK
        DOVR_Callback();
#endif
    }
#endif

    /*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#if (IMR_MSK & ISTR_ERR)
    if (wIstr & ISTR_ERR & wInterrupt_Mask)
    {
        _SetISTR((uint16_t)CLR_ERR);
#ifdef ERR_CALLBACK
        ERR_Callback();
#endif
    }
#endif

    /*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#if (IMR_MSK & ISTR_WKUP)
    if (wIstr & ISTR_WKUP & wInterrupt_Mask)
    {
        _SetISTR((uint16_t)CLR_WKUP);
#ifdef WKUP_CALLBACK
        WKUP_Callback();
#endif
    }
#endif

    /*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#if (IMR_MSK & ISTR_SUSP)
    if (wIstr & ISTR_SUSP & wInterrupt_Mask)
    {
        /* clear of the ISTR bit must be done after setting of CNTR_FSUSP */
        _SetISTR((uint16_t)CLR_SUSP);
#ifdef SUSP_CALLBACK
        SUSP_Callback();
#endif
    }
#endif

    /*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#if (IMR_MSK & ISTR_SOF)
    if (wIstr & ISTR_SOF & wInterrupt_Mask)
    {
        _SetISTR((uint16_t)CLR_SOF);
        bIntPackSOF++;
#ifdef SOF_CALLBACK
        SOF_Callback();
#endif
    }
#endif

    /*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#if (IMR_MSK & ISTR_ESOF)
    if (wIstr & ISTR_ESOF & wInterrupt_Mask)
    {
        /* clear ESOF flag in ISTR */
        _SetISTR((uint16_t)CLR_ESOF);

        if ((_GetFNR() & FNR_RXDP) != 0)
        {
            /* increment ESOF counter */
            esof_counter++;

            /* test if we enter in ESOF more than 3 times with FSUSP =0 and RXDP =1=>> possible missing SUSP flag*/
            if ((esof_counter > 3) && ((_GetCNTR() & CNTR_FSUSP) == 0))
            {
                /* this a sequence to apply a force RESET*/

                /*Store CNTR value */
                wCNTR = _GetCNTR();

                /*Store endpoints registers status */
                for (i = 0; i < 8; i++)
                    EP[i] = _GetENDPOINT(i);

                /*apply FRES */
                wCNTR |= CNTR_FRES;
                _SetCNTR(wCNTR);

                /*clear FRES*/
                wCNTR &= ~CNTR_FRES;
                _SetCNTR(wCNTR);

                /*poll for RESET flag in ISTR*/
                while ((_GetISTR() & ISTR_RESET) == 0)
                    ;

                /* clear RESET flag in ISTR */
                _SetISTR((uint16_t)CLR_RESET);

                /*restore Enpoints*/
                for (i = 0; i < 8; i++)
                    _SetENDPOINT(i, EP[i]);

                esof_counter = 0;
            }
        }
        else
        {
            esof_counter = 0;
        }
#ifdef ESOF_CALLBACK
        ESOF_Callback();
#endif
    }
#endif
}

void USB_LP_CAN1_RX0_IRQHandler(void)
{
	usb_istr();
}