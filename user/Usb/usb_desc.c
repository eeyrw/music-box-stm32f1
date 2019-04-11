/**
  ******************************************************************************
  * @file    usb_desc.c
  * @author  MCD Application Team
  * @version V4.1.0
  * @date    26-May-2017
  * @brief   Descriptors for Usb midi and Cdc composite device.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_desc.h"

/* USB Standard Device Descriptor */
const uint8_t Midi_CDC_Device_DeviceDescriptor[] =
  {
    0x12,   /* bLength */
    USB_DEVICE_DESCRIPTOR_TYPE,     /* bDescriptorType */
    0x00,
    0x02,   /* bcdUSB = 2.00 */
    0xEF, // Class Code
    0x02, // Subclass code
    0x01, // Protocol code
    0x40,   /* bMaxPacketSize0 */
    0x83,
    0x04,   /* idVendor = 0x0483 */
    0x66,
    0x67,   /* idProduct = 0x7666 */
    0x00,
    0x02,   /* bcdDevice = 2.00 */
    1,              /* Index of string descriptor describing manufacturer */
    2,              /* Index of string descriptor describing product */
    3,              /* Index of string descriptor describing the device's serial number */
    0x01    /* bNumConfigurations */
  };

const uint8_t Midi_CDC_Device_ConfigDescriptor[] =
  {
    /*Configuration Descriptor*/
    0x09,   /* bLength: Configuration Descriptor size */
    USB_CONFIGURATION_DESCRIPTOR_TYPE,      /* bDescriptorType: Configuration */
    MIDI_CDC_DEV_SIZ_CONFIG_DESC,       /* wTotalLength:no of returned bytes */
    0x00,
    0x04,   /* bNumInterfaces: 4 interface */
    0x01,   /* bConfigurationValue: Configuration value */
    0x00,   /* iConfiguration: Index of string descriptor describing the configuration */
    0xC0,   /* bmAttributes: self powered */
    0x32,   /* MaxPower 0 mA */

    /* Interface Association Descriptor: Audio Function 1*/
    0x08, // Size of this descriptor in bytes
    0x0B, // Interface assocication descriptor type
    MIDI_IF_0_ID, // The first associated interface
    0x02, // Number of contiguous associated interface
    0x01, // bInterfaceClass of the first interface
    0x01, // bInterfaceSubclass of the first interface
    0x00, // bInterfaceProtocol of the first interface
    0x00, // Interface string index

    0x09,//sizeof(USB_INTF_DSC),   // Size of this descriptor in bytes
    USB_INTERFACE_DESCRIPTOR_TYPE,               // INTERFACE descriptor type
    MIDI_IF_0_ID,                      // Interface Number
    0,                      // Alternate Setting Number
    0,                      // Number of endpoints in this intf
    1,               		// Class code
    1,     					// Subclass code
    0,     					// Protocol code
    0,                      // Interface string index
        /* MIDI Adapter Class-specific AC Interface Descriptor */
    0x09,       //bLength
    0x24,       //bDescriptorType - CS_INTERFACE
    0x01,       //bDescriptorSubtype - HEADER
    0x00,0x01,  //bcdADC
    0x09,0x00,  //wTotalLength
    0x01,       //bInCollection
    0x01,       //baInterfaceNr(1)

    /* MIDI Adapter Standard MS Interface Descriptor */
    0x09,       //bLength
    USB_INTERFACE_DESCRIPTOR_TYPE,       //bDescriptorType
    MIDI_IF_1_ID,       //bInterfaceNumber
    0x00,       //bAlternateSetting
    0x02,       //bNumEndpoints
    0x01,       //bInterfaceClass
    0x03,       //bInterfaceSubclass
    0x00,       //bInterfaceProtocol
    0x00,       //iInterface

    /* MIDI Adapter Class-specific MS Interface Descriptor */
    0x07,       //bLength
    0x24,       //bDescriptorType - CS_INTERFACE
    0x01,       //bDescriptorSubtype - MS_HEADER
    0x00,0x01,  //BcdADC
    0x41,0x00,  //wTotalLength

    /* MIDI Adapter MIDI IN Jack Descriptor (Embedded) */
    0x06,       //bLength
    0x24,       //bDescriptorType - CS_INTERFACE
    0x02,       //bDescriptorSubtype - MIDI_IN_JACK
    0x01,       //bJackType - EMBEDDED
    0x01,       //bJackID
    0x00,       //iJack

    /* MIDI Adapter MIDI IN Jack Descriptor (External) */
    0x06,       //bLength
    0x24,       //bDescriptorType - CS_INTERFACE
    0x02,       //bDescriptorSubtype - MIDI_IN_JACK
    0x02,       //bJackType - EXTERNAL
    0x02,       //bJackID
    0x00,       //iJack

    /* MIDI Adapter MIDI OUT Jack Descriptor (Embedded) */
    0x09,       //bLength
    0x24,       //bDescriptorType - CS_INTERFACE
    0x03,       //bDescriptorSubtype - MIDI_OUT_JACK
    0x01,       //bJackType - EMBEDDED
    0x03,       //bJackID
    0x01,       //bNrInputPins
    0x02,       //BaSourceID(1)
    0x01,       //BaSourcePin(1)
    0x00,       //iJack

    /* MIDI Adapter MIDI OUT Jack Descriptor (External) */
    0x09,       //bLength
    0x24,       //bDescriptorType - CS_INTERFACE
    0x03,       //bDescriptorSubtype - MIDI_OUT_JACK
    0x02,       //bJackType - EXTERNAL
    0x04,       //bJackID
    0x01,       //bNrInputPins
    0x01,       //BaSourceID(1)
    0x01,       //BaSourcePin(1)
    0x00,       //iJack

    /* MIDI Adapter Standard Bulk OUT Endpoint Descriptor */
    0x09,       //bLength
    USB_ENDPOINT_DESCRIPTOR_TYPE,       //bDescriptorType - ENDPOINT
    0x04,       //bEndpointAddress - OUT
    0x02,       //bmAttributes
    MIDI_CDC_DEV_DATA_SIZE,0x00,  //wMaxPacketSize
    0x00,       //bInterval
    0x00,       //bRefresh
    0x00,       //bSynchAddress

    /* MIDI Adapter Class-specific Bulk OUT Endpoint Descriptor */
    0x05,       //bLength
    0x25,       //bDescriptorType - CS_ENDPOINT
    0x01,       //bDescriptorSubtype - MS_GENERAL
    0x01,       //bNumEmbMIDIJack
    0x01,       //BaAssocJackID(1)

    /* MIDI Adapter Standard Bulk IN Endpoint Descriptor */
    0x09,       //bLength
    USB_ENDPOINT_DESCRIPTOR_TYPE,       //bDescriptorType
    0x85,       //bEndpointAddress
    0x02,       //bmAttributes
    MIDI_CDC_DEV_DATA_SIZE,0x00,  //wMaxPacketSize
    0x00,       //bInterval
    0x00,       //bRefresh
    0x00,       //bSynchAddress

    /* MIDI Adapter Class-specific Bulk IN Endpoint Descriptor */
    0x05,       //bLength
    0x25,       //bDescriptorType - CS_ENDPOINT
    0x01,       //bDescriptorSubtype - MS_GENERAL
    0x01,       //bNumEmbMIDIJack
    0x03,        //BaAssocJackID(1)
        // IAD ------------------------------------------------------------------------------------------------------------

    // Interface Association Descriptor
    0x08,                               // Size of this descriptor in bytes
    0x0B,                               // Interface association descriptor type
    CDC_COM_IF_ID,                               // First associated interface
    0x02,                               // Number of contiguous associated interfaces
    0x02,                          // bInterfaceClass of the first interface
    0x02,             // bInterfaceSubClass of the first interface
    0x01,                             // bInterfaceProtocol of the first interface
    0x00,                               // Interface string index
    /*Interface Descriptor*/
    0x09,   /* bLength: Interface Descriptor size */
    USB_INTERFACE_DESCRIPTOR_TYPE,  /* bDescriptorType: Interface */
    /* Interface descriptor type */
    CDC_COM_IF_ID,   /* bInterfaceNumber: Number of Interface */
    0x00,   /* bAlternateSetting: Alternate setting */
    0x01,   /* bNumEndpoints: One endpoints used */
    0x02,   /* bInterfaceClass: Communication Interface Class */
    0x02,   /* bInterfaceSubClass: Abstract Control Model */
    0x01,   /* bInterfaceProtocol: Common AT commands */
    0x00,   /* iInterface: */
    /*Header Functional Descriptor*/
    0x05,   /* bLength: Endpoint Descriptor size */
    0x24,   /* bDescriptorType: CS_INTERFACE */
    0x00,   /* bDescriptorSubtype: Header Func Desc */
    0x10,   /* bcdCDC: spec release number */
    0x01,
    /*Call Management Functional Descriptor*/
    0x05,   /* bFunctionLength */
    0x24,   /* bDescriptorType: CS_INTERFACE */
    0x01,   /* bDescriptorSubtype: Call Management Func Desc */
    0x00,   /* bmCapabilities: D0+D1 */
    CDC_DATA_IF_ID,   /* bDataInterface: 1 */
    /*ACM Functional Descriptor*/
    0x04,   /* bFunctionLength */
    0x24,   /* bDescriptorType: CS_INTERFACE */
    0x02,   /* bDescriptorSubtype: Abstract Control Management desc */
    0x02,   /* bmCapabilities */
    /*Union Functional Descriptor*/
    0x05,   /* bFunctionLength */
    0x24,   /* bDescriptorType: CS_INTERFACE */
    0x06,   /* bDescriptorSubtype: Union func desc */
    CDC_COM_IF_ID,   /* bMasterInterface: Communication class interface */
    CDC_DATA_IF_ID,   /* bSlaveInterface0: Data Class Interface */
    /*Endpoint 2 Descriptor*/
    0x07,   /* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint */
    0x82,   /* bEndpointAddress: (IN2) */
    0x03,   /* bmAttributes: Interrupt */
    MIDI_CDC_DEV_INT_SIZE,      /* wMaxPacketSize: */
    0x00,
    0xFF,   /* bInterval: */
    /*Data class interface descriptor*/
    0x09,   /* bLength: Endpoint Descriptor size */
    USB_INTERFACE_DESCRIPTOR_TYPE,  /* bDescriptorType: */
    CDC_DATA_IF_ID,   /* bInterfaceNumber: Number of Interface */
    0x00,   /* bAlternateSetting: Alternate setting */
    0x02,   /* bNumEndpoints: Two endpoints used */
    0x0A,   /* bInterfaceClass: CDC */
    0x00,   /* bInterfaceSubClass: */
    0x00,   /* bInterfaceProtocol: */
    0x00,   /* iInterface: */
    /*Endpoint 3 Descriptor*/
    0x07,   /* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint */
    0x03,   /* bEndpointAddress: (OUT3) */
    0x02,   /* bmAttributes: Bulk */
    MIDI_CDC_DEV_DATA_SIZE,             /* wMaxPacketSize: */
    0x00,
    0x00,   /* bInterval: ignore for Bulk transfer */
    /*Endpoint 1 Descriptor*/
    0x07,   /* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint */
    0x81,   /* bEndpointAddress: (IN1) */
    0x02,   /* bmAttributes: Bulk */
    MIDI_CDC_DEV_DATA_SIZE,             /* wMaxPacketSize: */
    0x00,
    0x00,    /* bInterval */

  };

