/*
   TBD
   djebab.toufik@gmail.com
*/
/*Standard libraries */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
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


/*Bluetooth configuration*/

#define SPP_TAG "POT_SENSATION_DEVICE"
#define SPP_SERVER_NAME "SPP_SERVER"
#define EXAMPLE_DEVICE_NAME "POT_SENSATION_DEVICE"
//Data for first memory case to tell if Device is registred
//This data is processesd in init_device()
#define NON_REGISTRED_DEVICE 84
#define REGISTRED_DEVICE 85
static const esp_spp_mode_t esp_spp_mode = ESP_SPP_MODE_CB;
static const esp_spp_sec_t sec_mask = ESP_SPP_SEC_AUTHENTICATE;
static const esp_spp_role_t role_slave = ESP_SPP_ROLE_SLAVE;

/***/
char store_data[51];
char read_data[51];
uint8_t  *pstore_data = (uint8_t *) &store_data;
uint8_t  *pread_data = (uint8_t *) &read_data;
char first_memory_case[2];

/*
Functions prototypes 
*/
void  bluetooth_handle(esp_spp_cb_param_t *param);  /*Toufik Djebab */
int init_device(); /*Toufik Djebab */
static void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);
void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param);








void app_main(void)
{
     
/*Bluetooth config and enabling*/
    esp_err_t ret = nvs_flash_init();
     
   
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));
    
    
    /*Initialize and Enable the Controller*/
       esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    if ((ret = esp_bt_controller_init(&bt_cfg)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s initialize controller failed: %s\n", __func__, esp_err_to_name(ret));   
        return;
    }
    if ((ret = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s enable controller failed: %s\n", __func__, esp_err_to_name(ret));    
        return;
    }
    /*Initialize and Enable the Host*/
     if ((ret = esp_bluedroid_init()) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s initialize bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
         
        return;
    }
    if ((ret = esp_bluedroid_enable()) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s enable bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
        
        return;
    }


    if ((ret = esp_bt_gap_register_callback(esp_bt_gap_cb)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s gap register failed: %s\n", __func__, esp_err_to_name(ret));
        
        return;
    }

    if ((ret = esp_spp_register_callback(esp_spp_cb)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s spp register failed: %s\n", __func__, esp_err_to_name(ret));
         
        return;
    }

    if ((ret = esp_spp_init(esp_spp_mode)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s spp init failed: %s\n", __func__, esp_err_to_name(ret));
         
        return;
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


init_device();
    

 
}

void  bluetooth_handle(esp_spp_cb_param_t *param)
{
    /*
    Message Format is Device_id (24 char), User_ID (24 char), Threshold (3char)
    Example of typical message : Function_ID,5eae9931b0f4b0e109b8b117,Toufik_Djebab,135
    Function_ID: #define Reset_All_Data 0
                 #define Register_Device 1
                 #define Set_New_THRESH 2

    Other function_IDS might be added
    Values between 84 and 100 are reserved for internal use 
    Internal non-volatile memory is organized as described below:
    Memory case : 0x00                         0x01-0x52 
                  Tag for registred device     Device_ID,User_ID and threshold 
     */
    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");
    assert(partition != NULL);
       // Read back the data, checking that read data and written data match
    ESP_ERROR_CHECK(esp_partition_read(partition, 0, read_data, 24));
  // assert(memcmp(store_data, read_data, sizeof(read_data)) == 0);
   
       
            
            printf("printf stored data are %s\n",pread_data);
            
        
     
    printf("hey\n");
     for(int i=0;i<24;i++)
        {
            store_data[i]=param->data_ind.data[i];
            
        }
        printf("printf received data are %s \n",pstore_data);
      
  // Erase entire partition
    memset(read_data, 0xFF, sizeof(read_data));
    ESP_ERROR_CHECK(esp_partition_erase_range(partition, 0, partition->size));

    // Write the data, starting from the beginning of the partition
    ESP_ERROR_CHECK(esp_partition_write(partition, 0, store_data, sizeof(store_data)));
  
        
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
            ESP_ERROR_CHECK(esp_partition_read(partition, 0, read_data, 1));  
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
                            ESP_ERROR_CHECK(esp_partition_write(partition, 0, first_memory_case,1));
                            return NON_REGISTRED_DEVICE;

                    break;
            }
            printf("Stored data are %s\n",pstore_data);
            printf("Data length  %d\n",(int) sizeof(read_data)/sizeof(*read_data));

}





