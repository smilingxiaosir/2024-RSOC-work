// #include <rtthread.h>
// #include <rtdevice.h>

// #if defined(BSP_USING_TEST)

// #define DRV_DEBUG
// #define LOG_TAG     "drv.test"
// #include <drv_log.h>
// static rt_err_t dev_test_init(rt_device_t dev)
// {
//     LOG_T("test dev init");
//     return RT_EOK;
// }
// static rt_err_t dev_test_open(rt_device_t dev,rt_uint16_t oflag)
// {
//     LOG_I("test dev open flag = %d",oflag);
//     return RT_EOK;
// }
// static rt_err_t dev_test_close(rt_device_t dev)
// {
//     LOG_I("test dev close");
//     return RT_EOK;
// }



// int rt_device_test_init(void)
// {
//     rt_device_t test_dev = rt_device_create(RT_Device_Class_Char,0);
//     if(!test_dev)
//     {
//         LOG_E("test_dev create failed!\n");
//         return -RT_ERROR;
//     }

//     test_dev->init = dev_test_init;
//     test_dev->open = dev_test_open;
//     test_dev->close = dev_test_close;

//     if(rt_device_register(test_dev,"test_dev",RT_DEVICE_FLAG_RDWR) != RT_EOK)
//     {
//         LOG_E("test dev register failed.");
//         return -RT_ERROR;
//     }
//     return RT_EOK;
// }
// INIT_BOARD_EXPORT(rt_device_test_init);

// #endif