/* USB String Descriptors */
const uint8_t Midi_CDC_Device_StringLangID[MIDI_CDC_DEV_SIZ_STRING_LANGID] =
  {
    MIDI_CDC_DEV_SIZ_STRING_LANGID,
    USB_STRING_DESCRIPTOR_TYPE,
    0x09,
    0x04 /* LangID = 0x0409: U.S. English */
  };

const uint8_t Midi_CDC_Device_StringVendor[MIDI_CDC_DEV_SIZ_STRING_VENDOR] =
  {
    MIDI_CDC_DEV_SIZ_STRING_VENDOR,     /* Size of Vendor string */
    USB_STRING_DESCRIPTOR_TYPE,             /* bDescriptorType*/
    /* Manufacturer: "STMicroelectronics" */
    'Y', 0, 'u', 0, 'a', 0, 'n',
  };

const uint8_t Midi_CDC_Device_StringProduct[MIDI_CDC_DEV_SIZ_STRING_PRODUCT] =
  {
    MIDI_CDC_DEV_SIZ_STRING_PRODUCT,          /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
    /* Product name: "STM32 Midi CDC Device" */
    'S', 0, 'T', 0, 'M', 0, '3', 0, '2', 0, ' ', 0, 'M', 0, 'I', 0,
    'D', 0, 'I', 0, ' ', 0, 'D', 0, 'e', 0, 'v', 0, 'i', 0, 'c', 0,
    'e', 0, 
  };

uint8_t Midi_CDC_Device_StringSerial[MIDI_CDC_DEV_SIZ_STRING_SERIAL] =
  {
    MIDI_CDC_DEV_SIZ_STRING_SERIAL,           /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,                   /* bDescriptorType */
    'S', 0, 'T', 0, 'M', 0, '3', 0, '2', 0
  };

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
