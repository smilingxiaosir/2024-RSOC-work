#include <rtthread.h>
#include <rtdevice.h>
#include <drv_gpio.h>
#include <ulog.h>
#include <aht10.h>
#include <beep.h>
#include "dev_sign_api.h"
#include "mqtt_api.h"

#define LOG_TAG "my_work.app"
#define LOG_LVL LOG_LVL_DBG
#include <ulog.h>

static char DEMO_PRODUCT_KEY[IOTX_PRODUCT_KEY_LEN + 1] = {0};
static char DEMO_DEVICE_NAME[IOTX_DEVICE_NAME_LEN + 1] = {0};
static char DEMO_DEVICE_SECRET[IOTX_DEVICE_SECRET_LEN + 1] = {0};

void *HAL_Malloc(uint32_t size);
void HAL_Free(void *ptr);
void HAL_Printf(const char *fmt, ...);
int HAL_GetProductKey(char product_key[IOTX_PRODUCT_KEY_LEN + 1]);
int HAL_GetDeviceName(char device_name[IOTX_DEVICE_NAME_LEN + 1]);
int HAL_GetDeviceSecret(char device_secret[IOTX_DEVICE_SECRET_LEN]);
uint64_t HAL_UptimeMs(void);
int HAL_Snprintf(char *str, const int len, const char *fmt, ...);

