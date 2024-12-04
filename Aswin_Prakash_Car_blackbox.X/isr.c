/*Name :Aswin Prakash
 * Date :31 October 2024
 * DESCRIPTION : CAR BLACK BOX PROJECT
 * File:   isr.c
 */
#include <xc.h>
#include "main.h"

extern unsigned char sec,return_time;
void __interrupt() isr(void)
{
    static unsigned int count = 0;
    
    if (TMR2IF == 1)                             //checking if interrupt flag is set
    {
        if (++count == 1250)                      //count for 1 sec 
        {
            count = 0;
            if(sec > 0)
                sec--;
            else if(return_time > 0)
                return_time--;
        }
        
        TMR2IF = 0;
    }
}