#include <msp430.h>


/**
 * LAB 3 PWM main.c
 */
int rowOut;
char out[4] = {0x00,0x01,0x02,0x03};
char checkBit[4] = {0x02,0x04,0x08,0x10};
int keypad[4][4] = {{2222,4444,6666,0},
                    {8888,11000,13222,0},
                    {15444,19666,20000,0},
                    {0,0,0,0}};
int checkInput();
int int2Duty(int input);

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    ADC10CTL0 = ADC10SHT_2 + ADC10ON + ADC10IE; // ADC10ON, interrupt enabled
    ADC10CTL1 = INCH_1;						// input A1
    ADC10AE0 |= 0x02;                         // PA.1 ADC option select
    P1DIR |= 0x01;                            // Set P1.0 to output direction

    while(1)
    {
    	ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
    	__enable_interrupt();
    	if(ADC10MEM < 0x1FF)
    	{
    		P1OUT &=~ 0X01;
    	}else
    	{
    		P1OUT |= 0x01;
    	}
    }
}
// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{

}
