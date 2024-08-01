/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-5-10      ShiHao       first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <aht10.h>

#define DBG_TAG "main"
#define DBG_LVL         DBG_LOG
#include <rtdbg.h>

/* 配置 LED 灯引脚 */
//#define PIN_LED_B              GET_PIN(F, 11)      // PF11 :  LED_B        --> LED
//#define PIN_LED_R              GET_PIN(F, 12)      // PF12 :  LED_R        --> LED

#define AHT_I2C_BUS_NAME "i2c3"

void AHT_TEST(void)
{
    unsigned int count = 1;

    aht10_device_t AHT = aht10_init(AHT_I2C_BUS_NAME);

    float Temp, Humi;

    while (count > 0)
    {

        Humi = aht10_read_humidity(AHT);
        Temp = aht10_read_temperature(AHT);

        rt_kprintf("Tem: %.2f\n",Temp);
        rt_kprintf("Humi: %.2f %%\n",Humi);
        rt_thread_mdelay(1000);
        count++;
    }
}
MSH_CMD_EXPORT(AHT_TEST,AHT_TEST);