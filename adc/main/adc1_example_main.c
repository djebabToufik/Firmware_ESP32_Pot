/* ADC1 Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "driver/i2c.h"

#include "esp_adc_cal.h"


void app_main(void)
{
    i2c_config_t config;
    i2c_port_t i2c_num;
    unsigned char data =0x80;
    uint8_t* add=&data;

    config.mode= I2C_MODE_MASTER;
    config.master.clk_speed=100000;
    config.sda_io_num = 18;
    config.sda_pullup_en = GPIO_PULLUP_ENABLE;
    config.scl_io_num = 19;
    config.scl_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_param_config(I2C_MODE_MASTER, &config);
    i2c_driver_install(1, config.mode, 0, 0, 0);
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  


  
        adc1_config_width(ADC_WIDTH_BIT_12);
        adc1_config_channel_atten(ADC1_CHANNEL_6,ADC_ATTEN_DB_6);
    

    //Continuously sample ADC1
    while (1) {
        
        for (int i = 0; i < 100; i++) {
        printf("ADC VALUES IS %d \n",adc1_get_raw(ADC1_CHANNEL_6)); 










         vTaskDelay(pdMS_TO_TICKS(1000));


        }

        
    }
}
