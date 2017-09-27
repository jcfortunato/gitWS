# define FCY  29491000L  //number of instructions per milisecond Fclock=7372800Hz and PLL=16

  
_FOSC(CSW_FSCM_OFF & XT_PLL16);  //oscilator at 16x cristal frequency
_FWDT(WDT_OFF);                  //watchdog timer off

  
#define LED1   _LATF0   //define port for LED1
#define LED2   _LATF1   //define port for LED1
 
// define speed and staccato (small pause after every note)
#define metro 108      //defines how many "Quarter note" (pt: semínimas) fit into one minute of time (ex:120 means 2*Quarter notes/s) 
#define staccato 0.1   //defines the percentage of the note duration to pause after playing the note
 
// define time for notes and pauses
#define whole 4    // Define time for Whole note     (pt: semibreve)
#define half  2    // Define time for Minim          (pt: minima)
#define qua   1    // Define time for Crotchet       (pt: semínima)
#define eigh  0.5  // Define time for Quaver         (pt: colcheia)
#define eighh 0.75 // Define time for Quaver + half  (pt: colcheia e meia)
#define sixth 0.25 // Define time for Semiquaver     (pt: semicolcheia)
 

 
// define notes
#define la4   8378
#define fa4   10473
#define do5   6982
#define mi5   5543
#define la4b  8798       
#define la5   4151       
#define la5b  4399
#define fa5   5229
#define si4b  7843
#define mi5b  5870
#define re5b  6595       
#define re5   6227       
#define sol5  4666       
#define si4   7402       
 
 
#define t1  500
#define p1  175
 
#include <xc.h>
#include <p30F4011.h>
#include <libpic30.h>
#include <timer.h>
#include <stdio.h>
#include <uart.h>            //UART (serial port) library
 
 
char RXbuffer[80];        //buffer used for storing received characters trough the serial port
int str_pos = 0;        //last character position in RXbuffer
  
