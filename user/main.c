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
#include "Player.h"

extern __IO uint8_t Receive_Buffer[64];
extern __IO uint32_t Receive_length;
extern __IO uint32_t length;
uint8_t Send_Buffer[64];
uint32_t packet_sent = 1;
uint32_t packet_receive = 1;
Player mPlayer;

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
    GPIO_SetBits(GPIOC, GPIO_Pin_13);

    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        Player32kProc(&mPlayer);
    }
    GPIO_ResetBits(GPIOC, GPIO_Pin_13);
}

void TIMER_Config(void)
{
    TIM_TimeBaseInitTypeDef timerInitStructure;
    timerInitStructure.TIM_Prescaler = 1;
    timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    timerInitStructure.TIM_Period = 1124;
    timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    timerInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &timerInitStructure);
    TIM_ClearFlag(TIM2, TIM_IT_Update);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM2, ENABLE);

    NVIC_InitTypeDef nvicStructure;
    nvicStructure.NVIC_IRQChannel = TIM2_IRQn;
    nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
    nvicStructure.NVIC_IRQChannelSubPriority = 1;
    nvicStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicStructure);
}

//TIM3 PWM部分初始化
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM3_PWM_Init(u16 arr, u16 psc)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);                        //使能定时器3时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE); //使能GPIO外设和AFIO复用功能模块时钟

    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); //Timer3部分重映射  TIM3_CH2->PB5

    //设置该引脚为复用输出功能,输出TIM3 CH2的PWM脉冲波形        GPIOB.5
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;       //TIM_CH2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure); //初始化GPIO

    //        GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);
    //设置该引脚为复用输出功能,输出TIM3 CH3的PWM脉冲波形        GPIOB.0
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;       //TIM_CH3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure); //初始化GPIO

    //初始化TIM3
    TIM_TimeBaseStructure.TIM_Period = arr;                     //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler = psc;                  //设置用来作为TIMx时钟频率除数的预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;                //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数模式
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);             //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

    //初始化TIM3 Channel2 PWM模式
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;             //选择定时器模式:TIM脉冲宽度调制模式2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     //输出极性:TIM输出比较极性高
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);                      //根据T指定的参数初始化外设TIM3 OC2
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);             //使能TIM3在CCR2上的预装载寄存器

    //初始化TIM3 Channel2 PWM模式
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;             //选择定时器模式:TIM脉冲宽度调制模式2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;     //输出极性:TIM输出比较极性高
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);                      //根据T指定的参数初始化外设TIM3 OC3
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);

    TIM_Cmd(TIM3, ENABLE); //使能TIM3
}

void vTaskFunction(void *pvParameters)
{
    debug("start task");
    while (1)
    {
        debug("led on ");
        vTaskDelay(100);
        debug("led off ");
        vTaskDelay(100);
    }
}

void vPlayTask(void *pvParameters)
{
    PlayerInit(&mPlayer);
    PlayerPlay(&mPlayer);
    TIM3_PWM_Init(1023, 0);
    TIMER_Config();
    while (1)
    {
        PlayerProcess(&mPlayer);
        //vTaskDelay(1);
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
    // TestProcess();

    xTaskCreate(vTaskFunction, "Task 1", 512, (void *)pcTextForTask1, 1, NULL);
    xTaskCreate(vTaskUsb, "Task usb", 1024, NULL, 1, NULL);
    xTaskCreate(vPlayTask, "Task play", 512, NULL, 1, NULL);
    vTaskStartScheduler();

    while (1)
        ;

    return 0;
}