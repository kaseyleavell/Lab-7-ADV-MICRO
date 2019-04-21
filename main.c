#include <msp430.h>


/**
 * LAB 7 VU meter main.c
 */
#define SCL  0x02;          //serial clock line
#define SDA  0x01;          //serial data line
#define SLEEP; __delay_cycles(700);

int rowOut;
int fired = 0;
int outArray[10]= {0};
char dispArray[4] = {0};

int temp,temp2,i,avg_adc;

void readBit();
void i2cStart();
void i2cStop();
void sendData(char addressPush, char instructionPush, char controlPush, char * arrayPush);
void clockBits(char byte);
int checkInput();
int int2Duty(int input);

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    ADC10CTL0 = ADC10SHT_2 + ADC10ON + ADC10IE; // ADC10ON, interrupt enabled
    ADC10CTL1 = INCH_0;						// input A1
    ADC10AE0 |= 0x01;                         // PA.1 ADC option select
    P1OUT &=~ 0xFF;
    P1DIR &=~ 0x03;
    //I2C initialization
    P2DIR |= 0xFF;

    CCTL0 = CCIE;
    CCR0 = 1000;
    TACTL = TASSEL_2 + MC_1;
    __enable_interrupt();
    while(1)
    {
        if(fired == 1)
        {
            ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
            __enable_interrupt();
            temp = outArray[0];
            outArray[0] = ADC10MEM;
            avg_adc = outArray[0];
            for(i = 1;i<10;i++)
            {
                temp2 = outArray[i];
                outArray[i] = temp;
                avg_adc = outArray[i] + avg_adc;
                temp = temp2;
            }
            avg_adc = avg_adc/10;
            setDispArray(avg_adc,dispArray);
            sendData(0x76,0x00,0xC7,dispArray);
            zeroArray(dispArray);
            CCTL0 = CCIE;
            fired = 0;
        }
    }
}
// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{

}
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
    __disable_interrupt();
    fired = 1;
    CCTL0 &=~ CCIE;
    __enable_interrupt();
}
void sendData(char addressPush, char instructionPush, char controlPush, char * arrayPush)
{
    int j;
    i2cStart();
    clockBits(addressPush);
    SLEEP;
    readBit();
    clockBits(instructionPush);
    readBit();
    SLEEP;
    clockBits(controlPush);
    readBit();
    SLEEP;
    for(j = 0;j<4;j++)
    {
        clockBits(arrayPush[j]);
        SLEEP;
        readBit();
    }
    i2cStop();
}
void i2cStart()
{
    P2OUT |= SCL;
    P2OUT |= SDA;
    SLEEP;
    P2OUT &=~SDA;
    SLEEP;
    P2OUT &=~SCL;
    SLEEP;
}
void i2cStop()
{
    P2OUT &=~SDA;
    SLEEP;
    P2OUT |= SCL;
    SLEEP;
    P2OUT|= SDA;
    SLEEP;
}
void readBit()
{
    P2OUT |= SDA;
    //read the SDA line
    P2DIR &=~ SDA;
    SLEEP;
    //P2OUT |= SCL;
    //read a single bit from the slave
    //...slave will pull SDA line to low if it was called
    //check the bit
    char temp = SDA;
    if(P2IN & temp)
    {
        __delay_cycles(200);
    }
    else{
        P2OUT |= SCL;
        P2OUT &=~ SCL;
    }
    P2DIR |= SDA;
}
void clockBits(char byte)
{
    int i;
    for(i = 0;i < 8;i++)//read write bit is embedded in the address
    {
        if(byte & 0x80)
        {
            //send to LED while testing
            P2OUT |= SDA;
        }else
        {
            P2OUT &=~ SDA;
        }
        byte = byte << 1;
        P2OUT |= SCL;              //rising edge for screen reading the bit on line
        SLEEP;
        P2OUT &=~ SCL;
    }
}
void setDispArray(int adc,char * arrayPush)
{
    int numberCompare[8] = {425,450,475,500,525,550,575,600};
    int i,place;
    for(i = 0;i<8;i++)
    {
        if(numberCompare[i]>adc)
        {
            break;
        }
    }
    if(!((i%2) == 0))    //odd number
    {
        place = (i-1)/2;
        arrayPush[place] = 0x06;
    } else
    {
        place = i/2;
    }
    for(i = 0;i<place;i++)
    {
        arrayPush[i] = 0x36;//setting all of the bars
    }
}
void zeroArray(char*arrayPush)
{
    int inc;
    for (inc = 0;inc<4;inc++)
    {
        arrayPush[inc] = 0x00;
    }
}
