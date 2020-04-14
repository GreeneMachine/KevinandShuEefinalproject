/*
 * File:   main.c
 * Author: kevingreene and shueeong
 *
 * Created on April 13, 2020, 4:44 PM
 */

#include "mcc_generated_files/mcc.h"
#include "xc.h"
#include <stdio.h> 
#pragma warning disable 373
#pragma warning disable 520     
#pragma warning disable 751
#pragma warning disable 1498

#define LED_ON      25 
#define LED_OFF     0

void readRisingEdge(void);
void readFallingEdge(void);
void myCaptureISR(void);

//----------------------------------------------
// Main "function"
//----------------------------------------------
void main (void) {

    uint8_t  i;
    
    char cmd;
    char charIn;
    
    SYSTEM_Initialize();
    
    INTERRUPT_PeripheralInterruptEnable();
    INTERRUPT_GlobalInterruptEnable();           

	printf("Dev'19 Board\r\n");
    printf("Final Project - Universal Remote Control\r\n");
    printf("\r\n");
	printf("\r\n");                     


	for(;;) {

		if (EUSART1_DataReady) {			// wait for incoming data on USART
            cmd = EUSART1_Read();
			switch (cmd) {		// and do what it tells you to do

			//--------------------------------------------
			// Reply with help menu
			//--------------------------------------------
			case '?':
                printf("-------------------------------------------------\r\n");
                printf("?: help menu\r\n");
				printf("o: k\r\n");
                printf("Z: Reset processor\r\n");                     
                printf("z: Clear the terminal\r\n"); 
                printf("c: count number of half bits\r\n");
                printf("t: determine Time periods for all the bits\r\n");
                printf("d: decode logic 1 periods\r\n");
                printf("r: Report bit width values\r\n");
                printf("l: cLone 4 donor remote buttons\r\n");
                printf("1-4: Transmit IR packet\r\n");
                printf("-------------------------------------------------\r\n");
				break;

			//--------------------------------------------
			// Reply with "ok", used for PC to PIC test
			//--------------------------------------------
			case 'o':
				printf("o:	ok\r\n");
				break;                
                
            //--------------------------------------------
            // Reset the processor after clearing the terminal
            //--------------------------------------------                      
            case 'Z':
                for (i=0; i<40; i++) printf("\n");
                RESET();                    
                break;

            //--------------------------------------------
            // Clear the terminal
            //--------------------------------------------                      
            case 'z':
                for (i=0; i<40; i++) printf("\n");          
                break;                  

            //--------------------------------------------
            // Count number of half bits
            //--------------------------------------------                      
            case 'c':
                            
                break;
                
            //--------------------------------------------
            // Determine Time periods for all the bits
            //--------------------------------------------                      
            case 't':
                           
                break;

            //--------------------------------------------
            // Decode logic 1 periods
            //--------------------------------------------                      
            case 'd':
                            
                break;

            //--------------------------------------------
            // Report bit width values
            //--------------------------------------------                      
            case 'r':
                            
                break;

            //--------------------------------------------
            // Clone 4 donor remote buttons
            //--------------------------------------------                      
            case '1':
                           
                break;

            //--------------------------------------------
            // Transmit IR packet
            //--------------------------------------------                      
            case '1':
                           
                break;                

            
			//--------------------------------------------
			// If something unknown is hit, tell user
			//--------------------------------------------
			default:
				printf("Unknown key %c\r\n",cmd);
				break;
			} // end switch            
		} // end if
	} // end while 
} // end main

typedef enum {

void myCaptureISR(void) {
    
    
}


void readRisingEdge(void) {
    
    CCP3CONbits.CCP3M = 0b0101; 
    
}

void readFallingEdge(void) {
    
    CCP3CONbits.CCP3M = 0b0100;
            
}


