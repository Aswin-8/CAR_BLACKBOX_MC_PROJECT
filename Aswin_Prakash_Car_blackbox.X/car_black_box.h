/* 
 * File:   car_black_box.h
 * Author: Aswin
 *
 * Created on 21 October, 2024, 10:11 AM
 */

#ifndef CAR_BLACK_BOX_H
#define	CAR_BLACK_BOX_H

void display_dashboard(unsigned char event[],unsigned char speed);
void log_event(unsigned char event[],unsigned char speed);
void clear_screen(void);
unsigned char login_screen(unsigned char key,unsigned char reset_flag);
unsigned char menu_screen(unsigned char key);
void view_log(unsigned char key);
void log_event_eeprom();
void download_log(void);
void get_time(void);
void clear_log(void);
unsigned char set_time(unsigned char key);
unsigned char change_password(unsigned char key,unsigned char reset_flag);
#endif

