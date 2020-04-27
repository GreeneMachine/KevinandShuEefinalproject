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

#define CONV        16

#define NUM_BUTTONS 4

void readRisingEdge(void);
void readFallingEdge(void);
//void myCaptureISR(void);
void sort(void);
void learn(uint8_t buttonNum);
void transmitButtonOverIR(uint8_t choice);

//uint16_t startLowCnts = 0;
//uint16_t startHighCnts = 0;
//uint32_t lowCnts = 0;
//uint32_t oneACnts = 0;
//uint32_t oneBCnts = 0;
//uint16_t endLowCnts = 0;

uint8_t numEdges = 0;

uint16_t highUS = 0;
uint16_t lowUS = 0;
uint16_t lowHalfUS = 0;

uint16_t tmrCounts[100];
uint16_t training[71];
bool doneTesting = false; 

uint8_t sample[500];

bool collectingData = false; 

uint32_t storeButton[NUM_BUTTONS];

uint16_t startLoUS = 0;
uint16_t startHiUS = 0;
uint16_t stopUS = 0;

printf("HI");

//----------------------------------------------
// Main "function"
//----------------------------------------------
void main (void) {

    uint8_t  i;
    uint16_t oneCnts = 0;
    uint16_t zeroCnts = 0;
    
    char cmd;
    char charIn;
    
    SYSTEM_Initialize();
    ECCP3_Initialize();
    EPWM2_Initialize();
    
    INTERRUPT_PeripheralInterruptEnable();
    INTERRUPT_GlobalInterruptEnable();           

    EPWM2_LoadDutyValue(LED_ON);
    LED_PIN_SetHigh();
    
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
                printf("--------------------------------------t-----------\r\n");
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
                numEdges = 0;
                printf("Press any remote button.\r\n");
                
//                collectingData = true;
//                while (collectingData == true);
//                
//                if (oneACnts > oneBCnts) {
//                    oneCnts = oneACnts;
//                    zeroCnts = oneBCnts;
//                } else {
//                    oneCnts = oneBCnts;
//                    zeroCnts = oneACnts;
//                }
                
                doneTesting = false;
                while (doneTesting == false);
                
                printf("\r\nDuration array in us\r\n");
                
                for (uint8_t i = 0; i < numEdges; i++) {
                    printf("%u ", training[i]);
                    if (i%8 == 0) {
                        printf("\r\n");
                    }
                } 
                
                startLoUS = training[1];
                startHiUS = training[2];
                stopUS = training[numEdges - 1]; 
                
                sort();
                //printf("lowHalfUS = %u, lowUS = %u, highUS = %u\r\n", lowHalfUS, lowUS, highUS);
                
                printf("\r\n");
                           
                break;
                
            case 's':
                
                //sort();        
                printf("Press remote.");
                
                while(IR_RX_GetValue() == 1);
                    
                for (uint16_t i = 0; i < 500; i++ ) {
                    sample[i] = IR_RX_GetValue();
                    TMR0_WriteTimer(0x10000-3200);
                    INTCONbits.TMR0IF = 0;
                    while(TMR0_HasOverflowOccured() == false);
                }
                
                for(uint16_t i = 0; i < 500; i++) {
                    printf(" % u\r\n", sample[i]);
                }
                    
                break;


            //--------------------------------------------
            // Decode logic 1 periods
            //--------------------------------------------                      
            case 'd':
                printf("Start: 		Lo: %u uS/%u cnts     Hi: %u uS/%u cnts\r\n", startLoUS, startLoUS*16, startHiUS, startHiUS*16);
                printf("Data 1:		Lo: %u uS/%u cnts     Hi: %u uS/%u cnts\r\n", lowHalfUS, lowHalfUS*16, highUS, highUS*16);
                printf("Data 0:		Lo: %u uS/%u cnts     Hi: %u uS/%u cnts\r\n", lowHalfUS, lowHalfUS*16, lowUS, lowUS*16);
                printf("Stop:		Lo: %u uS/%u cnts\r\n", stopUS, stopUS*16);
                printf("Half bits: %u half bits per button\r\n", numEdges);
                
                            
                break;

            //--------------------------------------------
            // Report bit width values
            //--------------------------------------------                      
            case 'r':
                            
                break;

            //--------------------------------------------
            // Clone 4 donor remote buttons
            //--------------------------------------------                      
            case 'l':
                
                for(uint8_t j = 0; j < 4; j++){
                    numEdges = 0;
                    printf("Press a button on the remote you would like to clone, you will clone four buttons\r\n");
                    doneTesting = false;
                    while (doneTesting == false);
                    learn(j); 
                    
//                    for (uint8_t i = 0; i < 68; i++) {
//                        printf(" %u ", training[i]);
//                        if (i%8 == 0) {
//                            printf("\r\n");
//                        }
//                    }
                
                }
                           
                break;

            //--------------------------------------------
            // Transmit IR packet
            //--------------------------------------------                      
            case '1':
                printf("Transmitting first button learned\r\n");
                EPWM2_LoadDutyValue(LED_ON);
                
                transmitButtonOverIR(0);
                break;

                
            case '2':
                printf("Transmitting 2\r\n");
                transmitButtonOverIR(1);
                break;
                    
            case '3':
                printf("Transmitting 3\r\n");
                transmitButtonOverIR(2);
                break;
                
            case '4':
                printf("Transmitting 4\r\n");
                transmitButtonOverIR(3);  
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

void ECCP3_CaptureISR(void) {
    
    //printf("inISR\r\n");
    //static state isrState = START_START_LOW; 
    static uint16_t previousTMRcnts = 0;
    static bool rise = true; 
    
    uint16_t currentTMRcnts = 0;    
    //uint16_t checkCnts = 0;
    //uint8_t allowance = 5;     // 20% allowance
    
    if (doneTesting == false) {
        currentTMRcnts = CCPR3H;    // Every time ISR triggers, we load the current tmr cnts into this variable
        currentTMRcnts = (currentTMRcnts << 8) + CCPR3L;

        training[numEdges] = (currentTMRcnts - previousTMRcnts)/16; 
        numEdges++; 

        previousTMRcnts = currentTMRcnts; 

        if (rise == true) {
            readRisingEdge();
            rise = false;
        } else {
            readFallingEdge();
            rise = true;
        }

        if (numEdges == 68) {
            doneTesting = true;
            rise = true;
            readFallingEdge();
        }
    }
    
    PIR4bits.CCP3IF = 0;
}

void readRisingEdge(void) {
    
    CCP3CONbits.CCP3M = 0b0101; 
    
}

void readFallingEdge(void) {
    
    CCP3CONbits.CCP3M = 0b0100;
            
}

void sort(void) {
    
    uint16_t aSecs = 0;
    uint16_t bSecs = 0;
    uint16_t aSum = 0;
    uint16_t bSum = 0;
    uint16_t lowHalfSum = 0;
    uint8_t aCnt = 1;
    uint8_t bCnt = 0; 
    
    aSecs = training[4]; 
    aSum = training[4];
    
    for (uint8_t i = 6; i <= 66; i = i + 2) {
        
        if (training[i] > (aSecs + aSecs/5) || training[i] < (aSecs - aSecs/5)) {
            bSum = bSum + training[i];
            bCnt++;
        } else {
            aSum = aSum + training[i];
            aCnt++; 
        }
        
    }
    
    for (uint8_t i = 3; i <= 65; i = i + 2) {
        if (training[i] > (training[i - 2] + training[i - 2]/5)){
            numEdges = i; 
            break;
        } else {
            lowHalfSum += training[i];
        }
    }
    
    lowHalfUS = lowHalfSum/32; 
    aSecs = aSum/aCnt;
    bSecs = bSum/bCnt; 

    if (aSecs < bSecs) {
        lowUS = aSecs;
        highUS = bSecs;
    } else {
        lowUS = bSecs;
        highUS = aSecs; 
    }
    
    for (uint8_t i = 0; i < 71; i++) {
        training[i] = 0;
    }  
    
}


void learn(uint8_t buttonNum){
    
    uint32_t tempButtonBits = 0;
    
    for(uint8_t i = 4; i <= 66; i = i + 2){
        
            if(training[i] > lowUS - lowUS *2/10 && training[i] < lowUS + lowUS *2/10){
            
                tempButtonBits = (tempButtonBits << 1 | 0);
            
            }else if((training[i] > highUS - highUS *2/10 && training[i] < highUS + highUS *2/10)){
            
                tempButtonBits = (tempButtonBits << 1 | 1);
            }   
    }
    
    storeButton[buttonNum] = tempButtonBits;
    
    printf("HEX Value \r\n");
    printf("0x");
    printf("%04x",tempButtonBits>>16);
    printf("%04x\r\n",tempButtonBits&0xFFFF);

}

void transmitButtonOverIR(uint8_t choice) {
    
    uint32_t mask;
    LED_PIN_SetLow();
    
    // Start Lo Bit
    EPWM2_LoadDutyValue(LED_OFF);
    TMR0_WriteTimer(0x10000 - startLoUS*16);
    INTCONbits.TMR0IF = 0;
    while(TMR0_HasOverflowOccured() == false);
    
    // Start Hi Bit
    EPWM2_LoadDutyValue(LED_ON);
    TMR0_WriteTimer(0x10000 - startHiUS*16);
    INTCONbits.TMR0IF = 0;
    while(TMR0_HasOverflowOccured() == false);
    
    // Data Bits
    mask = 0x80000000;
    
    for (uint8_t i = 0; i < 32; i++) {
        
        EPWM2_LoadDutyValue(LED_OFF);
        TMR0_WriteTimer(0x10000 - lowHalfUS*16);
        INTCONbits.TMR0IF = 0;
        while(TMR0_HasOverflowOccured() == false);
        
        if ((storeButton[choice] & mask) != 0){
            
            EPWM2_LoadDutyValue(LED_ON);
            TMR0_WriteTimer(0x10000 - highUS*16);
            INTCONbits.TMR0IF = 0;
            while(TMR0_HasOverflowOccured() == false); 
            
        }else{
            
            EPWM2_LoadDutyValue(LED_ON);
            TMR0_WriteTimer(0x10000 - lowUS*16);
            INTCONbits.TMR0IF = 0;
            while(TMR0_HasOverflowOccured() == false);
            
        }
        
        mask = (mask >> 1);
    }    
    // Stop Low Bit
    EPWM2_LoadDutyValue(LED_OFF);
    TMR0_WriteTimer(0x10000 - stopUS*16);
    INTCONbits.TMR0IF = 0;
    while(TMR0_HasOverflowOccured() == false); 
    
    EPWM2_LoadDutyValue(LED_ON);
    
    LED_PIN_SetHigh();
    
}

bool transmitting = false; 
typedef enum {IDLE, START_LOW, START_HIGH, DATA_LOW, DATA_HIGH, STOP_LOW} state; 

void ECCP1_CompareISR(void) {
    
    static state compareState = IDLE; 
    
    case IDLE:
        
        if (transmitting){
            compareState = START_LOW; 
            
            EPWM2_LoadDutyValue(LED_OFF);
            ECCP1_SetCompareCount(startLoUS*16);
            TMR3_WriteTimer(0);
        }
        
        break;
        
        
    case START_LOW: 
        
        
            
        break;
            
            
    case START_HIGH:
                
        break;
        
    // Clear the ECCP1 interrupt flag
    PIR1bits.CCP1IF = 0;
}




//switch (isrState){
//        
//        case START_START_LOW:
//            stateCheck[numEdges-1] = 0;
//            //printf("inISR START_START_LOW\r\n");
//            //numEdges = 0;
//            numDataBits = 0;
//            numBBits = 0;
//            numABits = 0;
//            
//            previousTMRcnts = currentTMRcnts; 
//            
//            PIR4bits.CCP3IF = 0;
//            if(collectingData == true){
//                isrState = END_START_LOW;
//                readRisingEdge();
//                numEdges = 1;
//            }
//            break;
//                
//        case END_START_LOW:
//            stateCheck[numEdges-1] = 1;
//            startLowCnts = currentTMRcnts - previousTMRcnts;
//            
//            previousTMRcnts = currentTMRcnts;
//            
//            readFallingEdge();
//            PIR4bits.CCP3IF = 0;
//            isrState = END_START_HIGH;
//            
//            break;
//            
//        case END_START_HIGH:
//            
//            startHighCnts = currentTMRcnts - previousTMRcnts;
//            
//            previousTMRcnts = currentTMRcnts;
//            
//            readRisingEdge();
//            PIR4bits.CCP3IF = 0;
//            isrState = FIRST_DATA_LOW;
//            
//            break;
//            
//        case FIRST_DATA_LOW:
//            
//            lowCnts = currentTMRcnts - previousTMRcnts;
//            
//            previousTMRcnts = currentTMRcnts;
//            
//            readFallingEdge();
//            PIR4bits.CCP3IF = 0;
//            isrState = FIRST_DATA_HIGH;
//            
//            break;
//            
//        case FIRST_DATA_HIGH:
//            
//            oneACnts = currentTMRcnts - previousTMRcnts; // oneACnts is how long the first data bit is high
//            numABits++; 
//            numDataBits++; 
//            
//            previousTMRcnts = currentTMRcnts;
//            
//            readRisingEdge();
//            PIR4bits.CCP3IF = 0;
//            isrState = DATA_LOW;
//            
//            break;
//            
//        case DATA_LOW:
//            
//            checkCnts = currentTMRcnts - previousTMRcnts;
//            
//            // If checkCnts is 20% more than lowCnts OR if numDataBits == 32, we are at the stop low bit
//            if(checkCnts > (lowCnts + lowCnts*2/10) || numDataBits == 45){ 
//                    endLowCnts = checkCnts; 
//                    isrState = START_START_LOW;
//                    //printf("Data collect false\r\n");
//                    collectingData = false;
//            }else{
//                lowCnts = (lowCnts*numDataBits + checkCnts) / (numDataBits + 1); 
//                
//                isrState = DATA_HIGH;
//            }
//            
//            previousTMRcnts = currentTMRcnts;
//            
//            readFallingEdge();
//            PIR4bits.CCP3IF = 0;
//
//            break;
//            
//        case DATA_HIGH:
//            
//            checkCnts = currentTMRcnts - previousTMRcnts;
//            numDataBits++; 
//            
//            // If checkCnts is more than 20 % away from oneACnts, we consider it B high
//            if(checkCnts > (oneACnts + oneACnts / allowance)  || checkCnts < (oneACnts - oneACnts / allowance) ){
//                oneBCnts = (oneBCnts*numBBits + checkCnts) / (numBBits + 1); 
//                numBBits++; 
//            }
//            
//            // Otherwise, it is A high
//            else{
//                oneACnts = (oneACnts*numABits + checkCnts) / (numABits + 1); 
//                numABits++;
//            }
//            
//            isrState = DATA_LOW;
//            
//            previousTMRcnts = currentTMRcnts;
//            
//            readRisingEdge();
//            PIR4bits.CCP3IF = 0;
//            
//            break;
//    }
