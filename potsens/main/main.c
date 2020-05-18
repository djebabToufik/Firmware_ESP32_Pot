/*
   TBD
   djebab.toufik@gmail.com
*/
//--------------------------includes-------------------
/*Standard libraries */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_event.h"
/*Non-volatile memory headers */
#include <string.h>
#include <assert.h>
#include "esp_partition.h"
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"
/*Bluetooth headers */
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"
/*time and system headers */
#include "time.h"
#include "sys/time.h"
#include "esp_event.h"
/*ADC headers*/
#include <driver/adc.h>
/*wifi heades*/
#include "esp_wifi.h"
#include "esp_wps.h"
/*HTTP*/
#include "esp_http_client.h"

#include "function.h"



//------------------------Defines--------------------------------

/*Bluetooth configuration*/

#define SPP_TAG "POT_SENSATION_DEVICE"
#define SPP_SERVER_NAME "SPP_SERVER"
#define EXAMPLE_DEVICE_NAME "POT_SENSATION_DEVICE"
static const esp_spp_mode_t esp_spp_mode = ESP_SPP_MODE_CB;
static const esp_spp_sec_t sec_mask = ESP_SPP_SEC_AUTHENTICATE;
static const esp_spp_role_t role_slave = ESP_SPP_ROLE_SLAVE;

/*Data for first memory case to tell if Device is registred
This data is processesd in init_device()*/
#define NON_REGISTRED_DEVICE 84
#define REGISTRED_DEVICE 85
/*Function IDS Defines for Phone device Protocol*/
#define ID_RESET_ALL_DATA 42
#define ID_REGISTER_DEVICE 43
#define ID_SET_NEW_THRESH 44
/*Defines for ADC Conversion*/
#define ADC_READ_TIME_S 3600 
/*wifi defines*/
/*set wps mode via project configuration */
#define EXAMPLE_ESP_WIFI_SSID      "o2-SchoenhausHH3.OGre" 
#define EXAMPLE_ESP_WIFI_PASS      "5099759481318351"
#define EXAMPLE_ESP_MAXIMUM_RETRY  2
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1




/*
Constante and variable nedded for coding
*/
char store_data[100];
char read_data[100];
uint8_t  *pstore_data = (uint8_t *) &store_data;
uint8_t  *pread_data = (uint8_t *) &read_data;
char first_memory_case[2];
bool Registred=false;
bool *Is_Registred=&Registred;

static const char *TAG = "Pot_sensation";



static EventGroupHandle_t s_wifi_event_group;

static int s_retry_num = 0;

/*------------------------Functions prototypes ------------------*/
void  bluetooth_handle(esp_spp_cb_param_t *param);  /*Toufik Djebab */
int init_device(); /*Toufik Djebab */
bool registred_device(); /*Toufik Djebab*/
bool non_registred_device(); /*Toufik Djebab*/
bool bluetooth_init();  /*Toufik Djebab*/

/*Event handlers for bluetooth and wifi*/
static void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);
void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param);
static void event_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data);
void wifi_init_sta(void);
esp_err_t _http_event_handle(esp_http_client_event_t *evt);





/*------------------------main--------------------*/ 

void app_main(void)
{
     
     
/*NON-VOLATILE memory init */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
    //------------------------------------------------------------------------------
    printf("FIRST TEST PASSED result is %d\n",add(2,5));
bluetooth_init();
/*ADC_init()*/
adc1_config_width(ADC_WIDTH_BIT_12);
adc1_config_channel_atten(ADC1_CHANNEL_0,ADC_ATTEN_DB_0);
/*Wifi  init*/
ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
wifi_init_sta();

 char local_response_buffer[200] = {0};
    esp_http_client_config_t config = {
        .url = "http://192.168.1.5:3000/devices",
        .event_handler = _http_event_handle,
        .user_data = local_response_buffer,        // Pass address of local buffer to get response
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // GET
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %d",
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }
    ESP_LOG_BUFFER_HEX(TAG, local_response_buffer, strlen(local_response_buffer));

    // POST 39
    const char *post_data = "{\"name\":\"device\",\"userId\":\"device_007\"}";
    esp_http_client_set_url(client, "http://192.168.1.5:3000/devices");
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, post_data, 39);
    err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %d",
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    }

/*
Check if the device is registred or not
if the device is registred the device will:
    1- Read ADC Values and compare it with threshold each ADC_READ_TIME_S defined above 
        if ADC_val<threshold 
            WIFI ENABLE
            Send HTTP POST TO SERVER 
            DISABLE WIFI
        endif
    2-if BLUETOOTH_CONNECT_EVENT
        Bluetoothhandle()
    endif
    3- wait ADC_READ_TIME_S
    4-GO to 1

*/


while(true)
{
    switch(init_device())
    {
        case REGISTRED_DEVICE:
             registred_device();
                       
        break;

        case NON_REGISTRED_DEVICE:               
            
            non_registred_device();
        
        break;

    }

}
    

    

    

 
}

/*----------------end main ----------------------*/







/*Functions references*/

