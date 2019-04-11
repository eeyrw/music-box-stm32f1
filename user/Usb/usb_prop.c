/**
  ******************************************************************************
  * @file    usb_prop.c
  * @author  MCD Application Team
  * @version V4.1.0
  * @date    26-May-2017
  * @brief   All processing related to Virtual Com Port Demo
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
#include "usb_conf.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "hal.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t Request = 0;

LINE_CODING linecoding =
  {
    115200, /* baud rate*/
    0x00,   /* stop bits-1*/
    0x00,   /* parity - none*/
    0x08    /* no. of bits 8*/
  };

/* -------------------------------------------------------------------------- */
/*  Structures initializations */
/* -------------------------------------------------------------------------- */

DEVICE Device_Table =
  {
    EP_NUM,
    1
  };

DEVICE_PROP Device_Property =
  {
    Midi_CDC_Device_init,
    Midi_CDC_Device_Reset,
    Midi_CDC_Device_Status_In,
    Midi_CDC_Device_Status_Out,
    Midi_CDC_Device_Data_Setup,
    Midi_CDC_Device_NoData_Setup,
    Midi_CDC_Device_Get_Interface_Setting,
    Midi_CDC_Device_GetDeviceDescriptor,
    Midi_CDC_Device_GetConfigDescriptor,
    Midi_CDC_Device_GetStringDescriptor,
    0,
    0x40 /*MAX PACKET SIZE*/
  };

USER_STANDARD_REQUESTS User_Standard_Requests =
  {
    Midi_CDC_Device_GetConfiguration,
    Midi_CDC_Device_SetConfiguration,
    Midi_CDC_Device_GetInterface,
    Midi_CDC_Device_SetInterface,
    Midi_CDC_Device_GetStatus,
    Midi_CDC_Device_ClearFeature,
    Midi_CDC_Device_SetEndPointFeature,
    Midi_CDC_Device_SetDeviceFeature,
    Midi_CDC_Device_SetDeviceAddress
  };

ONE_DESCRIPTOR Device_Descriptor =
  {
    (uint8_t*)Midi_CDC_Device_DeviceDescriptor,
    MIDI_CDC_DEV_SIZ_DEVICE_DESC
  };

ONE_DESCRIPTOR Config_Descriptor =
  {
    (uint8_t*)Midi_CDC_Device_ConfigDescriptor,
    MIDI_CDC_DEV_SIZ_CONFIG_DESC
  };

ONE_DESCRIPTOR String_Descriptor[4] =
  {
    {(uint8_t*)Midi_CDC_Device_StringLangID, MIDI_CDC_DEV_SIZ_STRING_LANGID},
    {(uint8_t*)Midi_CDC_Device_StringVendor, MIDI_CDC_DEV_SIZ_STRING_VENDOR},
    {(uint8_t*)Midi_CDC_Device_StringProduct, MIDI_CDC_DEV_SIZ_STRING_PRODUCT},
    {(uint8_t*)Midi_CDC_Device_StringSerial, MIDI_CDC_DEV_SIZ_STRING_SERIAL}
  };

/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : Midi_CDC_Device_init.
* Description    : Midi CDC Device Mouse init routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Midi_CDC_Device_init(void)
{

  /* Update the serial number string descriptor with the data from the unique
  ID*/
  InitSnStringWith64bitId();

  pInformation->Current_Configuration = 0;

  /* Connect the device */
  PowerOn();

  /* Perform basic device initialization operations */
  USB_SIL_Init();

  bDeviceState = UNCONNECTED;
}

