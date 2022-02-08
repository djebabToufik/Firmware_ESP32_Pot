#ifndef SENSORMANAGER_H
#define SENSORMANAGER_H


#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include <sys/time.h>

#include "driver/gpio.h"
#include "driver/adc.h"
#include "driver/i2c.h"

#define HR_T_SLAVE_ADDR 0x40 
#define LUX_SLAVE_ADDR 0x40 
#define WRITE_BIT I2C_MASTER_WRITE              /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ                /*!< I2C master read */
#define ACK_CHECK_EN 0x1                        /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0                       /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0                             /*!< I2C ack value */
#define NACK_VAL 0x1    
#define I2C_MASTER_NUM 1
#define I2C_MASTER_SDA_IO 18
#define I2C_MASTER_SCL_IO 19
#define I2C_MASTER_FREQ_HZ 40000

#define DATA_LENGTH 512  



int get_soil();
int get_hr();
int get_temp();
int get_lux();



#endif