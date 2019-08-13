//
// Created by YangYongbao on 2017/3/16.
//

#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "FreeRTOS.h"
#include "task.h"
#include "uart_log.h"
#include "hal.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"

extern __IO uint8_t Receive_Buffer[64];
extern __IO uint32_t Receive_length;
extern __IO uint32_t length;
uint8_t Send_Buffer[64];
uint32_t packet_sent = 1;
uint32_t packet_receive = 1;

void Delay(__IO uint32_t nCount)
{
    for (; nCount != 0; nCount--)
        ;
}

void RCC_Configuration(void)
{
    /* GPIOA, GPIOB clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
}

void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void TIM2_IRQHandler()
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        // GPIO_ToggleBits(GPIOD, GPIO_Pin_12);
    }
}

void TIMER_Config(void)
{
    TIM_TimeBaseInitTypeDef timerInitStructure;
    timerInitStructure.TIM_Prescaler = 1;
    timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    timerInitStructure.TIM_Period = 2249;
    timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    timerInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &timerInitStructure);
    TIM_Cmd(TIM2, ENABLE);

    NVIC_InitTypeDef nvicStructure;
    nvicStructure.NVIC_IRQChannel = TIM2_IRQn;
    nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
    nvicStructure.NVIC_IRQChannelSubPriority = 1;
    nvicStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicStructure);
}

void vTaskFunction(void *pvParameters)
{
    debug("start task");
    while (1)
    {
        GPIO_ResetBits(GPIOC, GPIO_Pin_13);
        //debug("led on ");
        vTaskDelay(100);
        GPIO_SetBits(GPIOC, GPIO_Pin_13);
        //debug("led off ");
        vTaskDelay(100);
    }
}

void vTaskUsb(void *pvParameters)
{
    while (1)
    {
        if (bDeviceState == CONFIGURED)
        {
            CDC_Receive_DATA();
            /*Check to see if we have data yet */
            if (Receive_length != 0)
            {
                if (packet_sent == 1)
                    CDC_Send_DATA((unsigned char *)Receive_Buffer, Receive_length);
                Receive_length = 0;
            }
        }
    }
}
extern void SynthAsm(void *Address);
extern void stdio_setup(void);
extern void TestProcess(void);
int main()
{
    // init uart log
    stdio_setup();
    uart_log_init();
    RCC_Configuration();
    GPIO_Configuration();

    Set_System();
    Set_USBClock();
    USB_Interrupts_Config();
    USB_Init();

    debug("start main ");
    const char *pcTextForTask1 = "Task1 is running\r\n";
    TestProcess();

    xTaskCreate(vTaskFunction, "Task 1", 64, (void *)pcTextForTask1, 1, NULL);
    xTaskCreate(vTaskUsb, "Task usb", 1024, NULL, 1, NULL);

    vTaskStartScheduler();

    while (1)
        ;

    return 0;
}