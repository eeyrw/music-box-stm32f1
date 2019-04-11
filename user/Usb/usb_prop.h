/**
  ******************************************************************************
  * @file    usb_prop.h
  * @author  MCD Application Team
  * @version V4.1.0
  * @date    26-May-2017
  * @brief   All processing related to Midi CDC Device Demo (Endpoint 0)
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


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __usb_prop_H
#define __usb_prop_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
typedef struct
{
  uint32_t bitrate;
  uint8_t format;
  uint8_t paritytype;
  uint8_t datatype;
}LINE_CODING;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/

#define Midi_CDC_Device_GetConfiguration          NOP_Process
//#define Midi_CDC_Device_SetConfiguration          NOP_Process
#define Midi_CDC_Device_GetInterface              NOP_Process
#define Midi_CDC_Device_SetInterface              NOP_Process
#define Midi_CDC_Device_GetStatus                 NOP_Process
#define Midi_CDC_Device_ClearFeature              NOP_Process
#define Midi_CDC_Device_SetEndPointFeature        NOP_Process
#define Midi_CDC_Device_SetDeviceFeature          NOP_Process
//#define Midi_CDC_Device_SetDeviceAddress          NOP_Process

#define SEND_ENCAPSULATED_COMMAND   0x00
#define GET_ENCAPSULATED_RESPONSE   0x01
#define SET_COMM_FEATURE            0x02
#define GET_COMM_FEATURE            0x03
#define CLEAR_COMM_FEATURE          0x04
#define SET_LINE_CODING             0x20
#define GET_LINE_CODING             0x21
#define SET_CONTROL_LINE_STATE      0x22
#define SEND_BREAK                  0x23

/* Exported functions ------------------------------------------------------- */
void Midi_CDC_Device_init(void);
void Midi_CDC_Device_Reset(void);
void Midi_CDC_Device_SetConfiguration(void);
void Midi_CDC_Device_SetDeviceAddress (void);
void Midi_CDC_Device_Status_In (void);
void Midi_CDC_Device_Status_Out (void);
RESULT Midi_CDC_Device_Data_Setup(uint8_t);
RESULT Midi_CDC_Device_NoData_Setup(uint8_t);
RESULT Midi_CDC_Device_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting);
uint8_t *Midi_CDC_Device_GetDeviceDescriptor(uint16_t );
uint8_t *Midi_CDC_Device_GetConfigDescriptor(uint16_t);
uint8_t *Midi_CDC_Device_GetStringDescriptor(uint16_t);

uint8_t *Midi_CDC_Device_GetLineCoding(uint16_t Length);
uint8_t *Midi_CDC_Device_SetLineCoding(uint16_t Length);

void InitSnStringWith64bitId(void);
void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len);

#endif /* __usb_prop_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

