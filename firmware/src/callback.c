/*******************************************************************************
 * @file callback.c
 * @brief USB Callbacks.
 *******************************************************************************/

//=============================================================================
// src/callback.c: generated by Hardware Configurator
//
// This file is only generated if it does not exist. Modifications in this file
// will persist even if Configurator generates code. To refresh this file,
// you must first delete it and then regenerate code.
//=============================================================================
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <SI_EFM8UB1_Register_Enums.h>
#include <efm8_usb.h>
#include <stdio.h>
#include "idle.h"
#include "app.h"
#include "bsp.h"
#include "descriptors.h"
#include "u2f_hid.h"

#define HID_INTERFACE_INDEX 0

uint8_t tmpBuffer;

//#define PRINT_EVENTS

#ifdef PRINT_EVENTS

#define u2f_print_ev(s) u2f_prints(s)

#else
#define u2f_print_ev(x)
#endif


void USBD_ResetCb(void) {
	u2f_print_ev("USBD_ResetCb\r\n");
}


void USBD_DeviceStateChangeCb(USBD_State_TypeDef oldState,
		USBD_State_TypeDef newState) {
	u2f_print_ev("USBD_DeviceStateChangeCb\r\n");
}

bool USBD_IsSelfPoweredCb(void) {
	return false;
}


USB_Status_TypeDef USBD_SetupCmdCb(
		SI_VARIABLE_SEGMENT_POINTER(setup, USB_Setup_TypeDef, MEM_MODEL_SEG)) {

	USB_Status_TypeDef retVal = USB_STATUS_REQ_UNHANDLED;


	if ((setup->bmRequestType.Type == USB_SETUP_TYPE_STANDARD)
			&& (setup->bmRequestType.Direction == USB_SETUP_DIR_IN)
			&& (setup->bmRequestType.Recipient == USB_SETUP_RECIPIENT_INTERFACE)) {
		// A HID device must extend the standard GET_DESCRIPTOR command
		// with support for HID descriptors.

		switch (setup->bRequest) {
		case GET_DESCRIPTOR:
			if (setup->wIndex == 0)
			{
				if ((setup->wValue >> 8) == USB_HID_REPORT_DESCRIPTOR) {

						USBD_Write(EP0, ReportDescriptor0,
								EFM8_MIN(sizeof(ReportDescriptor0), setup->wLength),
								false);
						retVal = USB_STATUS_OK;

				} else if ((setup->wValue >> 8) == USB_HID_DESCRIPTOR) {

						USBD_Write(EP0, (&configDesc[18]),
								EFM8_MIN(USB_HID_DESCSIZE, setup->wLength), false);
						retVal = USB_STATUS_OK;

				}
			}
			break;
		}
	}
	else if ((setup->bmRequestType.Type == USB_SETUP_TYPE_CLASS)
	           && (setup->bmRequestType.Recipient == USB_SETUP_RECIPIENT_INTERFACE)
	           && (setup->wIndex == HID_INTERFACE_INDEX))
	  {
	    // Implement the necessary HID class specific commands.
	    switch (setup->bRequest)
	    {
	      case USB_HID_SET_IDLE:
	        if (((setup->wValue & 0xFF) == 0)             // Report ID
	            && (setup->wLength == 0)
	            && (setup->bmRequestType.Direction != USB_SETUP_DIR_IN))
	        {
	          retVal = USB_STATUS_OK;
	        }
	        break;

	      case USB_HID_GET_IDLE:
	        if ((setup->wValue == 0)                      // Report ID
	            && (setup->wLength == 1)
	            && (setup->bmRequestType.Direction == USB_SETUP_DIR_IN))
	        {
	          tmpBuffer = 24;
	          USBD_Write(EP0, &tmpBuffer, 1, false);
	          retVal = USB_STATUS_OK;
	        }
	        break;
	      default:
	    	  break;
	    }
	  }

	return retVal;
}


uint8_t hidmsgbuf[64];

uint16_t USBD_XferCompleteCb(uint8_t epAddr, USB_Status_TypeDef status,
		uint16_t xferred, uint16_t remaining) {


	if (epAddr == EP1OUT)
	{
		// dump_hex(hidmsgbuf, sizeof(hidmsgbuf));

		set_app_u2f_hid_msg((struct u2f_hid_msg *) hidmsgbuf );

	}
	return 0;
}


