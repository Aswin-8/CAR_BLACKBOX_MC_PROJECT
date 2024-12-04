/*Name :Aswin Prakash
 * Date :31 October 2024
 * DESCRIPTION : CAR BLACK BOX PROJECT
 * File:   car_black_box.c
 */

#include <stdio.h>
#include <stdlib.h>
#include "main.h"

unsigned char clock_reg[3],sec,return_time;
char time[7];  // "HH:MM:SS"
char log[11];  //HHMMSSeventspeed
int pos = -1;            //initialising pos to -1
unsigned char *menu[]={"View Log","Clear Log","Download Log","Set Time","Change passwd"};
unsigned char menu_pos=0;

extern unsigned char long_press_delay;

void get_time(void)
{
    clock_reg[0] = read_ds1307(HOUR_ADDR); // HH -> BCD 
    clock_reg[1] = read_ds1307(MIN_ADDR); // MM -> BCD 
    clock_reg[2] = read_ds1307(SEC_ADDR); // SS -> BCD 
    
    //BCD Format 
    //clock_reg[0] = HH
    //clock_reg[1] = MM
    //clock_reg[2] = SS
   
    /* To store the time in HH:MM:SS format */
    
    // HH -> 
    time[0] = ((clock_reg[0] >> 4) & 0x03) + '0';
    time[1] = (clock_reg[0] & 0x0F) + '0';
    
  
    // MM 
    time[2] = ((clock_reg[1] >> 4) & 0x07) + '0';
    time[3] = (clock_reg[1] & 0x0F) + '0';
    
  
    // SS
    time[4] = ((clock_reg[2] >> 4) & 0x07) + '0';
    time[5] = (clock_reg[2] & 0x0F) + '0';
    time[6] = '\0';
}

void display_time(void)
{
    get_time();
    clcd_putch(time[0],LINE2(2));
    clcd_putch(time[1],LINE2(3));
    clcd_putch(':',LINE2(4));                      //displaying by character by character on CLCD the time
    clcd_putch(time[2],LINE2(5));
    clcd_putch(time[3],LINE2(6));
    clcd_putch(':',LINE2(7));
    clcd_putch(time[4],LINE2(8));
    clcd_putch(time[5],LINE2(9));    
}

void display_dashboard(unsigned char event[],unsigned char speed)
{   
    clcd_print("TIME     E  SP",LINE1(2));
    display_time();
    //display event
    clcd_print (event,LINE2(11));
    
    //display speed
    clcd_putch(speed/10+'0',LINE2(14));
    clcd_putch(speed%10+'0',LINE2(15));  
}

unsigned char  menu_screen(unsigned char key)
{
    static unsigned char delay=0,flag1=0,flag2=0;
    if( key==SW5)             //checking if  SW5 is pressed
    {
        flag1=1;              //setting the flag
        return_time=5;        //making return time to 5
        if(delay++>30)
            delay=0;
    }
    else if(key==ALL_RELEASED &&  flag1==1 && menu_pos < 4 && delay!=0)
    {
        clear_screen();
        menu_pos++;            //incrementing menu pos
        long_press_delay=0;
        flag1=0;
        delay=0;
    }
    if( key==SW4)             //checking if  SW4 is pressed
    {
        flag2=1;
        return_time=5;        //making return time to 5
        if(delay++>30)
           delay=0;
    }
   else if(key == ALL_RELEASED && menu_pos > 0 && flag2==1 && delay!=0)
    {
        clear_screen();
        menu_pos--;           //decrementing menu_pos
        long_press_delay=0;
        flag2=0;
        delay=0;
    }
    if(menu_pos==4)          //checking menu_pos is at 4th pos
    {
        clcd_putch('*',LINE2(1));
        clcd_print (menu[menu_pos-1],LINE1(2));                //printing the options
        clcd_print (menu[menu_pos],LINE2(2));
    }
    else
    {
        clcd_putch('*',LINE1(1));
        clcd_print (menu[menu_pos],LINE1(2));                   //printing the options
        clcd_print (menu[menu_pos+1],LINE2(2));
    }
    if(return_time==0)                                 //checking if return time reaches zero
        return RETURN_BACK;                            //going back to dash board window
} 

