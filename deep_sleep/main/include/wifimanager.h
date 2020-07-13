#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#define EXAMPLE_ESP_WIFI_SSID      "o2-SchoenhausHH3.OGre" 
#define EXAMPLE_ESP_WIFI_PASS      "5099759481318351"
#define EXAMPLE_ESP_MAXIMUM_RETRY  2
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include <sys/time.h>
#include "esp_log.h"
#include "esp_event.h"

#include "esp_wifi.h"
#include "esp_wps.h"


void wifi_init_sta(void);


#endif