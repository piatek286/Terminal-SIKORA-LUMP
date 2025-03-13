/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs 

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs  - 1.45
        Device            :  PIC12F1572
        Driver Version    :  2.00
    The generated drivers are tested against the following:
        Compiler          :  XC8 1.35
        MPLAB             :  MPLAB X 3.40
*/

/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

#include "mcc_generated_files/mcc.h"

/*
                         Main application
 */
#define rs485_out   IO_RA2_SetHigh()
#define rs485_in    IO_RA2_SetLow()
#define STX         0x20
#define ETX         0x03

uint8_t read_buffer[7];
uint8_t in_data;


void main(void)
{
    uint8_t i;
    // initialize the device
    SYSTEM_Initialize();

    // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
    //INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();
    /*
    rs485_out;
    printf("PIC");
    IO_RA5_SetHigh();
    __delay_ms(500);
    IO_RA5_SetLow();
    __delay_ms(500);
    */

    IO_RA4_SetHigh();
    IO_RA2_SetLow();
    do{
        /*
        IO_RA2_SetHigh();
        __delay_ms(100);
        EUSART_Write('W');
        EUSART_Write('Y');
        EUSART_Write('S');
        EUSART_Write('L');
        EUSART_Write('I');
        EUSART_Write('J');
        EUSART_Write('\n');
        __delay_ms(100);
        IO_RA2_SetLow();
        __delay_ms(100);
         * */
        do{
            in_data = EUSART_Read();
        }while(in_data != 0x02);//STX

        i = 0;
        do{
            read_buffer[i] = EUSART_Read(); 
            i++;
        }while((i < 7) && (read_buffer[i-1] != ETX));

        
        if(read_buffer[0] == '0' && read_buffer[1] == '1' && read_buffer[2] == '2')
        {
            if(read_buffer[3] == '0')
            {
               PWM1_Start();
            }
            else if(read_buffer[3] == '1')
            {
               PWM1_Start();//led
                IO_RA4_SetLow();//pk
                __delay_ms(1000);
                IO_RA4_SetHigh();//pk
            }
        }
        else if(read_buffer[0] == '0' && read_buffer[1] == '1' && read_buffer[2] == '1')
        {
            PWM1_Stop();//led
            //IO_RA4_SetHigh();//pk
        }
        /*
        __delay_ms(100);
        IO_RA2_SetHigh();
        __delay_ms(100);
        EUSART_Write(' ');
        EUSART_Write('O');
        EUSART_Write('U');
        EUSART_Write('T');
        EUSART_Write(':');
        EUSART_Write(read_buffer[0]);
        EUSART_Write(read_buffer[1]);
        EUSART_Write(read_buffer[2]);
        EUSART_Write(read_buffer[3]);
        EUSART_Write(' ');
        __delay_ms(100);
         * */
    }while(1);

    /*
    while (1)
    {
        rs485_in;
        do{
            rs485_out;
            printf("wy?lij");
            rs485_in;
            while(EUSART_DataReady);
            in_data = getch();
            rs485_out;
            printf("wy");
            putch(in_data);
        }while(in_data != STX);
            
        if(in_data == STX)
        {
            i = 0;
            do{
                read_buffer[i] = getch(); 
                i++;
            }while((i < 7) && (read_buffer[--i] != ETX));
        }
        
        if(read_buffer[1] == '0' && read_buffer[2] == '2' && read_buffer[3] == '4')
        {
            if(read_buffer[4] == '0')
            {
                IO_RA5_SetLow();
            }
            else if(read_buffer[4] == '1')
            {
                IO_RA5_SetLow();
                IO_RA4_SetLow();
            }
        }
        
    }
     */
}
/**
 End of File
*/