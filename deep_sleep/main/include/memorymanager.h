#ifndef MM_H
#define MM_H

#define MEMORY_LENGHT 100
#define NON_REGISTRED_DEVICE 84
#define REGISTRED_DEVICE 85
#define TH_OFFSET 54
#define ID_OFFSET 2
#define ID_LENGHT 24

#include <stdbool.h>
#include "assert.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include  "nvs.h"
#include "nvs_flash.h"
#include <string.h>

#include "esp_partition.h"
#include <assert.h>

int threshold;


char internaread[MEMORY_LENGHT];
int getthresh();
bool isregistred();
int registerdevice(unsigned char read_data[MEMORY_LENGHT]);
int deregisterdevice(unsigned char read_data[MEMORY_LENGHT]);
int getdevice_id(char device_id[24]);




#endif