/* 
 * File:   main.h
 * Author: Aswin
 *
 * Created on 21 October, 2024, 10:31 AM
 */

#ifndef MAIN_H
#define	MAIN_H

#include <xc.h>
#include "i2c.h"
#include "ds1307.h"
#include "digital_keypad.h"
#include "adc.h"
#include "clcd.h"
#include "car_black_box.h"
#include "ExternalEEPROM.h"
#include "timers.h"
#include "uart.h"
#include <string.h>

#define DASH_BOARD     0x00
#define LOGIN          0x01
#define MAIN_MENU      0x02


#define RESET_PASSWORD 0x10
#define RESET_NOTHING  0x20
#define RETURN_NOTHING 0x30

#define RETURN_BACK    0x25
#define LOGIN_SUCCESS  0x26

#define VIEW_LOG        0x15
#define DOWNLOAD_LOG    0x16
#define CLEAR_LOG       0x17
#define SET_TIME        0x18
#define CHANGE_PASSWORD 0x21
#define DISPLAY_MSG     0x19
#define RE_PASSWORD     0x33
#define NEW_PASSWORD    0x34

#endif	/* MAIN_H */

