#ifndef BLUETOOTH_H
#define BLUETOOTH_H
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"

#define ID_RESET_ALL_DATA 42
#define ID_REGISTER_DEVICE 43
#define ID_SET_NEW_THRESH 44

bool bluetooth_init();
void  bluetooth_handle(esp_spp_cb_param_t *param);
void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);
void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param);

#endif