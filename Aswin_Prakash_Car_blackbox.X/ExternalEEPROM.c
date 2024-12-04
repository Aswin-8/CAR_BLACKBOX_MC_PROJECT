/*Name :Aswin Prakash
 * Date :31 October 2024
 * DESCRIPTION : CAR BLACK BOX PROJECT
 * File:   ExternalEEPROM.c
 */
#include <xc.h>
#include "i2c.h"
#include "ExternalEEPROM.h"


unsigned char read_externaleeprom(unsigned char addr)
{
    unsigned char data;
    
    i2c_start();
    i2c_write(EEPROM_SLAVE_WRITE);
    i2c_write(addr);
    i2c_rep_start();
    i2c_write(EEPROM_SLAVE_READ);
    data = i2c_read(0);
    i2c_stop();
    
    return data;
}

void write_externaleeprom(unsigned char addr, char data) // SEc_ADDR, data
{
    i2c_start();
    i2c_write(EEPROM_SLAVE_WRITE);
    i2c_write(addr);
    i2c_write(data);
    i2c_stop();
}

void str_write_externaleeprom(unsigned char addr, char *data)
{
    while(*data!=0)
    {
        write_externaleeprom( addr,  *data);
        data++;
        addr++;
    }
}
