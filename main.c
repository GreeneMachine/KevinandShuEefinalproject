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

uint16_t startLowCnts = 0;
uint16_t startHighCnts = 0;
uint16_t lowCnts = 0;
uint16_t oneACnts = 0;
uint16_t oneBCnts = 0;
uint16_t endLowCnts = 0;

uint8_t numEdges = 0;

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

//uint16_t startLowCnts = 0;
//uint16_t startHighCnts = 0;
//uint16_t lowCnts = 0;
//uint16_t oneACnts = 0;
//uint16_t oneBCnts = 0;
//uint16_t endLowCnts = 0;

typedef enum {START_START_LOW, END_START_LOW, END_START_HIGH, FIRST_DATA_LOW, FIRST_DATA_HIGH, DATA_LOW, DATA_HIGH} state; 

void myCaptureISR(void) {
    
    static state isrState = START_START_LOW; 
    static uint16_t previousTMRcnts = 0;
    static uint8_t numDataBits = 0;     // We increment numDataBits every time we process the highCnts
    
    uint16_t currentTMRcnts = 0;    
    uint16_t checkCnts = 0;     // What is this variable for?
    uint8_t allowance = 10;     // 10% allowance
    
    currentTMRcnts = CCPR3H;    // Every time ISR triggers, we load the current tmr cnts into this variable
    currentTMRcnts = (currentTMRcnts << 8) + CCPR3L;
    
    numEdges++; 
    
    switch (isrState){
        
        case START_START_LOW:
            
            previousTMRcnts = currentTMRcnts; 
            
            readRisingEdge();
            PIR4bits.CCP3IF = 0;
            isrState = END_START_LOW;
            
            break;
                
        case END_START_LOW:
            
            startLowCnts = currentTMRcnts - previousTMRcnts;
            
            previousTMRcnts = currentTMRcnts;
            
            readFallingEdge();
            PIR4bits.CCP3IF = 0;
            isrState = END_START_HIGH;
            
            break;
            
        case END_START_HIGH:
            
            startHighCnts = currentTMRcnts - previousTMRcnts;
            
            previousTMRcnts = currentTMRcnts;
            
            readRisingEdge();
            PIR4bits.CCP3IF = 0;
            isrState = FIRST_DATA_LOW;
            
            break;
            
        case FIRST_DATA_LOW:
            
            lowCnts = currentTMRcnts - previousTMRcnts;
            
            previousTMRcnts = currentTMRcnts;
            
            readFallingEdge();
            PIR4bits.CCP3IF = 0;
            isrState = FIRST_DATA_HIGH;
            
            break;
            
        case FIRST_DATA_HIGH:
            
            oneACnts = currentTMRcnts - previousTMRcnts; // oneACnts is how long the first data bit is high
            numDataBits++; 
            
            previousTMRcnts = currentTMRcnts;
            
            readRisingEdge();
            PIR4bits.CCP3IF = 0;
            isrState = DATA_LOW;
            
            break;
            
        case DATA_LOW:
            
            checkCnts = currentTMRcnts - previousTMRcnts;
            
            // Stop bit, check to see if significantly longer than lowCnts, or if we reach 32 bits (could maybe throw a flag?)
            if(checkCnts > lowCnts + SOME NUMBER WHAT SHOULD IT BE?){ // Number should be within 10 % of one another
                    endLowCnts = checkCnts;     
                    isrState = START_START_LOW;
            }else{
                lowCnts = (lowCnts * (numDataBits - 1) + checkCnts) / numDataBits; // TODO: Check if this is good then implement below
                //lowCnts = ((checkCnts + lowCnts) / 2); // Not the right way average
                isrState = DATA_HIGH;
            }
            
            previousTMRcnts = currentTMRcnts;
            
            readFallingEdge();
            PIR4bits.CCP3IF = 0;

            break;
            
        case DATA_HIGH:
            
            checkCnts = currentTMRcnts - previousTMRcnts;
            numDataBits++; 
            
            // If checkCnts is less than 10 % away from oneACnts, we consider the same bit as A
            if(checkCnts > (oneACnts + oneACnts / allowance)  || checkCnts < (oneACnts + oneACnts / allowance) ){
                //oneBCnts = ((checkCnts + oneBCnts) / 2); // Not the right way to average
            }else{
                //oneACnts = ((checkCnts + oneACnts) / 2); 
            }
            
            isrState = DATA_LOW;
            
            previousTMRcnts = currentTMRcnts;
            
            readRisingEdge();
            PIR4bits.CCP3IF = 0;
            
            break;
    }
}


void readRisingEdge(void) {
    
    CCP3CONbits.CCP3M = 0b0101; 
    
}

void readFallingEdge(void) {
    
    CCP3CONbits.CCP3M = 0b0100;
            
}


