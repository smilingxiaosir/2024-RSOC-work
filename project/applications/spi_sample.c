#include <rtthread.h>
#include <rtdevice.h>

#include <drv_spi.h>
#include <drv_gpio.h>

static int spi_attach(void)
{
    return rt_hw_spi_device_attach("spi2","spi21",GET_PIN(B,12));
}
//INIT_DEVICE_EXPORT(spi_attach);

static int spi_transfer_one_data(void)
{
    rt_err_t ret = RT_EOK;
    struct rt_spi_device *spi21 = (struct rt_spi_device *)rt_device_find("spi21");

    struct rt_spi_configuration cfg;
    cfg.data_width = 8;
    cfg.mode = RT_SPI_MASTER | RT_SPI_MODE_0 |RT_SPI_MSB;
    cfg.max_hz = 1 * 1000 * 1000;
    rt_spi_configure(spi21,&cfg);

    rt_uint8_t sendbuff = 0xDA;
    rt_uint8_t recvBuff = 0xF1;
    ret = rt_spi_transfer(spi21, &sendbuff, &recvBuff, 1);
    rt_kprintf("ret = %d\n",ret);
    rt_kprintf("接受到的值为：%d\n",recvBuff);

}
MSH_CMD_EXPORT(spi_transfer_one_data,spi_transfer_one_data);

static int spi_send_one_data(void)
{
    rt_err_t ret = RT_EOK;
    struct rt_spi_device *spi21 = (struct rt_spi_device *)rt_device_find("spi21");

    struct rt_spi_configuration cfg;
    cfg.data_width = 8;
    cfg.mode = RT_SPI_MASTER | RT_SPI_MODE_0 |RT_SPI_MSB;
    cfg.max_hz = 1 * 1000 * 1000;
    rt_spi_configure(spi21,&cfg);

    rt_uint8_t sendbuff = 0x11;
    ret = rt_spi_send(spi21, &sendbuff, 1);
    rt_kprintf("ret = %d\n",ret);

}
MSH_CMD_EXPORT(spi_send_one_data,spi_send_one_data);

static int spi_recv_one_data(void)
{
    rt_err_t ret = RT_EOK;
    struct rt_spi_device *spi21 = (struct rt_spi_device *)rt_device_find("spi21");

    struct rt_spi_configuration cfg;
    cfg.data_width = 8;
    cfg.mode = RT_SPI_MASTER | RT_SPI_MODE_0 |RT_SPI_MSB;
    cfg.max_hz = 1 * 1000 * 1000;
    rt_spi_configure(spi21,&cfg);

    rt_uint8_t recvbuff = 0x45;
    ret = rt_spi_recv(spi21, &recvbuff, 1);
    rt_kprintf("ret = %d\n",ret);
    rt_kprintf("接受到的值为：%d\n",recvbuff);

}
MSH_CMD_EXPORT(spi_recv_one_data,spi_recv_one_data);

static int spi_send_then_send_one_data(void)
{
    rt_err_t ret = RT_EOK;
    struct rt_spi_device *spi21 = (struct rt_spi_device *)rt_device_find("spi21");

    struct rt_spi_configuration cfg;
    cfg.data_width = 8;
    cfg.mode = RT_SPI_MASTER | RT_SPI_MODE_0 |RT_SPI_MSB;
    cfg.max_hz = 1 * 1000 * 1000;
    rt_spi_configure(spi21,&cfg);

    rt_uint8_t sendbuff1[2] = {0x1A, 0x99};
    rt_uint8_t sendbuff2[2] = {0x12, 0x22};
    ret = rt_spi_send_then_send(spi21, &sendbuff1, 2, &sendbuff2, 2);
    rt_kprintf("ret = %d\n",ret);

}
MSH_CMD_EXPORT(spi_send_then_send_one_data,spi_send_then_send_one_data);

static int spi_send_then_recv_one_data(void)
{
    rt_err_t ret = RT_EOK;
    struct rt_spi_device *spi21 = (struct rt_spi_device *)rt_device_find("spi21");

    struct rt_spi_configuration cfg;
    cfg.data_width = 8;
    cfg.mode = RT_SPI_MASTER | RT_SPI_MODE_0 |RT_SPI_MSB;
    cfg.max_hz = 1 * 1000 * 1000;
    rt_spi_configure(spi21,&cfg);

    rt_uint8_t sendbuff1[2] = {0x1A, 0x99};
    rt_uint8_t recvbuff2[2] = {0x12, 0x22};
    ret = rt_spi_send_then_recv(spi21, &sendbuff1, 2, &recvbuff2, 2);
    rt_kprintf("ret = %d\n",ret);
    rt_kprintf("接受到的值为：%d和%d\n",sendbuff1[0],sendbuff1[1]);

}
MSH_CMD_EXPORT(spi_send_then_recv_one_data,spi_send_then_recv_one_data);