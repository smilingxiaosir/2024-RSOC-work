#include <board.h>
#include <rtthread.h>
#include <drv_gpio.h>
#ifndef RT_USING_NANO
#include <rtdevice.h>
#endif /* RT_USING_NANO */
#include <ap3216c.h>


#define LOG_TAG     "ap3216.app"
#define LOG_LVL     LOG_LVL_DBG
#include <ulog.h>

#define THREAD_PRIORITY 25
#define THREAD_STACK_SIZE 4096
#define THREAD_TIMESLICE 5

#define GPIO_LED_B    GET_PIN(F, 11)
#define GPIO_LED_R    GET_PIN(F, 12)





rt_thread_t AP3216_Thread = RT_NULL;
static void AP3216_entry(void *para);
void AP3216_Create_Thread(void)
{
    AP3216_Thread = rt_thread_create("AP3216_Thread",AP3216_entry,RT_NULL,THREAD_STACK_SIZE,THREAD_PRIORITY,THREAD_TIMESLICE);
    if(AP3216_Thread != NULL)
    {
        rt_thread_startup(AP3216_Thread);
    }
    else
    {
        rt_kprintf("create thread failed!");
    }
}

static void AP3216_entry(void *para)
{
    ap3216c_device_t dev;
    const char *i2c_bus_name = "i2c2";
    int count = 0;
    rt_uint16_t ps_data;
    //初始化ap3216c
    dev = ap3216c_init(i2c_bus_name);
    if(dev == RT_NULL)
    {
        LOG_E("The sensor initialize failure.");
    }
    while(1)
    {
        rt_pin_mode(GPIO_LED_B, PIN_MODE_OUTPUT);
        //读接近感应值
        ps_data = ap3216c_read_ps_data(dev);
        if(ps_data == 0)
        {
            LOG_D("object is not proximity of sensor.");
            rt_pin_write(GPIO_LED_B,PIN_HIGH);
        }
        else
        {
            LOG_D("current ps data : %d.",ps_data);
            rt_pin_write(GPIO_LED_B, PIN_LOW);
        }
        rt_thread_mdelay(2000);
    }
   
    

}

    