void  bluetooth_handle(esp_spp_cb_param_t *param)
{
    /*
    Message Format is Function_ID, Device_id (24 char), User_ID (24 char), Threshold (3char)
    Example of typical message : 1,5eae9931b0f4b0e109b8b117,Toufik_Djebab,135
    Function_ID: #define Reset_All_Data 42
                 #define Register_Device 43
                 #define Set_New_THRESH 44

    Other function_IDS might be added
    Values between 40 and 60 are reserved for internal use 
    Internal non-volatile memory is organized as described below:
    Memory case : 0x00                         0x01-0x52 
                  Tag for registred device     Device_ID,User_ID and threshold 
     */

    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");
    assert(partition != NULL);

    // Read back the data, checking that read data and written data match
    ESP_ERROR_CHECK(esp_partition_read(partition, 0, store_data, sizeof(store_data)));  
    printf("Read before the switch %s \n",pstore_data);

    switch(param->data_ind.data[0])
        {
            case ID_RESET_ALL_DATA:
                memset(read_data, NON_REGISTRED_DEVICE, sizeof(read_data));
                ESP_ERROR_CHECK(esp_partition_erase_range(partition, 0, partition->size));
                ESP_ERROR_CHECK(esp_partition_write(partition, 0, read_data, sizeof(read_data)));
                ESP_ERROR_CHECK(esp_partition_read(partition, 0, store_data, sizeof(read_data)));  
                printf("Data deleted vlues are : %s \n",pread_data);
                *Is_Registred=false;
                Registred=false;
            break;

            case ID_REGISTER_DEVICE:
            read_data[0]=REGISTRED_DEVICE;
                
                for(int i=0;i<sizeof(read_data)-1;i++)
                    {
                        read_data[i+1]=param->data_ind.data[i];
                    }
                
                printf("it should write %s \n",pread_data);
                ESP_ERROR_CHECK(esp_partition_erase_range(partition, 0, partition->size));
                ESP_ERROR_CHECK(esp_partition_write(partition, 0, read_data,sizeof(read_data)));
                memset(store_data,0xFF,sizeof(store_data));
                ESP_ERROR_CHECK(esp_partition_read(partition,0, store_data, sizeof(store_data)));  
                printf("New Registration Sotred values are : %s \n",pstore_data);
                *Is_Registred=true;
                Registred=true;
            break;

            case ID_SET_NEW_THRESH:
            printf("Case ID_SET_NEW_THRESH TBD \n");
            break;
        }   
        printf("Received data size %d \n",sizeof(pread_data)/((uint8_t) *pread_data));
      
   
    ESP_ERROR_CHECK(esp_partition_read(partition, 0, store_data, 51));  
    printf("Stored data After writing are %s \n",pstore_data);
  
        
}

static void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)

{

   
    
    switch (event) {
    case ESP_SPP_INIT_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_INIT_EVT");
        esp_bt_dev_set_device_name(EXAMPLE_DEVICE_NAME);
        esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
        esp_spp_start_srv(sec_mask,role_slave, 0, SPP_SERVER_NAME);
        break;
    case ESP_SPP_DISCOVERY_COMP_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_DISCOVERY_COMP_EVT");
        break;
    case ESP_SPP_OPEN_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_OPEN_EVT");
        break;
    case ESP_SPP_CLOSE_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CLOSE_EVT");
        break;
    case ESP_SPP_START_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_START_EVT");
        break;
    case ESP_SPP_CL_INIT_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CL_INIT_EVT");
        break;
    case ESP_SPP_DATA_IND_EVT:
      ESP_LOGI(SPP_TAG, "ESP_SPP_DATA_IND_EVT");
      bluetooth_handle(param);

        break;
    case ESP_SPP_CONG_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CONG_EVT");
        break;
    case ESP_SPP_WRITE_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_WRITE_EVT");
        break;
    case ESP_SPP_SRV_OPEN_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_SRV_OPEN_EVT"); 
        break;
    default:
        break;
    }
}

void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{
    switch (event) {
    case ESP_BT_GAP_AUTH_CMPL_EVT:{
        if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS) {
            ESP_LOGI(SPP_TAG, "authentication success: %s", param->auth_cmpl.device_name);
            esp_log_buffer_hex(SPP_TAG, param->auth_cmpl.bda, ESP_BD_ADDR_LEN);
        } else {
            ESP_LOGE(SPP_TAG, "authentication failed, status:%d", param->auth_cmpl.stat);
        }
        break;
    }
    case ESP_BT_GAP_PIN_REQ_EVT:{
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_PIN_REQ_EVT min_16_digit:%d", param->pin_req.min_16_digit);
        if (param->pin_req.min_16_digit) {
            ESP_LOGI(SPP_TAG, "Input pin code: 0000 0000 0000 0000");
            esp_bt_pin_code_t pin_code = {0};
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 16, pin_code);
        } else {
            ESP_LOGI(SPP_TAG, "Input pin code: 1234");
            esp_bt_pin_code_t pin_code;
            pin_code[0] = '1';
            pin_code[1] = '2';
            pin_code[2] = '3';
            pin_code[3] = '4';
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 4, pin_code);
        }
        break;
    }

