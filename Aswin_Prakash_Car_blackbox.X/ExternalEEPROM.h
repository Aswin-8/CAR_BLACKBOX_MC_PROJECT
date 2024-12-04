/* 
 * File:   ds1307.h
 */

#ifndef EXTERNALEEPROM_H
#define	EXTERNALEEPROM_H

#define EEPROM_SLAVE_WRITE             0b10100000 // 0xA0
#define EEPROM_SLAVE_READ              0b10100001 // 0xA1




unsigned char read_externaleeprom(unsigned char addr);
void write_externaleeprom(unsigned char addr, char data);
void str_write_externaleeprom(unsigned char addr, char *data);

#endif	/* DS1307_H */

