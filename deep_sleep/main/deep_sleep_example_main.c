/* Deep sleep wake up example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"
#include "esp_log.h"
#include "esp32/ulp.h"
#include "driver/touch_pad.h"
#include "driver/adc.h"
#include "driver/rtc_io.h"
#include "soc/sens_periph.h"
#include "soc/rtc.h"
#include "nvs_flash.h"
#include  "nvs.h"

#include "Bluetooth.h"
#include "memorymanager.h"
#include "wifimanager.h"
#include "httpmanager.h"
#include "sensormanager.h"

#define READ_TIME_SEC 120

static RTC_DATA_ATTR struct timeval sleep_enter_time;







void app_main(void)
{
    const int wakeup_time_sec = 20;
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    // Isolate GPIO12 pin from external circuits. This is needed for modules
    // which have an external pull-up resistor on GPIO12 (such as ESP32-WROVER)
    // to minimize current consumption.
    rtc_gpio_isolate(GPIO_NUM_12);

    isregistred();
    printf("THRESHOLD %d\n",getthresh());
    
    struct timeval now;
    gettimeofday(&now, NULL);
    if(!(sleep_enter_time.tv_usec>0))
    gettimeofday(&sleep_enter_time, NULL);

    int sleep_time_ms = (now.tv_sec - sleep_enter_time.tv_sec);

    switch (esp_sleep_get_wakeup_cause()) 
        {

            case ESP_SLEEP_WAKEUP_TIMER: 
                printf("TIME IS %d sec\n",sleep_time_ms);
                if(sleep_time_ms>READ_TIME_SEC && isregistred())
                    {
                        get_hr();
                        get_temp();
                        get_lux();

                        if(get_soil()>getthresh())
                        {
                            wifi_init_sta();
                            http_post();
                            printf("HTTP POST THRESHOLD ERROR\n");

                        }
                        sleep_enter_time.tv_usec=0;
                    }
            break;
        


            case ESP_SLEEP_WAKEUP_UNDEFINED:
            break;

            default:
                printf("Not a deep sleep reset\n");
            break;
        }



    vTaskDelay(1000 / portTICK_PERIOD_MS);
    
    printf("bluetooth init...\n");
    bluetooth_init();

    printf("Enabling timer wakeup, %ds\n", wakeup_time_sec);
    esp_sleep_enable_timer_wakeup(wakeup_time_sec * 1000000);
    printf("Entering deep sleep\n");
    esp_deep_sleep_start();
}



int init_device()
{
     esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    // Isolate GPIO12 pin from external circuits. This is needed for modules
    // which have an external pull-up resistor on GPIO12 (such as ESP32-WROVER)
    // to minimize current consumption.
    rtc_gpio_isolate(GPIO_NUM_12);
 return 0;
}

