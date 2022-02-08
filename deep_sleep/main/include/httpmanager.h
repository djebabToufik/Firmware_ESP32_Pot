#ifndef HTTPMANAGER_H
#define HTTPMANAGER_H

#include "esp_http_client.h"
#include "memorymanager.h"

esp_err_t _http_event_handle(esp_http_client_event_t *evt);
int http_post();
int http_delete();

#endif