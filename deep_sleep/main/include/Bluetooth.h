#ifndef BLUETOOTH_H
#define BLUETOOTH_H


#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include <sys/time.h>


#include "esp_log.h"
#include "esp_event.h"

#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"

#include "memorymanager.h"



#define SPP_TAG "POT_SENSATION_DEVICE"
#define SPP_SERVER_NAME "SPP_SERVER"
#define EXAMPLE_DEVICE_NAME "POT_SENSATION_DEVICE"

#define NON_REGISTRED_DEVICE 84
#define REGISTRED_DEVICE 85
#define ID_RESET_ALL_DATA 42
#define ID_REGISTER_DEVICE 43
#define ID_SET_NEW_THRESH 44

static const esp_spp_mode_t esp_spp_mode = ESP_SPP_MODE_CB;
static const esp_spp_sec_t sec_mask = ESP_SPP_SEC_AUTHENTICATE;
static const esp_spp_role_t role_slave = ESP_SPP_ROLE_SLAVE;



bool bluetooth_init();
void  bluetooth_handle(esp_spp_cb_param_t *param);
void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);
void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param);

#endif