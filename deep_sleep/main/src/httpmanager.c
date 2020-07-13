#include "httpmanager.h"

esp_err_t _http_event_handle(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            printf( "HTTP_EVENT_ERROR\n");
        break;
        case HTTP_EVENT_ON_CONNECTED:
            printf(  "HTTP_EVENT_ON_CONNECTED\n");
            break;
        case HTTP_EVENT_HEADER_SENT:
            printf( "HTTP_EVENT_HEADER_SENT\n");
            break;
        case HTTP_EVENT_ON_HEADER:
            printf( "HTTP_EVENT_ON_HEADER\n");
            printf("%.*s", evt->data_len, (char*)evt->data);
            break;
        case HTTP_EVENT_ON_DATA:
            printf( "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client)) {
                printf("%.*s", evt->data_len, (char*)evt->data);
            }

            break;
        case HTTP_EVENT_ON_FINISH:
            printf( "HTTP_EVENT_ON_FINISH\n");
            break;
        case HTTP_EVENT_DISCONNECTED:
            printf(  "HTTP_EVENT_DISCONNECTED\n");
            break;
    }
    return ESP_OK;
}

int http_post()
{
     char local_response_buffer[200] = {0};
    esp_http_client_config_t config = {
        .url = "http://192.168.1.5:3000/devices",
        .event_handler = _http_event_handle,
        .user_data = local_response_buffer,        // Pass address of local buffer to get response
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    const char *post_data = "{\"name\":\"device\",\"userId\":\"device_007\"}";
    esp_http_client_set_url(client, "http://192.168.1.5:3000/devices");
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, post_data, 39);
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        printf("HTTP POST Status = %d, content_length = %d\n",
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        printf("HTTP POST request failed: %s", esp_err_to_name(err));
    }
    return 0;
}

int http_delete()
{
    char device_id[24];
    getdevice_id(device_id);
    char url [57]="http://192.168.1.5:3000/devices/";
    for(int i=0;i<24;i++)
        {
            url[32+i]=device_id[i];
        }   
        for(int j=0;j<57;j++)
        printf("%c",url[j]);


    esp_http_client_config_t config = {
        .url = url,
        .event_handler = _http_event_handle,
        .user_data = device_id,        // Pass address of local buffer to get response
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    
    esp_http_client_set_method(client, HTTP_METHOD_DELETE);
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        printf("HTTP POST Status = %d, content_length = %d\n",
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        printf("HTTP POST request failed: %s", esp_err_to_name(err));
    }
    return 0;

}