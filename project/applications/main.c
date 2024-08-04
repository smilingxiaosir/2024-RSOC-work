#include <rtthread.h>
#include <rtdevice.h>
#include <drv_gpio.h>
#include <drv_lcd.h>
#include <ap3216c.h>
#include <webclient.h>
#include <aht10.h>

#define LOG_TAG     "ap3216.app"
#define LOG_LVL     LOG_LVL_DBG
#include <ulog.h>

#define GPIO_LED_B    GET_PIN(F, 11)
#define GPIO_LED_R    GET_PIN(F, 12)

#define GET_LOCAL_URI    "https://api.seniverse.com/v3/weather/now.json?key=S6pmC8gm1FpYknJAh&location=ip&language=zh-Hans&unit=c"

int main(void)
{
    //初始化LCD屏幕
    drv_lcd_init();
    lcd_clear(WHITE);
    lcd_set_color(WHITE,BLACK);
    
    //连接WIFI
    char str[] = "wifi scan";
    char str1[] = "wifi join xiao xiao12345";
    system(str);
    system(str1);
    rt_thread_mdelay(5000);

    AP3216_Create_Thread();
    rt_thread_mdelay(2000);
    get_degree_Thread();
    rt_thread_mdelay(2000);
    MQTT_Thread();
    
    


    return 0;
}