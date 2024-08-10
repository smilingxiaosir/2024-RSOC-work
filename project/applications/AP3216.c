#include <board.h>
#include <rtthread.h>
#include <drv_gpio.h>
#include <rtdevice.h>
#include <ap3216c.h>
#include <board/ports/led_matrix/drv_matrix_led.h>


#define LOG_TAG     "ap3216.app"
#define LOG_LVL     LOG_LVL_DBG
#include <ulog.h>

#define THREAD_PRIORITY 25
#define THREAD_STACK_SIZE 4096
#define THREAD_TIMESLICE 5

/* define LED */
enum{
    EXTERN_LED_0,
    EXTERN_LED_1,
    EXTERN_LED_2,
    EXTERN_LED_3,
    EXTERN_LED_4,
    EXTERN_LED_5,
    EXTERN_LED_6,
    EXTERN_LED_7,
    EXTERN_LED_8,
    EXTERN_LED_9,
    EXTERN_LED_10,
    EXTERN_LED_11,
    EXTERN_LED_12,
    EXTERN_LED_13,
    EXTERN_LED_14,
    EXTERN_LED_15,
    EXTERN_LED_16,
    EXTERN_LED_17,
    EXTERN_LED_18,
};


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
        //读接近感应值
        ps_data = ap3216c_read_ps_data(dev);
        if(ps_data > 500)
        {
            for (int i = EXTERN_LED_0; i <= EXTERN_LED_18; i++)
            {
                Set_LEDColor(i, GREEN);
                RGB_Reflash();
            }
        }
        else
        {
            LOG_D("current ps data : %d.",ps_data);
            for (int i = EXTERN_LED_0; i <= EXTERN_LED_18; i++)
            {
                Set_LEDColor(i, DARK);
                RGB_Reflash();
            }
            
        }
        rt_thread_mdelay(2000);
    }
   
    

}

    