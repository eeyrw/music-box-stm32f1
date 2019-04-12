//
// Created by YangYongbao on 2017/3/18.
//

#ifndef STM32F10X_MAKEFILE_FREERTOS_UART_LOG_H
#define STM32F10X_MAKEFILE_FREERTOS_UART_LOG_H

#include <stdarg.h>
#include <stdio.h>


void uart_log_init(void);
void uart_putchar(unsigned int c);
int uart_getchar(void);
#ifdef USE_CUSTOM_PRINTF
void debug(const char *format, ...);
#else
#define debug(f_, ...) printf((f_), ##__VA_ARGS__)
#endif

#endif //STM32F10X_MAKEFILE_FREERTOS_UART_LOG_H
