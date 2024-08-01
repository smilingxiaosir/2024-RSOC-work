#include <rtthread.h>
#include <rtdevice.h>
#include <drv_gpio.h>
#include <drv_lcd.h>
#include <ap3216c.h>

#define LOG_TAG     "ap3216.app"
#define LOG_LVL     LOG_LVL_DBG
#include <ulog.h>

#define GPIO_LED_B    GET_PIN(F, 11)
#define GPIO_LED_R    GET_PIN(F, 12)


int main(void)
{
    // drv_lcd_init();
    // lcd_clear(WHITE);
    // lcd_set_color(WHITE,BLACK);
    // lcd_show_string(0,0,16,"Hello RT-Thread!");

    //AP3216_Create_Thread();
    
    
    // rt_pin_mode(GPIO_LED_B, PIN_MODE_OUTPUT);

    // while (1)
    // {
    //     rt_pin_write(GPIO_LED_B, PIN_HIGH);
    //     rt_thread_mdelay(500);
    //     rt_pin_write(GPIO_LED_B, PIN_LOW);
    //     rt_thread_mdelay(500);
    // }


    






    return 0;
}