#define EXAMPLE_TRACE(fmt, ...)  \
    do { \
        HAL_Printf("%s|%03d :: ", __func__, __LINE__); \
        HAL_Printf(fmt, ##__VA_ARGS__); \
        HAL_Printf("%s", "\r\n"); \
    } while(0)

static void example_message_arrive(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    iotx_mqtt_topic_info_t *topic_info = (iotx_mqtt_topic_info_pt) msg->msg;

    switch (msg->event_type) {
        case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
            EXAMPLE_TRACE("Message Arrived:");
            EXAMPLE_TRACE("Topic  : %.*s", topic_info->topic_len, topic_info->ptopic);
            EXAMPLE_TRACE("Payload: %.*s", topic_info->payload_len, topic_info->payload);
            EXAMPLE_TRACE("\n");
            break;
        default:
            break;
    }
}

static int example_subscribe(void *handle)
{
    int res = 0;
    const char *fmt = "/sys/%s/%s/thing/event/property/post_reply";
    char *topic = NULL;
    int topic_len = 0;

    topic_len = strlen(fmt) + strlen(DEMO_PRODUCT_KEY) + strlen(DEMO_DEVICE_NAME) + 1;
    topic = HAL_Malloc(topic_len);
    if (topic == NULL) {
        EXAMPLE_TRACE("memory not enough");
        return -1;
    }
    memset(topic, 0, topic_len);
    HAL_Snprintf(topic, topic_len, fmt, DEMO_PRODUCT_KEY, DEMO_DEVICE_NAME);

    res = IOT_MQTT_Subscribe(handle, topic, IOTX_MQTT_QOS0, example_message_arrive, NULL);
    if (res < 0) {
        EXAMPLE_TRACE("subscribe failed");
        HAL_Free(topic);
        return -1;
    }

    HAL_Free(topic);
    return 0;
}

static void example_event_handle(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    EXAMPLE_TRACE("msg->event_type : %d", msg->event_type);
}

// 按钮计数器
static int num = 1;

// 定义按钮引脚
#define KEY_LEFT GET_PIN(C, 0)
// 定义蜂鸣器引脚
#define BUZZER GET_PIN(B, 0)
// 定义 AHT10 I2C 设备名称
#define AHT10_I2C_BUS "i2c3"

// 定义信号量和线程句柄
static rt_sem_t key_sem = RT_NULL;
static aht10_device_t dev = RT_NULL;
static rt_thread_t ger_temp_hum_thread = RT_NULL;
static rt_thread_t send_aliyun_thread = RT_NULL;
static rt_mailbox_t MbForTempAndHumidity = RT_NULL;
static rt_sem_t key_ger_send = RT_NULL;
static void *pclient = NULL;
static int res = 0;
static iotx_mqtt_param_t mqtt_params;

// 定义数据结构体
typedef struct Data {
    float temperature;
    float humidity;
} Data, *Data_t;

void init_data(Data_t data, float _temperature, float _humidity) {
    data->temperature = _temperature;
    data->humidity = _humidity;
}


// static int example_publish(void *handle)
// {
//     int             res = 0;
//     const char     *fmt = "/%s/%s/user/get";
//     char           *topic = NULL;
//     int             topic_len = 0;
//     char           *payload = "{\"message\":\"hello!\"}";

//     topic_len = strlen(fmt) + strlen(DEMO_PRODUCT_KEY) + strlen(DEMO_DEVICE_NAME) + 1;
//     topic = HAL_Malloc(topic_len);
//     if (topic == NULL) {
//         EXAMPLE_TRACE("memory not enough");
//         return -1;
//     }
//     memset(topic, 0, topic_len);
//     HAL_Snprintf(topic, topic_len, fmt, DEMO_PRODUCT_KEY, DEMO_DEVICE_NAME);

//     res = IOT_MQTT_Publish_Simple(0, topic, IOTX_MQTT_QOS0, payload, strlen(payload));
//     if (res < 0) {
//         EXAMPLE_TRACE("publish failed, res = %d", res);
//         HAL_Free(topic);
//         return -1;
//     }

//     HAL_Free(topic);
//     return 0;
// }



// MQTT发布函数
static int example_publish(void *handle, Data_t data) {
    int res = 0;
    const char *fmt = "/sys/%s/%s/thing/event/property/post";
    char *topic = NULL;
    int topic_len = 0;
    char *payload = NULL;

    // 组装 负载
    payload = HAL_Malloc(54);
    memset(payload,0,54);
    HAL_Snprintf(payload, 54, "{\"params\":{\"temperature\":\"%0.2f\",\"Humidity\":\"%0.2f\"}}", data->temperature, data->humidity);


    //组装主题
    topic_len = strlen(fmt) + strlen(DEMO_PRODUCT_KEY) + strlen(DEMO_DEVICE_NAME) + 1;
    topic = HAL_Malloc(topic_len);
    if (topic == NULL) {
        EXAMPLE_TRACE("memory not enough");
        return -1;
    }
    memset(topic, 0, topic_len);
    HAL_Snprintf(topic, topic_len, fmt, DEMO_PRODUCT_KEY, DEMO_DEVICE_NAME);

    res = IOT_MQTT_Publish_Simple(handle, topic, IOTX_MQTT_QOS0, payload, strlen(payload));
    if (res < 0) {
        EXAMPLE_TRACE("publish failed, res = %d", res);
        HAL_Free(topic);
        HAL_Free(payload);
        return -1;
    }

    HAL_Free(topic);
    HAL_Free(payload);
    return 0;
}

// 发送数据到阿里云线程函数
void send_aliyun(void *param) {
    while (1) {
        rt_sem_take(key_ger_send, RT_WAITING_FOREVER);

        HAL_GetProductKey(DEMO_PRODUCT_KEY);
        HAL_GetDeviceName(DEMO_DEVICE_NAME);
        HAL_GetDeviceSecret(DEMO_DEVICE_SECRET);

        memset(&mqtt_params, 0x0, sizeof(mqtt_params));
        mqtt_params.handle_event.h_fp = example_event_handle;
        if(pclient == NULL){
        pclient = IOT_MQTT_Construct(&mqtt_params);
        }
        if (NULL == pclient) {
            EXAMPLE_TRACE("MQTT construct failed");
            return;
        }

        res = example_subscribe(pclient);
        if (res < 0) {
            IOT_MQTT_Destroy(&pclient);
            return;
        }

        while (1) {
            Data_t data_aliyun = (Data_t)rt_malloc(sizeof(Data));
            if ((rt_mb_recv(MbForTempAndHumidity, (rt_uint32_t *)&data_aliyun, RT_WAITING_FOREVER)) == RT_EOK) {

                LOG_I("接受的指针地址,%x",data_aliyun);
                LOG_E("temp:\t%.2f\thum:\t%.2f",data_aliyun->temperature,data_aliyun->humidity);



                // example_publish(pclient, data_aliyun);
                example_publish(pclient,data_aliyun);
            }
            rt_free(data_aliyun);
        }
    }
}

// 温湿度获取线程函数
static void ger_temp_hum(void *param) {
    float temperature = 0.0;
    float humidity = 0.0;
    int flag = (int)param;

    if (flag == 1) {
        while (1) {
            temperature = aht10_read_temperature(dev);
            humidity = aht10_read_humidity(dev);
            Data_t data = (Data_t)rt_malloc(sizeof(Data));
            init_data(data, temperature, humidity);
            LOG_I("发送的指针地址,%x",data);
            LOG_I("温度: %0.2f 湿度: %0.2f", temperature, humidity);
            if ((rt_mb_urgent(MbForTempAndHumidity, (rt_uint32_t)data)) != RT_EOK) {

                //读取一个并释放一个
                Data_t temp = (Data_t)rt_malloc(sizeof(Data));
                rt_mb_recv(MbForTempAndHumidity, (rt_uint32_t *)&temp, RT_WAITING_FOREVER);
                rt_free(temp);
                rt_mb_urgent(MbForTempAndHumidity, (rt_uint32_t)data);
            }
            rt_thread_mdelay(500);
        }
    } else {
        LOG_I("线程挂起，等待下一次按钮激活");
    }
}

// 按键回调函数
void key_left_callback(void *args) {
    rt_sem_release(key_sem);
    LOG_I("按键左被按下!");
}

// 配置按键中断
void key_call_set() {
    rt_pin_mode(KEY_LEFT, PIN_MODE_INPUT_PULLUP);
    rt_pin_attach_irq(KEY_LEFT, PIN_IRQ_MODE_FALLING, key_left_callback, RT_NULL);
    rt_pin_irq_enable(KEY_LEFT, PIN_IRQ_ENABLE);
}

// 主线程入口函数
static void thread_entry(void *param) {
    while (1) {
        rt_sem_take(key_sem, RT_WAITING_FOREVER);

        if (ger_temp_hum_thread == RT_NULL) {
            if (dev == RT_NULL) {
                dev = aht10_init(AHT10_I2C_BUS);
            }
            if (MbForTempAndHumidity == RT_NULL) {
                MbForTempAndHumidity = rt_mb_create("MbForTempAndHumidity",1024, RT_IPC_FLAG_FIFO);
            }
            if (key_ger_send == RT_NULL) {
                key_ger_send = rt_sem_create("ger_send", 0, RT_IPC_FLAG_FIFO);
            }

            ger_temp_hum_thread = rt_thread_create(
                "ger", ger_temp_hum, (void *)num, 1024, 25, 5);
            rt_thread_startup(ger_temp_hum_thread);

            send_aliyun_thread = rt_thread_create(
                "send", send_aliyun, RT_NULL,5120, 26, 5);
            rt_thread_startup(send_aliyun_thread);

            num = 0;
            rt_sem_release(key_ger_send);
        } else {
            if (dev != RT_NULL) {
                aht10_deinit(dev);
                dev = RT_NULL;
            }
            if (MbForTempAndHumidity != RT_NULL) {
                rt_mb_delete(MbForTempAndHumidity);
                MbForTempAndHumidity = RT_NULL;
            }
            rt_thread_delete(ger_temp_hum_thread);
            rt_thread_delete(send_aliyun_thread);
            if (key_ger_send != RT_NULL) {
                rt_sem_delete(key_ger_send);
                key_ger_send = RT_NULL;
            }
            if (num == 0) {
                ger_temp_hum_thread = rt_thread_create(
                    "aliyun", ger_temp_hum, (void *)num, 1024, 25,5);
                rt_thread_startup(ger_temp_hum_thread);
                num = 1;
            }
            ger_temp_hum_thread = RT_NULL;
            send_aliyun_thread = RT_NULL;
        }
    }
}

// 初始化函数
void my_work() {
    key_sem = rt_sem_create("key_sem", 0, RT_IPC_FLAG_PRIO);
    if (key_sem == RT_NULL) {
        LOG_E("创建信号量失败");
        return;
    }

    key_call_set();

    rt_thread_t main_thread = rt_thread_create(
        "main_thread", thread_entry, RT_NULL, 1024, 25, 5);
    if (main_thread != RT_NULL) {
        rt_thread_startup(main_thread);
    } else {
        LOG_E("创建主线程失败");
    }
}

// 导出 my_work 命令
MSH_CMD_EXPORT(my_work, my_work);
