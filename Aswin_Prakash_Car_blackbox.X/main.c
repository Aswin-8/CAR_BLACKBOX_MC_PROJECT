/*Name :Aswin Prakash
 * Date :31 October 2024
 * DESCRIPTION : CAR BLACK BOX PROJECT
 * File:   main.c
 */

#include <xc.h>
#include "main.h"
#include "clcd.h"

#pragma config WDTE = OFF        // Watchdog Timer Enable bit (WDT disabled)

unsigned char long_press_delay=0;
extern char menu_pos,return_time;

static void init_config(void) {
    //Initialize I2C
    init_i2c(100000); //100K
    //Initialize RTC
    init_ds1307();
    
    //Initialize CLCD
    init_clcd();
    //Initialize ADC
    init_adc();
    //Initialize digital keypad
    init_digital_keypad();
    //initialize timer 2
    init_timer2();
    //initialize uart
    
    init_uart(9600);
    
    
    /* Peripheral Interrupt Enable Bit (For Timer2) */
    PEIE = 1;
    
    /* Enable all the Global Interrupts */
    GIE = 1;
    
}
void main(void) 
{   
    unsigned char control_flag=0,key,speed=0,gr=0,reset_flag,key2,delay=0;
    unsigned char event[3]="ON";
    char *gear[]={"GR","GN","G1","G2","G3","G4"};
    init_config();
    log_event(event,speed);  
    str_write_externaleeprom(0x00,"1010");                 //function to write password to eeprom and password is 1010
    while (1) 
    {
        speed=read_adc() / 10.33;  //0 to 1023 - 0 to 99
        key= read_digital_keypad(STATE);                         //short press
        key2=read_digital_keypad(LEVEL);                         //long press
        for(int delay=300;delay--;);
        
        if(key == SW1)
        {
            strcpy(event,"C ");                                    //checking if SW1 is pressed
            log_event(event,speed);
        }
        else if(key == SW2 && gr < 6)
        {
          
            strcpy(event,gear[gr]);
            log_event(event,speed);                    //checking if SW2 is pressed and up shifting gear
            gr++;
        }
        else if(key == SW3 && gr > 0)
        {
           gr--;
           strcpy(event,gear[gr]);                          //checking if SW3 is pressed and down shifting gear
           log_event(event,speed);                  
        }
        else if((key == SW4 || key == SW5 ) && control_flag == DASH_BOARD)
        {
            control_flag=LOGIN;
            clear_screen();
            clcd_print("Enter Password",LINE1(1));         //going to login screen from dash board
            clcd_write(LINE2(4), INST_MODE);
            clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
             __delay_us(100);
             reset_flag=RESET_PASSWORD;
             TMR2ON=1;
        }
        else if(control_flag == MAIN_MENU && key2==SW4 && long_press_delay ++ > 20 )
        {
            TMR2ON=0;
            switch(menu_pos)
            {
                case 0: clear_screen();                       //if a long press detected in MAIN MENU going to menu position
                        control_flag=VIEW_LOG;                 //updating control flag to view log
                        long_press_delay=0;
                        strcpy(event,"VL");
                        log_event(event,speed);               //recording the event
                        menu_pos=0;
                        break;
                case 1:clear_screen();
                        control_flag=CLEAR_LOG;              //updating control flag to clear log
                        long_press_delay=0;
                        strcpy(event,"CL");
                        log_event(event,speed);               //recording the event
                        menu_pos=0;
                        break;
                case 2: clear_screen();
                        control_flag=DOWNLOAD_LOG;         //updating control flag to download log
                        long_press_delay=0;
                        strcpy(event,"DL");
                        log_event(event,speed);            //recording the event
                        menu_pos=0;
                        break;
                case 3: clear_screen();
                        control_flag=SET_TIME;           //updating control flag to set time
                        long_press_delay=0;
                        strcpy(event,"ST");
                        log_event(event,speed);         //recording the event
                        menu_pos=0;
                        break;
                 case 4: clear_screen();
                        control_flag=CHANGE_PASSWORD;        //updating the control flag to change password
                        long_press_delay=0;
                        strcpy(event,"CP");
                        log_event(event,speed);              //recording the event
                        menu_pos=0;
                        reset_flag=NEW_PASSWORD;
                        break;    
            }
            
        }
     
        else if((control_flag==VIEW_LOG || control_flag==CLEAR_LOG || control_flag==DOWNLOAD_LOG || control_flag==MAIN_MENU)&& key2 == SW5 && long_press_delay ++ > 20)
        {
            clear_screen();
            if(gr!=0)
                strcpy(event,gear[gr-1]);
            control_flag=DASH_BOARD;              //returning back to DASH BOARD if SW5 is  long pressed
            long_press_delay=0;
        }
        else if((control_flag==VIEW_LOG || control_flag==CLEAR_LOG || control_flag==DOWNLOAD_LOG )&& key2 == SW4 && long_press_delay ++ > 20)
        {
            clear_screen(); 
            menu_pos=0;
            strcpy(event,gear[gr]);
            control_flag=MAIN_MENU;                     //returning back to MAIN MENU if SW4 is  long pressed
            long_press_delay=0;
        }
        switch (control_flag)
        {
            case DASH_BOARD :
                                display_dashboard(event,speed);       //calling display dashboard function
                                break;
            case    LOGIN   :
                                switch(login_screen(key,reset_flag))
                                {
                                    case RETURN_BACK  :
                                                        clear_screen();                        //back to dash board
                                                        control_flag= DASH_BOARD;
                                                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                                                        __delay_us(100);
                                                        TMR2ON=0;
                                                        break;
                                    case LOGIN_SUCCESS :
                                                        clear_screen();
                                                        control_flag= MAIN_MENU;        //moving to main menu if login is success
                                                        return_time=5;
                                                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                                                        __delay_us(100);
                                                        break;   
                                }
                                break;
            case MAIN_MENU    :
                                TMR2ON=1;                   //turning on timer2
                                if(menu_screen(key2)==RETURN_BACK)
                                    control_flag=RETURN_BACK;
                                break;
            case VIEW_LOG     :
                                 view_log(key);             //function to display logs
                                 
                                 break;
            case CLEAR_LOG    :
                                 clear_log();                //function to clear the logs
                                 break;
            case DOWNLOAD_LOG :
                                 download_log();              //function to download logs using UART
                                 break;
            case SET_TIME     : 
                                 switch(set_time(key2))
                                 {
                                    case MAIN_MENU  :
                                                        control_flag=MAIN_MENU;        //going back to main menu
                                                        break;
                                    case DASH_BOARD:
                                                        strcpy(event,gear[gr]);
                                                        control_flag=DASH_BOARD; //going to dash board
                                                        break;
                                 }
                                 break;
            case CHANGE_PASSWORD:
                                    switch(change_password(key,reset_flag))
                                    {
                                       case RE_PASSWORD:
                                            clear_screen();
                                            clcd_write(DISP_ON_AND_CURSOR_OFF,INST_MODE);
                                            __delay_us(100);
                                            strcpy(event,gear[gr]);  //if password reset is successful going to DASH BOARD
                                            control_flag=MAIN_MENU;
                                            reset_flag=RESET_NOTHING;
                                            break;
                                   }
                                   break;   
            case RETURN_BACK   :
                                   clear_screen();
                                   control_flag= DASH_BOARD;         //moving to DASH BOARD
                                   clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                                   __delay_us(100);
                                    TMR2ON=0;             //turning off timer2
                                   break;
        }
        reset_flag=RESET_NOTHING;       
    }
    return;
}