#if (CONFIG_BT_SSP_ENABLED == true)
    case ESP_BT_GAP_CFM_REQ_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_CFM_REQ_EVT Please compare the numeric value: %d", param->cfm_req.num_val);
        esp_bt_gap_ssp_confirm_reply(param->cfm_req.bda, true);
        break;
    case ESP_BT_GAP_KEY_NOTIF_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_KEY_NOTIF_EVT passkey:%d", param->key_notif.passkey);
        break;
    case ESP_BT_GAP_KEY_REQ_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_KEY_REQ_EVT Please enter passkey!");
        break;
#endif

    default: {
        ESP_LOGI(SPP_TAG, "event: %d", event);
        break;
    }
    }
    return;
}

int init_device()
{
    
    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");
    assert(partition != NULL);
    first_memory_case[0]=NON_REGISTRED_DEVICE;
            ESP_ERROR_CHECK(esp_partition_read(partition,0, read_data, 51));  
            printf("Init_device read before switch First Values is %d \n",read_data[0]);
            switch(read_data[0])
            {
                case NON_REGISTRED_DEVICE:
                            printf("NON REGISTRED DEVICE\n");
                            return NON_REGISTRED_DEVICE;
                    break;

                case REGISTRED_DEVICE:
                             printf("REGISTRED DEVICE\n");
                             return REGISTRED_DEVICE;
                    break;

                default:
                            // Write the NON_REGISTRED_DEVICE , in the beginning of the partition to exclude all non identified functions 
                            printf("DEFAULT CASE\n");
                            ESP_ERROR_CHECK(esp_partition_erase_range(partition, 0, partition->size));
                            ESP_ERROR_CHECK(esp_partition_write(partition, 0, first_memory_case,1));
                            return NON_REGISTRED_DEVICE;

                    break;
            }
            printf("Stored data are %s\n",pstore_data);
            printf("Data length  %d\n",(int) sizeof(read_data)/sizeof(*read_data));
            


}

bool registred_device()
{
    int val = adc1_get_raw(ADC1_CHANNEL_0);
    printf("adc_raw is: %d \n",val);
    printf("1-registred device \n2-reading ADC data=%d each 5s  \n3-waitting for new bluetooth event\n--------------------------------------------------\n",val); 
    vTaskDelay(10000 / portTICK_PERIOD_MS);
   
   return false ;

    
}

bool non_registred_device()
{
    printf("non registred device \nwaitting for new bluetooth event\n------------------------------------------------------------\n");
    vTaskDelay(10000 / portTICK_PERIOD_MS);
     
   return false ;
    
}

bool bluetooth_init()

{
      ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));
    esp_err_t berr;
    
    /*Initialize and Enable the Controller*/
       esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    if ((berr = esp_bt_controller_init(&bt_cfg)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s initialize controller failed: %s\n", __func__, esp_err_to_name(berr));   
        return false;
    }
    if ((berr = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s enable controller failed: %s\n", __func__, esp_err_to_name(berr));    
        return false;
    }
    /*Initialize and Enable the Host*/
     if ((berr = esp_bluedroid_init()) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s initialize bluedroid failed: %s\n", __func__, esp_err_to_name(berr));
         
        return false;
    }
    if ((berr = esp_bluedroid_enable()) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s enable bluedroid failed: %s\n", __func__, esp_err_to_name(berr));
        
        return false;
    }


    if ((berr = esp_bt_gap_register_callback(esp_bt_gap_cb)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s gap register failed: %s\n", __func__, esp_err_to_name(berr));
        
        return false;
    }

    if ((berr = esp_spp_register_callback(esp_spp_cb)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s spp register failed: %s\n", __func__, esp_err_to_name(berr));
         
        return false;
    }

    if ((berr = esp_spp_init(esp_spp_mode)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s spp init failed: %s\n", __func__, esp_err_to_name(berr));
         
        return false;
    }

#if (CONFIG_BT_SSP_ENABLED == true)
    /* Set default parameters for Secure Simple Pairing */
    esp_bt_sp_param_t param_type = ESP_BT_SP_IOCAP_MODE;
    esp_bt_io_cap_t iocap = ESP_BT_IO_CAP_IO;
    esp_bt_gap_set_security_param(param_type, &iocap, sizeof(uint8_t));
#endif

    /*
     * Set default parameters for Legacy Pairing
     * Use variable pin, input pin code when pairing
     */
    esp_bt_pin_type_t pin_type = ESP_BT_PIN_TYPE_VARIABLE;
    esp_bt_pin_code_t pin_code;
    esp_bt_gap_set_pin(pin_type, 0, pin_code);

return true;    
}

static void event_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}


esp_err_t _http_event_handle(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER");
            printf("%.*s", evt->data_len, (char*)evt->data);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client)) {
                printf("%.*s", evt->data_len, (char*)evt->data);
            }

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}

void wifi_init_sta(void)
{
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS,
            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    /* The event will not be processed after unregister */
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
    vEventGroupDelete(s_wifi_event_group);
}