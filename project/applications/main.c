#include <rtthread.h>
#include <rtdevice.h>
#include <drv_gpio.h>
#include <drv_lcd.h>
#include <ap3216c.h>
#include <webclient.h>
#include <aht10.h>

int main(void)
{
    //初始化LCD屏幕
    drv_lcd_init();
    lcd_clear(WHITE);
    lcd_set_color(WHITE,BLACK);
    
    wifi_connection();              //连接wifi
    rt_thread_mdelay(5000);
    AP3216_Create_Thread();         //启动AP3216c传感器
    Temp_Hum_Create_thread();       //启动温湿度传感器
    MQTT_Thread();



    return 0;
}