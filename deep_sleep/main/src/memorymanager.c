#include "memorymanager.h"





bool isregistred()
{  
    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");   
    assert(partition != NULL);
    memset(internaread, REGISTRED_DEVICE, MEMORY_LENGHT);
    esp_partition_read(partition,0, internaread, MEMORY_LENGHT);  
    printf("Init_device read before switch First Values is %d \n",internaread[0]);
    switch(internaread[0])
        {
            case NON_REGISTRED_DEVICE:
                printf("NON REGISTRED DEVICE\n");
                return false;
            break;

            case REGISTRED_DEVICE:
                printf("REGISTRED DEVICE\n");
                return true;
            break;

            default:
                // Write the NON_REGISTRED_DEVICE , in the beginning of the partition to exclude all non identified functions 
                printf("DEFAULT CASE\n");
                esp_partition_erase_range(partition, 0, partition->size);
                esp_partition_write(partition, 0, "T",1);
                return false;
                break;
            }

}

int registerdevice (unsigned char read_data[MEMORY_LENGHT])
{
    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");

    memset(internaread, REGISTRED_DEVICE, 2);
    esp_partition_erase_range(partition, 0, partition->size);
    esp_partition_write(partition, 0, internaread,2);
    esp_partition_write(partition, 2, read_data,MEMORY_LENGHT);
    return 0;
}

int deregisterdevice(unsigned char read_data[MEMORY_LENGHT])
{

    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");
    memset(read_data, NON_REGISTRED_DEVICE, MEMORY_LENGHT);
    esp_partition_erase_range(partition, 0, partition->size);
    esp_partition_write(partition, 0, read_data,MEMORY_LENGHT);
    return 0;
}

int getthresh()
{
    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");   
    assert(partition != NULL);
    memset(internaread, REGISTRED_DEVICE, MEMORY_LENGHT);
    esp_partition_read(partition,TH_OFFSET, internaread, 4);
        
    switch (internaread[0])
        {
            case 48:
                threshold=0;
            break;
        
            case 49:
                threshold=1000;
            break;
        
            case 50:
                threshold=2000;
            
            break;
        
            case 51:
                threshold=3000;
            break;

            case 52:
                threshold=4000;
            break;
        
            default:
                threshold=0;
            break;
        }
          switch (internaread[1])
        {
            case 48:
                threshold=threshold+0;
            break;
        
            case 49:
                threshold=threshold+100;
            break;
        
            case 50:
                threshold=threshold+200;
            
            break;
        
            case 51:
                threshold=threshold+300;
            break;

            case 52:
                threshold=threshold+400;
            break;
            case 53:
                threshold=threshold+500;
            break;
        
            case 54:
                threshold=threshold+600;
            break;
        
            case 55:
                threshold=threshold+700;
            
            break;
        
            case 56:
                threshold=threshold+800;
            break;

            case 57:
                threshold=threshold+900;
            break;
        
            default:
                threshold=0;
            break;
        }
            switch (internaread[2])
        {
            case 48:
                threshold=threshold+0;
            break;
        
            case 49:
                threshold=threshold+10;
            break;
        
            case 50:
                threshold=threshold+20;
            
            break;
        
            case 51:
                threshold=threshold+30;
            break;

            case 52:
                threshold=threshold+40;
            break;
            case 53:
                threshold=threshold+50;
            break;
        
            case 54:
                threshold=threshold+60;
            break;
        
            case 55:
                threshold=threshold+70;
            
            break;
        
            case 56:
                threshold=threshold+80;
            break;

            case 57:
                threshold=threshold+90;
            break;
        
            default:
                threshold=0;
            break;
        }

    return threshold;
}

int getdevice_id(char device_id[24])
    {
        const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");   
        assert(partition != NULL);
        memset(device_id, REGISTRED_DEVICE, MEMORY_LENGHT);
        esp_partition_read(partition,4, device_id, ID_OFFSET+ID_LENGHT);
        return 0;   
    }