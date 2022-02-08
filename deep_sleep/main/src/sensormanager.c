#include "sensormanager.h"

uint8_t HR_addr=0xF5;
uint8_t T_addr=0xF3;
uint8_t lux_addr=0x01;


static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    i2c_param_config(i2c_master_port, &conf);
    return i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0);
}

static esp_err_t i2c_master_read_slave(i2c_port_t i2c_num, uint8_t *data_rd, size_t size, uint8_t address)
{
    if (size == 0) {
        return ESP_OK;
    }
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (address << 1) | READ_BIT, ACK_CHECK_EN);
    if (size > 1) {
        i2c_master_read(cmd, data_rd, size - 1, ACK_VAL);
    }
    i2c_master_read_byte(cmd, data_rd + size - 1, NACK_VAL);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 3000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}
static esp_err_t i2c_master_write_slave(i2c_port_t i2c_num, uint8_t *data_wr, size_t size, uint8_t address)
{
    int rett;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    rett=i2c_master_write_byte(cmd, (address << 1) | WRITE_BIT, ACK_CHECK_EN);
    rett=i2c_master_write(cmd, data_wr, size, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 3000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}


int get_soil()
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_6,ADC_ATTEN_DB_6);
    return adc1_get_raw(ADC1_CHANNEL_6); 
}

int get_hr()
{
    int ret;
    uint8_t *read_reg=(uint8_t *)malloc(DATA_LENGTH);

    i2c_master_init();

    ret= i2c_master_write_slave(I2C_MASTER_NUM,&HR_addr,1,HR_T_SLAVE_ADDR);
    if(ret!=0)
        {
            printf(" write trigger RH %s  \n",esp_err_to_name(ret));
        }
     else
        {
            printf("RH %s  \n",esp_err_to_name(ret));
        }
    vTaskDelay(2000/portTICK_PERIOD_MS);
    
    ret= i2c_master_read_slave(I2C_MASTER_NUM,read_reg,1,HR_T_SLAVE_ADDR);    
    if(ret!=0)
        {
            printf("RH read %s \n",esp_err_to_name(ret));
        }
    else
        {
            printf("Value RH is %s \n",read_reg);
        }

        return 0;

}

int get_temp()
{
    int ret;
    uint8_t *read_reg=(uint8_t *)malloc(DATA_LENGTH);

    i2c_master_init();

    ret= i2c_master_write_slave(I2C_MASTER_NUM,&T_addr,1,HR_T_SLAVE_ADDR);
    if(ret!=0)
        {
            printf(" write trigger temp %s  \n",esp_err_to_name(ret));
        }
     else
        {
            printf("temp %s  \n",esp_err_to_name(ret));
        }
    vTaskDelay(2000/portTICK_PERIOD_MS);
    
    ret= i2c_master_read_slave(I2C_MASTER_NUM,read_reg,2,HR_T_SLAVE_ADDR);    
    if(ret!=0)
        {
            printf("temp read %s \n",esp_err_to_name(ret));
        }
    else
        {
            printf("Value temp is %s \n",read_reg);
        }
         return 0;

}

int get_lux()
{

    int ret;
    uint8_t *read_reg=(uint8_t *)malloc(DATA_LENGTH);

    i2c_master_init();

    ret= i2c_master_write_slave(I2C_MASTER_NUM,&lux_addr,1,LUX_SLAVE_ADDR);
    if(ret!=0)
        {
            printf(" write trigger LUX %s  \n",esp_err_to_name(ret));
        }
     else
        {
            printf("lux %s  \n",esp_err_to_name(ret));
        }
    vTaskDelay(2000/portTICK_PERIOD_MS);
    
    ret= i2c_master_read_slave(I2C_MASTER_NUM,read_reg,1,LUX_SLAVE_ADDR);    
    if(ret!=0)
        {
            printf("LUX read %s \n",esp_err_to_name(ret));
        }
    else
        {
            printf("Value LUX is %s \n",read_reg);
        }
         return 0;

}