/*******************************************************************************
* Function Name  : Midi_CDC_Device_Reset
* Description    : Virtual_Com_Port Mouse reset routine
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Midi_CDC_Device_Reset(void)
{
  /* Set Virtual_Com_Port DEVICE as not configured */
  pInformation->Current_Configuration = 0;

  /* Current Feature initialization */
  pInformation->Current_Feature = Midi_CDC_Device_ConfigDescriptor[7];

  /* Set Virtual_Com_Port DEVICE with the default Interface*/
  pInformation->Current_Interface = 0;

  SetBTABLE(BTABLE_ADDRESS);

  /* Initialize Endpoint 0 */
  SetEPType(ENDP0, EP_CONTROL);
  SetEPTxStatus(ENDP0, EP_TX_STALL);
  SetEPRxAddr(ENDP0, ENDP0_RXADDR);
  SetEPTxAddr(ENDP0, ENDP0_TXADDR);
  Clear_Status_Out(ENDP0);
  SetEPRxCount(ENDP0, Device_Property.MaxPacketSize);
  SetEPRxValid(ENDP0);

  /* Initialize Endpoint 1 */
  SetEPType(ENDP1, EP_BULK);
  SetEPTxAddr(ENDP1, ENDP1_TXADDR);
  SetEPTxStatus(ENDP1, EP_TX_NAK);
  SetEPRxStatus(ENDP1, EP_RX_DIS);

  /* Initialize Endpoint 2 */
  SetEPType(ENDP2, EP_INTERRUPT);
  SetEPTxAddr(ENDP2, ENDP2_TXADDR);
  SetEPRxStatus(ENDP2, EP_RX_DIS);
  SetEPTxStatus(ENDP2, EP_TX_NAK);

  /* Initialize Endpoint 3 */
  SetEPType(ENDP3, EP_BULK);
  SetEPRxAddr(ENDP3, ENDP3_RXADDR);
  SetEPRxCount(ENDP3, MIDI_CDC_DEV_DATA_SIZE);
  SetEPRxStatus(ENDP3, EP_RX_VALID);
  SetEPTxStatus(ENDP3, EP_TX_DIS);

  /* Initialize Endpoint 5 */
  SetEPType(ENDP5, EP_BULK);
  SetEPTxAddr(ENDP5, ENDP5_TXADDR);
  SetEPRxStatus(ENDP5, EP_RX_DIS);
  SetEPTxStatus(ENDP5, EP_TX_NAK);

  /* Initialize Endpoint 4 */
  SetEPType(ENDP4, EP_BULK);
  SetEPRxAddr(ENDP4, ENDP4_RXADDR);
  SetEPRxCount(ENDP4, MIDI_CDC_DEV_DATA_SIZE);
  SetEPRxStatus(ENDP4, EP_RX_VALID);
  SetEPTxStatus(ENDP4, EP_TX_DIS);

  /* Set this device to response on default address */
  SetDeviceAddress(0);
  
  bDeviceState = ATTACHED;
}

/*******************************************************************************
* Function Name  : Midi_CDC_Device_SetConfiguration.
* Description    : Update the device state to configured.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Midi_CDC_Device_SetConfiguration(void)
{
  DEVICE_INFO *pInfo = &Device_Info;

  if (pInfo->Current_Configuration != 0)
  {
    /* Device configured */
    bDeviceState = CONFIGURED;
  }
}

/*******************************************************************************
* Function Name  : Midi_CDC_Device_SetConfiguration.
* Description    : Update the device state to addressed.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Midi_CDC_Device_SetDeviceAddress (void)
{
  bDeviceState = ADDRESSED;
}

/*******************************************************************************
* Function Name  : Midi_CDC_Device_Status_In.
* Description    : Midi CDC Device Status In Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Midi_CDC_Device_Status_In(void)
{
  if (Request == SET_LINE_CODING)
  {
    Request = 0;
  }
}

/*******************************************************************************
* Function Name  : Midi_CDC_Device_Status_Out
* Description    : Midi CDC Device Status OUT Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Midi_CDC_Device_Status_Out(void)
{}

/*******************************************************************************
* Function Name  : Midi_CDC_Device_Data_Setup
* Description    : handle the data class specific requests
* Input          : Request Nb.
* Output         : None.
* Return         : USB_UNSUPPORT or USB_SUCCESS.
*******************************************************************************/
RESULT Midi_CDC_Device_Data_Setup(uint8_t RequestNo)
{
  uint8_t    *(*CopyRoutine)(uint16_t);

  CopyRoutine = NULL;

  if (RequestNo == GET_LINE_CODING)
  {
    if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
    {
      CopyRoutine = Midi_CDC_Device_GetLineCoding;
    }
  }
  else if (RequestNo == SET_LINE_CODING)
  {
    if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
    {
      CopyRoutine = Midi_CDC_Device_SetLineCoding;
    }
    Request = SET_LINE_CODING;
  }

  if (CopyRoutine == NULL)
  {
    return USB_UNSUPPORT;
  }

  pInformation->Ctrl_Info.CopyData = CopyRoutine;
  pInformation->Ctrl_Info.Usb_wOffset = 0;
  (*CopyRoutine)(0);
  return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : Midi_CDC_Device_NoData_Setup.
* Description    : handle the no data class specific requests.
* Input          : Request Nb.
* Output         : None.
* Return         : USB_UNSUPPORT or USB_SUCCESS.
*******************************************************************************/
RESULT Midi_CDC_Device_NoData_Setup(uint8_t RequestNo)
{

  if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
  {
    if (RequestNo == SET_COMM_FEATURE)
    {
      return USB_SUCCESS;
    }
    else if (RequestNo == SET_CONTROL_LINE_STATE)
    {
      return USB_SUCCESS;
    }
  }

  return USB_UNSUPPORT;
}

/*******************************************************************************
* Function Name  : Midi_CDC_Device_GetDeviceDescriptor.
* Description    : Gets the device descriptor.
* Input          : Length.
* Output         : None.
* Return         : The address of the device descriptor.
*******************************************************************************/
uint8_t *Midi_CDC_Device_GetDeviceDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Device_Descriptor);
}