void download_log(void)
{
    static char index1=0 ,once=0;
    unsigned char download_data[11],z;
    if(index1==0  && pos !=-1)                    //checking if index is zero and pos not equal to -1
    {
        uart_puts("Logs : \n");                     //transfering the message using uart
        once=0;
    }
    if(pos==-1 && once==0)                      //checking if pso=-1 meaning there are no logs
    {
        uart_puts("No Logs.");
        once=1;
    }
    while(index1 <= pos)
    {
        for(z=5+(10*index1);z<16+(10*index1);z++)
        {
            download_data[z-(5+(10*index1))]=read_externaleeprom(z); //reading data from EEPROM and storing
        } 
        uart_putchar(index1+'0');
        uart_putchar (' ');
        uart_putchar (download_data[0]);
        uart_putchar (download_data[1]);
        uart_putchar (':');
        uart_putchar (download_data[2]);
        uart_putchar (download_data[3]);                  //transfering different index logs using uart to display
        uart_putchar (':');
        uart_putchar (download_data[4]);
        uart_putchar (download_data[5]);
        uart_putchar (' ');
        uart_putchar (download_data[6]);
        uart_putchar (download_data[7]);
        uart_putchar (' ');
        uart_putchar (download_data[8]);
        uart_putchar (download_data[9]);
        uart_putchar('\n');
        index1++;
    }   
}

void clear_log(void)
{
    pos=-1;                               //making pos to -1 to clear the logs
    clcd_print ("Logs Cleared",LINE1(2));
    clcd_print ("Successfully",LINE2(2));           //printing the message on clcd
}

