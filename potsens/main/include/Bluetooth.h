#ifndef BLUETOOTH_H
#define BLUETOOTH_H
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"

bool bluetooth_init();
void  bluetooth_handle(esp_spp_cb_param_t *param);

#endif