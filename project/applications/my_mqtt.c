#include "rtthread.h"
#include "dev_sign_api.h"
#include "mqtt_api.h"
#include <aht10.h>
#include <stdlib.h>
#include <cJSON.h>
char DEMO_PRODUCT_KEY[IOTX_PRODUCT_KEY_LEN + 1] = {0};
char DEMO_DEVICE_NAME[IOTX_DEVICE_NAME_LEN + 1] = {0};
char DEMO_DEVICE_SECRET[IOTX_DEVICE_SECRET_LEN + 1] = {0};

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

//定义
int Temp, Humi;
#define AHT_I2C_BUS_NAME "i2c3"

rt_thread_t  tid1 = NULL;    //启动MQTT传输的线程
rt_thread_t  tid2 = NULL;    //获取温湿度的线程


static void example_message_arrive(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    iotx_mqtt_topic_info_t     *topic_info = (iotx_mqtt_topic_info_pt) msg->msg;

    switch (msg->event_type) {
        case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
            /* print topic name and topic message */
            EXAMPLE_TRACE("Message Arrived:");
            EXAMPLE_TRACE("Topic  : %.*s", topic_info->topic_len, topic_info->ptopic);
            EXAMPLE_TRACE("Payload: %.*s", topic_info->payload_len, topic_info->payload);
            EXAMPLE_TRACE("\n");
            break;
        default:
            break;
    }
}

//订阅消息，以便于接收云平台发送过来的消息
static int example_subscribe(void *handle)
{
    int res = 0;
    const char *fmt = "/%s/%s/user/get";
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

//向云平台发布消息
static int example_publish(void *handle)
{
    int             res = 0;
    const char     *fmt = "/sys/%s/%s/thing/event/property/post";
    char           *topic = NULL;
    int             topic_len = 0;
    char           *payload = NULL;

    cJSON* root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "id", 1722740027879);
    cJSON* temper = cJSON_CreateObject();
    cJSON_AddNumberToObject(temper, "temp", Temp);
    cJSON_AddNumberToObject(temper, "hum", Humi);
    cJSON_AddItemToObject(root, "params", temper); 
    cJSON_AddStringToObject(root, "version", "1.0");
    cJSON_AddStringToObject(root, "method", "thing.event.property.post");
    payload = cJSON_Print(root);


    topic_len = strlen(fmt) + strlen(DEMO_PRODUCT_KEY) + strlen(DEMO_DEVICE_NAME) + 1;
    topic = HAL_Malloc(topic_len);
    if (topic == NULL) {
        EXAMPLE_TRACE("memory not enough");
        return -1;
    }
    memset(topic, 0, topic_len);
    HAL_Snprintf(topic, topic_len, fmt, DEMO_PRODUCT_KEY, DEMO_DEVICE_NAME);

    res = IOT_MQTT_Publish_Simple(0, topic, IOTX_MQTT_QOS0, payload, strlen(payload));
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

static void example_event_handle(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    EXAMPLE_TRACE("msg->event_type : %d", msg->event_type);
}




static int mqtt_example_main(int argc, char *argv[])
{
    void                   *pclient = NULL;
    int                     res = 0;
    int                     loop_cnt = 0;
    iotx_mqtt_param_t       mqtt_params;

    HAL_GetProductKey(DEMO_PRODUCT_KEY);
    HAL_GetDeviceName(DEMO_DEVICE_NAME);
    HAL_GetDeviceSecret(DEMO_DEVICE_SECRET);

    EXAMPLE_TRACE("mqtt example");

    
    memset(&mqtt_params, 0x0, sizeof(mqtt_params));

    
    mqtt_params.handle_event.h_fp = example_event_handle;

    pclient = IOT_MQTT_Construct(&mqtt_params);
    if (NULL == pclient) {
        EXAMPLE_TRACE("MQTT construct failed");
        return -1;
    }

    res = example_subscribe(pclient);
    if (res < 0) {
        IOT_MQTT_Destroy(&pclient);
        return -1;
    }

    while (1) {
        if (0 == loop_cnt % 20) {
            //rt_sem_take(dynamic_sem1, RT_WAITING_FOREVER);
            example_publish(pclient);
            rt_thread_mdelay(1000);
        }

        IOT_MQTT_Yield(pclient, 200);

        loop_cnt += 1;
    }

    return 0;
}


void MQTT_Thread(void)
{
    tid1 = rt_thread_create("tid1", mqtt_example_main, RT_NULL, 4096, 15, 5);
    if(tid1 != NULL)
    {
        rt_thread_startup(tid1);
    }
}

void Get_Degree(void *p)
{
    aht10_device_t AHT = aht10_init(AHT_I2C_BUS_NAME);
    while (1)
    {
        Humi = aht10_read_humidity(AHT);
        Temp = aht10_read_temperature(AHT);

        lcd_show_string(20, 50, 24, "TEMPERATURE:");
        lcd_show_num(20, 100, (int) Temp, sizeof(Temp), 24);
        lcd_show_string(50, 100, 24, "centigrade");
        lcd_show_string(20, 150, 24, "HUMIDITY:");
        lcd_show_num(20, 200, (int) Humi, sizeof(Humi), 24);
        lcd_show_string(50, 200, 24 , "%%");
        rt_thread_mdelay(1000);
    }
    
}

void Temp_Hum_Create_thread(void)
{
    tid2 = rt_thread_create("tid2", Get_Degree, RT_NULL, 2048, 25, 5);
    if(tid2 != NULL)
    {
        rt_thread_startup(tid2);
    }
    
}