unsigned char set_time(unsigned char key)
{
    static char sec_flag=0,min_flag=0,hour_flag=0,time_pos=0,delay=0,display=0,sw_flag=0;
    static unsigned char delay1=0,flag5=0,flag6=0;
    if(delay++==2)
    {
        display=1;
        delay=0;               //delay for blinking
    }
    if( key == SW5)             //checking if  SW5 is pressed
    {
        flag5=1;
        if(delay1++>20)
        {
            delay1=0;
            clear_screen();
            clcd_print ("Time Changed",LINE1(2));          //if switch press is long displaying success message
            clcd_print ("Successfully",LINE2(2));
            __delay_ms(1500);
            clear_screen();
            return DASH_BOARD;                           //returning DASH BOARD
        }
    }
    else if(key==ALL_RELEASED &&  flag5==1 && time_pos < 3 && delay1!=0)
   {
        time_pos++;                                      //changing between hr,sec and min
        if(time_pos==3)
            time_pos=0;
        flag5=0;
        delay1=0;
   }
    if( key == SW4)             //checking if  SW5 is pressed
    {
        flag6=1;
        if(delay1++>20)
        {
            delay1=0;
            clear_screen();
            clcd_print ("Time Changed",LINE1(2));  //printing success message
            clcd_print ("Successfully",LINE2(2));
            __delay_ms(1500);
            clear_screen();
            return MAIN_MENU;
        }
    }
    else if(key==ALL_RELEASED &&  flag6==1 && sw_flag==0 && delay1!=0)
    {
        sw_flag=1;
        flag6=0;
        delay1=0;
    }
    clcd_print ("Time (HH:MM:SS)",LINE1(0));                    //printing time format
    if(time_pos==0 )
    {
        clcd_putch(time[0],LINE2(0));                       //printing hr and min part
        clcd_putch(time[1],LINE2(1));  
        clcd_putch(':',LINE2(2));
        clcd_putch(time[2],LINE2(3));
        clcd_putch(time[3],LINE2(4));  
        clcd_putch(':',LINE2(5));
        if(display)
        {
            __delay_us(100000);
            clcd_putch(time[4],LINE2(6));                //blinking secs part
            clcd_putch(time[5],LINE2(7));
            display=0;
        }
        else
        {
            __delay_us(100000);
            clcd_putch('\0',LINE2(6));                   //printing null character if display is off to blink
            clcd_putch('\0',LINE2(7));
        }
        if(sw_flag)
        {
            if(time[4]=='5' && time[5]=='9')
            {
                time[5]='0';               //resetting sec field if it reaches 59
                time[4]='0';
                sec_flag=1;
            }
            if(time[5]=='9')
            {
                time[5]='0';              //updating one's field to zero once reached 9 
                if( time[4] < '5')
                    time[4]++;
                sec_flag=1;
            }
            if(time[5] >= '0' && time[5]<'9' && !sec_flag )
            {
                time[5]++;               //incrementing secs
            }
            sec_flag=0;
            sw_flag=0;
        }
    }
    else if(time_pos==1)
    {
        clcd_putch(time[0],LINE2(0));
        clcd_putch(time[1],LINE2(1));  
        clcd_putch(':',LINE2(2));
        clcd_putch(time[4],LINE2(6));
        clcd_putch(time[5],LINE2(7));                    //printing hr and secs field
        clcd_putch(':',LINE2(5));
        if(display)
        {
            __delay_us(100000);
            clcd_putch(time[2],LINE2(3));
            clcd_putch(time[3],LINE2(4));                 //blinking min field
            display=0;
        }
        else
        {
            __delay_us(100000);
            clcd_putch('\0',LINE2(3));
            clcd_putch('\0',LINE2(4));
        }
        if(sw_flag)
        {
            if(time[2]=='5' && time[3]=='9')
            {
                time[2]='0';
                time[3]='0';                   //resetting min field once reached 59
                min_flag=1;
            }
            if(time[3]=='9')
            {
                time[3]='0';                    //resetting one's field
                if( time[2] < '5')
                    time[2]++;
                min_flag=1;
            }
            if(time[3] >= '0' && time[3]<'9' && !min_flag )
            {
                time[3]++;                     //incrementing min field
            }
        }
        sw_flag=0;                         //resetting flags
        min_flag=0;
    }
    else if(time_pos==2)
    { 
         clcd_putch(':',LINE2(2));
        clcd_putch(time[2],LINE2(3));
        clcd_putch(time[3],LINE2(4));
        clcd_putch(':',LINE2(5));
        clcd_putch(time[4],LINE2(6));               //printing min and sec field
        clcd_putch(time[5],LINE2(7)); 
        if(display)
        {
            __delay_us(100000);
            clcd_putch(time[0],LINE2(0));
            clcd_putch(time[1],LINE2(1));               //blinking hr field
            display=0;
        }
        else
        {
            __delay_us(100000);
            clcd_putch('\0',LINE2(0));
            clcd_putch('\0',LINE2(1));
        }
        if(sw_flag)
        {
            if(time[0]=='2' && time[1]=='3')
            {
                time[0]='0';
                time[1]='0';                      //resetting back to zero once reached 23
                hour_flag=1;
            }
            if(time[1]=='9')
            {
                time[1]='0';                     //resetting one's field
                if( time[0] < '2')
                    time[0]++;              //increasing ten's part
                hour_flag=1;
            }
            if(time[1] >= '0' && time[1]<'9' && !hour_flag )
            {       
                time[1]++;                              //increasing one's part
            }
        }
        hour_flag=0;
        sw_flag=0;
    }    
        clock_reg[2]=((time[4] & 0x07) << 4) | (time[5] & 0x0F);
        write_ds1307(SEC_ADDR, clock_reg[2]);
        clock_reg[1]=((time[2] & 0x07) << 4) | (time[3] & 0x0F);          //writing the updated time to RTC
        write_ds1307(MIN_ADDR, clock_reg[1]);
        clock_reg[0]=((time[0] & 0x03) << 4) | (time[1] & 0x0F);
        write_ds1307(HOUR_ADDR, clock_reg[0]); 
        return RESET_NOTHING;
}