/*******************************************************************************
* Function Name  : Midi_CDC_Device_GetConfigDescriptor.
* Description    : get the configuration descriptor.
* Input          : Length.
* Output         : None.
* Return         : The address of the configuration descriptor.
*******************************************************************************/
uint8_t *Midi_CDC_Device_GetConfigDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Config_Descriptor);
}

/*******************************************************************************
* Function Name  : Midi_CDC_Device_GetStringDescriptor
* Description    : Gets the string descriptors according to the needed index
* Input          : Length.
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
uint8_t *Midi_CDC_Device_GetStringDescriptor(uint16_t Length)
{
  uint8_t wValue0 = pInformation->USBwValue0;
  if (wValue0 >= 4)
  {
    return NULL;
  }
  else
  {
    return Standard_GetDescriptorData(Length, &String_Descriptor[wValue0]);
  }
}

/*******************************************************************************
* Function Name  : Midi_CDC_Device_Get_Interface_Setting.
* Description    : test the interface and the alternate setting according to the
*                  supported one.
* Input1         : uint8_t: Interface : interface number.
* Input2         : uint8_t: AlternateSetting : Alternate Setting number.
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
RESULT Midi_CDC_Device_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting)
{
  if (AlternateSetting > 0)
  {
    return USB_UNSUPPORT;
  }
  else if (Interface > 1)
  {
    return USB_UNSUPPORT;
  }
  return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : Midi_CDC_Device_GetLineCoding.
* Description    : send the linecoding structure to the PC host.
* Input          : Length.
* Output         : None.
* Return         : Linecoding structure base address.
*******************************************************************************/
uint8_t *Midi_CDC_Device_GetLineCoding(uint16_t Length)
{
  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = sizeof(linecoding);
    return NULL;
  }
  return(uint8_t *)&linecoding;
}

/*******************************************************************************
* Function Name  : Midi_CDC_Device_SetLineCoding.
* Description    : Set the linecoding structure fields.
* Input          : Length.
* Output         : None.
* Return         : Linecoding structure base address.
*******************************************************************************/
uint8_t *Midi_CDC_Device_SetLineCoding(uint16_t Length)
{
  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = sizeof(linecoding);
    return NULL;
  }
  return(uint8_t *)&linecoding;
}

void InitSnStringWith64bitId(void)
{
  uint64_t id=HAL_GetHwSerialNum();
  if (id != 0)
  {
    IntToUnicode (id & 0xFFFFFFFF, &Midi_CDC_Device_StringSerial[2] , 8);
    IntToUnicode ((id>>32) & 0xFFFFFFFF, &Midi_CDC_Device_StringSerial[18], 4);
  }
}
/*******************************************************************************
* Function Name  : HexToChar.
* Description    : Convert Hex 32Bits value into char.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len)
{
  uint8_t idx = 0;
  
  for( idx = 0 ; idx < len ; idx ++)
  {
    if( ((value >> 28)) < 0xA )
    {
      pbuf[ 2* idx] = (value >> 28) + '0';
    }
    else
    {
      pbuf[2* idx] = (value >> 28) + 'A' - 10; 
    }
    
    value = value << 4;
    
    pbuf[ 2* idx + 1] = 0;
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