/*main function */
int main(void)
{
    unsigned int UMODEvalue, U2STAvalue; //auxiliary variables to configure the UART
    int ADCValue1 = 0;
//    int[] rhythm;
    int[] staccato;
//    int[] musica;
     
    int musica[] = {la4,la4,la4,fa4,do5,|la4,fa4,do5,la4,|mi5,mi5,mi5,fa5,do5,|la4b,fa4,do5,la4,|la5,la4,la4,la5,la5b,sol5,|sol5b,fa5,sol5b,0,si4b,mi5b,re5,re5b,|do5,si4,do5,0,fa4,la4b,fa4,la4b,|do5,la4,do5,mi5,||la5,la4,la4,la5,la5b,sol5,|sol5b,fa5,sol5b,0,si4b,mi5b,re5,re5b,|do5,si4,do5,0,fa4,la4b,fa4,do5,|la4,fa4,do5,la4};
    int rhythm[] = {qua,qua,qua,eighh,sixth,|qua,eighh,sixth,half,|qua,qua,qua,eighh,sixth,|qua,eighh,sixth,half,|qua,eighh,sixth,qua,eighh,sixth,|sixth,sixth,eigh,eigh,eigh,qua,eighh,sixth,|sixth,sixth,eigh,eigh,eigh,qua,eighh,sixth,|qua,eighh,sixth,half,|qua,eighh,sixth,qua,eighh,sixth,|sixth,sixth,eigh,eigh,eigh,qua,eighh,sixth,|sixth,sixth,eigh,eigh,eigh,qua,eighh,sixth,|qua,eighh,sixth,half};
     
//    int ADCValue2 = 0;
  
    TRISBbits.TRISB0 = 1;    //AN0 configured as an input
    TRISBbits.TRISB1 = 1;    //AN1 configured as an input
  
    _TRISD2 = 0;
    _TRISD0 = 0;
  
    //SerialPort config
    UMODEvalue = UART_EN & UART_IDLE_CON & UART_NO_PAR_8BIT; //activates the uart in continuos mode and 8bit no parity mode
    U2STAvalue = UART_INT_TX & UART_TX_ENABLE & UART_INT_RX_CHAR & UART_RX_TX; //activates interruptof pin Tx + enables Tx + enable interrupt do Rx
     OpenUART2 (UMODEvalue, U2STAvalue, 15); //configures and activates UART2 at 115000 bps
    U2STAbits.URXISEL = 1;
    _U2RXIE = 1; //0-Interruption off, 1-Interruption on
    U2MODEbits.LPBACK = 0; //disables hardware loopback on UART2
    __C30_UART = 2; //define UART2 as predefined for use with stdio library
    __delay_ms(1000);
  
    printf("\n\r Serial port ONLINE \n"); //to check if the serial port is working
  
    //Configure ADC
    ADCON1bits.ADON = 0;     //turns the ADC module OFF
    ADCON1bits.FORM = 0;    //result as an integer
    ADCON1bits.SSRC = 0;    //clearing sampling bit starts convertion
    ADCON1bits.SIMSAM = 0;    //samples each channel at a time
    ADCON1bits.ASAM = 0;    //sampling begins when samp bit is set
    ADCON2bits.VCFG = 0;    //VrefH+ = AVcc and Vref- = AGND
    ADCON2bits.CHPS = 0;    //samples CH0 only
    ADCON2bits.SMPI = 0;    //interrups at every sample sequence
    ADCON2bits.BUFM = 0;    //result buffer used as a 16bit word
    ADCON2bits.ALTS = 0;    //always use MUX-A
    ADCON2bits.CSCNA = 1;    //enable scan
    ADCON3bits.SAMC = 0xF;    //16 Tad
    ADCON3bits.ADRC = 0;    //use system clock for convertion
    ADCON3bits.ADCS = 0xF;    //16 Tcy for convertion time
    ADCHSbits.CH123NB = 0;    //negative input is Vref-
    ADCHSbits.CH123SB = 0;    //positive input is AN0 and so on
    ADCHSbits.CH0NB = 0;    //negative input is Vref-
    ADCHSbits.CH0SB = 0;    //positive input is AN0 and so on
    ADCHSbits.CH123NA = 0;    //negative input is Vref-
    ADCHSbits.CH123SA = 0;    //positive input is AN0 and so on
    ADCHSbits.CH0NA = 0;    //negative input is Vref-
    ADCHSbits.CH0SA = 0;    //positive input is AN0 and so on
    ADPCFG = 0xFC;            //AN0 and AN1 are analog inputs
    ADCSSL = 0x01;            //AN0 and AN1 are enabled for scan
    ADCON1bits.ADON = 1;     //turns the ADC module ON
  
    //timer2 config
    TMR2 = 0;                //reset timer2 for use as timer source for PWM
    PR2 = 0xFFFF;            //set PWM frequency ~ 40kHz: 0x02E1->39,96kHz ; 0x02E0->40,02kHz
    T2CONbits.TCS = 0;       //using instruction cycle to source timer2
    T2CONbits.TCKPS = 1;     //prescaler 1:1
  
    //timer3 config
    TMR3 = 0;                //reset timer2 for use as timer source for PWM
    PR3 = 0x8F;            //set PWM frequency ~ 40kHz: 0x02E1->39,96kHz ; 0x02E0->40,02kHz
    T3CONbits.TCS = 0;       //using instruction cycle to source timer2
    T3CONbits.TCKPS = 0;     //prescaler 1:1
  
    //output compare 1 config
    OC3CONbits.OCM = 6;        //OC1 PWM mode at RD0 pin
    OC3RS = PR2/2;                  //duty cycle = 50% -> PR2/2
    OC3R = 0;                //no initial delay
    OC3CONbits.OCTSEL = 0;     //using timer2
  
    //output compare 1 config
    OC1CONbits.OCM = 6;        //OC1 PWM mode at RD0 pin
    OC1RS = PR3/2;             //duty cycle = 50% -> PR2/2
    OC1R = 0;                  //no initial delay
    OC1CONbits.OCTSEL = 1;     //using timer3
  
    //start PWM
    T2CONbits.TON = 1;        //start timer and start PWM
  
    //start PWM
    T3CONbits.TON = 1;        //start timer and start PWM   
     
  //  printf("\n\rPWMs Online\n");
 
    if( sizeof(musica) != sizeof(rhythm) )
    {
        return;
    }
 
    for(int i=0; i < sizeof(rhythm); i++)
    {
        rhythm[i] = rhythm[i]*60000*(1-staccato)/metro;
        stac[i]   = rhythm[i]*60000*staccato/metro);
    }
 
    while(1)
    {        
        /*
         http://www.musicnotes.com/sheetmusic/mtd.asp?ppn=MN0102729
          
         */
         
         
        //start
         
        for(int i=0; i < sizeof(musica); i++)
        {
             
            if (musica[i] != 0)
            {
                PR2 = musica[i];
                OC3RS = PR2/2;    //duty cycle as a function of the analogue input
                __delay_ms(rhythm[i]);
                OC3RS=0;
                __delay_ms(stac[i]);
            }
            else
            {
                __delay_ms(rhythm[i]+stac[i]); //Não sei se dá
                //__delay_ms(rhythm[i]);
                //__delay_ms(stac[i]);
            }
             
             
        }
         
        __delay_ms(2000);
     
         
    }    //end while(1)
  
}//end main
  
/* This is UART2 receive ISR */
void __attribute__((__interrupt__,auto_psv)) _U2RXInterrupt(void)
{
    IFS1bits.U2RXIF = 0;    //resets RX2 interrupt flag
  
    while(U2STAbits.URXDA)    // Read the receive buffer until at least one or more character can be read
      {
          RXbuffer[str_pos] = U2RXREG;
        str_pos++;
        if(str_pos >= 80){str_pos = 0;}
        LED1 = !LED1;
    }
}