void view_log(unsigned char key)
{
    unsigned char log_data[11],k;
    static index=0;
    if(key == SW4)                 //checking if SW4 is pressed and index less than ort equal topos
    {
        if(index <= pos)
        {
            index++;                                  //incrementing index value
        if(index>pos)
            index=0;                                  //rolling back to zero
        }
    }
    else if(key == SW5)               //checking if SW5 is pressed and index greater than 0
    {
                                        
        if(index >= 0)
        {
            index--;                                  //decrementing index value
        if(index<0)
            index=pos;                                //rolling back to pos
        }
    }
    if(pos == -1)
    {
        clcd_print ("No Logs present",LINE1(0));              //if pos=-1 printing no logs message
        return;
    }
    for(k=5+(10*index);k<16+(10*index);k++)
    {
      log_data[k-(5+(10*index))]=read_externaleeprom(k);     //storing log data
    }
    clcd_print ("# TIME     E  SP",LINE1(0));
    clcd_putch (index+'0',LINE2(0));
    clcd_putch (log_data[0],LINE2(2));
    clcd_putch (log_data[1],LINE2(3));
    clcd_putch(':',LINE2(4));
    clcd_putch (log_data[2],LINE2(5));
    clcd_putch (log_data[3],LINE2(6));              //displaying the log data on CLCD
    clcd_putch(':',LINE2(7));
    clcd_putch (log_data[4],LINE2(8));
    clcd_putch (log_data[5],LINE2(9));
    clcd_putch (log_data[6],LINE2(11));
    clcd_putch (log_data[7],LINE2(12));
    clcd_putch (log_data[8],LINE2(14));
    clcd_putch (log_data[9],LINE2(15));    
}

unsigned char change_password(unsigned char key,unsigned char reset_flag)
{   
    static char new_passwd[4],re_passwd[4];   
    static int once,p=0,q=0;
    if(reset_flag==NEW_PASSWORD)
    {
        once=1;
        new_passwd[0]='\0';
        new_passwd[1]='\0';             //initializing new password with null character
        new_passwd[2]='\0';
        new_passwd[3]='\0';
        
    } 
    if(key == SW4 && p <4)
    {
        new_passwd[p]='1';                      //storing value in new password
        p++;                                    //incrementing p
        clcd_putch('*',LINE2(p+4));           
    }
    else if (key == SW5 && p <4)
    {
        new_passwd[p]='0'; 
        p++;                                //if SW5 is pressed 0 is stored in new password
        clcd_putch('*',LINE2(p+4));
    }
    if(once==1 && p < 4)
    {
        once=0;
    
        clear_screen();
        clcd_print ("Enter new Passwd",LINE1(0));             //displaying the field to enter new password
        clcd_write(LINE2(4), INST_MODE);
        clcd_write(DISP_ON_AND_CURSOR_ON,INST_MODE);
        __delay_us(100);
    }
    else if(p == 4 && once==0)
    {
        key=ALL_RELEASED;
        once=2;
        clear_screen();                                        //re entering the new password
        clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
        __delay_us(100);
        clcd_print ("Re Enter new Pas",LINE1(0));
    }
    if(key == SW4 && q <4 && once==2)
    {
        re_passwd[q]='1'; 
        q++;
        clcd_putch('*',LINE2(q+4));
    }
    else if (key == SW5 && q <4 && once==2)
    {
        re_passwd[q]='0';                                       //storing the re entered password
        q++;
        clcd_putch('*',LINE2(q+4));
    }
    if(q == 4)
    {
        p=0,q=0;
        if(strncmp(re_passwd,new_passwd,4)==0)                    //comparing new password and re entered password
        {
            clear_screen();
            clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
            __delay_us(100);
            str_write_externaleeprom(0x00,new_passwd);            
            clcd_print ("Password Changed",LINE1(0));             //if they are same printing success message at storing new password in EEPROM
            clcd_print ("Successfully",LINE2(2));
            __delay_ms(2000);
            return RE_PASSWORD;
        }
        else
        {
            clear_screen();
            clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
             __delay_us(100);
            clcd_print ("Password Mismatch",LINE1(0));           //if they are not same printing mismatch message
             __delay_ms(1500);
            return RE_PASSWORD;
        }   
    }    
    return RETURN_NOTHING;
}

