#include "Bluetooth.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"


#define SPP_TAG "POT_SENSATION_DEVICE"
#define SPP_SERVER_NAME "SPP_SERVER"
#define EXAMPLE_DEVICE_NAME "POT_SENSATION_DEVICE"
#define NON_REGISTRED_DEVICE 84
#define REGISTRED_DEVICE 85
static const esp_spp_mode_t esp_spp_mode = ESP_SPP_MODE_CB;
static const esp_spp_sec_t sec_mask = ESP_SPP_SEC_AUTHENTICATE;
static const esp_spp_role_t role_slave = ESP_SPP_ROLE_SLAVE;
void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param);

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