void log_event_eeprom()
{
    char addr=0x05 ;                     //initializing address
    pos++;                               //incrementing position
    if(pos == 10)
    {
        pos=0;
    }
    addr= 5 +  (10*pos);                    //updating address
    str_write_externaleeprom(addr,log);     //writing log in External EEPROM at particular address
}

unsigned char login_screen(unsigned char key,unsigned char reset_flag)
{
    static char user_passwd[4];
    static unsigned char i,attempt_rem;
    if(reset_flag == RESET_PASSWORD)
    {
        user_passwd[0]='\0';
        user_passwd[1]='\0';
        user_passwd[2]='\0';                       //initializing user password with null character
        user_passwd[3]='\0';
        i=0;
        attempt_rem=3;
        key=ALL_RELEASED;
        return_time=5;
    }
    if(return_time==0)                                
        return RETURN_BACK;                                //if return time is zero returning to DASH BOARD
    if(key == SW4 && i <4)
    {
        user_passwd[i]='1'; 
        i++;
        clcd_putch('*',LINE2(i+4));                          //if SW 4 is pressed storing 1 in user password
        return_time=5;
    }
    else if (key == SW5 && i <4)
    {
        user_passwd[i]='0'; 
        i++;                                             //if SW 5 is pressed storing 0 in user password
        clcd_putch('*',LINE2(i+4));
        return_time=5;
    }
    if(i == 4)
    {
        unsigned char spasswd[4];
        for(unsigned char j=0; j<4 ;j++)                 //when i reaches 4 reading password from EEPROM
        {
            spasswd[j]=read_externaleeprom(j);
        }
        if(strncmp(user_passwd,spasswd,4)==0)           //checking if the password matches
        {
            clear_screen();
            clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
             __delay_us(100);
            clcd_print("Login Success",LINE1(0));            //if password matches printing success message
            __delay_ms(1500); 
            return LOGIN_SUCCESS;
        }
        else
        {
            attempt_rem--;                                  //reducing attempts for every wrong password
            if(attempt_rem == 0)
            {
                clear_screen();
                clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                __delay_us(100);
                clcd_print("You are blocked",LINE1(0));
                clcd_print("wait for ",LINE2(0));          //if attempts becomes 0 screen is blocked for 60 sec
                clcd_print("secs",LINE2(12));
                sec=60;
                while(sec!=0)
                {
                    clcd_putch(sec/10 +'0',LINE2(9));              //displaying time remaining
                    clcd_putch(sec%10 +'0',LINE2(10));
                }
                attempt_rem=3;                                   //re initializing attempt to 3
            }
            else
            {
                clear_screen();
                clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                __delay_us(100);
                clcd_print("Wrong Password",LINE1(0));         //displaying wrong password msg for mismatch
                clcd_print("Attempts left",LINE2(0));           
                clcd_putch(attempt_rem +'0',LINE2(14));
                __delay_ms(2000);
            }
            clear_screen();
            clcd_print("Enter Password",LINE1(1));
            clcd_write(LINE2(4), INST_MODE);                   //Displaying screen for enter password
            clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
            __delay_us(100);
            i=0;
            return_time=5;
        }
    }
}

void log_event(unsigned char event[],unsigned char speed)
{
    get_time();
    strncpy(log,time,6);                         //copying time in first 6 bytes
    strncpy(&log[6],event,2);                    //copying event and speed in next 4 bytes
    log[8]=speed/10+'0';
    log[9]=speed%10+'0';
    log[10]='\0';                                  
    log_event_eeprom();                        //storing every activity in EEPROM
}

void clear_screen()
{
    clcd_write(CLEAR_DISP_SCREEN, INST_MODE);           //function to clear screen
    __delay_us(500);
}


