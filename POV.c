
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h> 

#include <16F877A.h>
#device ADC=8 // conversor A/D con 8 bits de resoluci�n

//============================================================================================
//								Definiciones Generales										//
//============================================================================================
#fuses XT,NOWDT,NOPROTECT,NOLVP,PUT,BROWNOUT
#use delay(clock=4000000)
 
#use fast_io (A)
#use fast_io (B)
#use fast_io (C)

#byte PORTA=0X05
#byte PORTB=0X06
#byte PORTD=0X08
#byte TRISA=0X85
#byte TRISB=0X86
#byte TRISD=0X88

int OCR1A,sei,TCCR1B,DDRD,PORTD2,MCUCR,GICR,DDRA,DDRB,DDRC,PINC,TIMSK,WGM12,ISR,DDD2,IS01,INT0,PORTC0,PORTC1,PORTC2,OCIE1A,CS11,ISC01;

#define F_CPU 1000000UL

#define EXLED_DATA PORTB
#define EXLED_DATA_D DDRB
#define LED_DATA PORTA
#define LED_DATA_D DDRA

#define SW PORTC
#define SW_D DDRC
#define SW1 PORTC0
#define SW2 PORTC1
#define SW3 PORTC2
#define SW_IO PINC

const unsigned char table[24][6] = {     { 0x7C, 0x82, 0x82, 0x82, 0x7C, 0x00  }, // 0 
                                        { 0x00, 0x84, 0xFE, 0x80, 0x00, 0x00  }, // 1    
                                        { 0x84, 0xC2, 0xA2, 0x92, 0x8C, 0x00  }, // 2    
                                        { 0x42, 0x82, 0x8A, 0x96, 0x62, 0x00  }, // 3    
                                        { 0x30, 0x28, 0x24, 0xFE, 0x20, 0x00  }, // 4    
                                        { 0x4E, 0x8A, 0x8A, 0x8A, 0x72, 0x00  }, // 5    
                                        { 0x78, 0x94, 0x92, 0x92, 0x60, 0x00  }, // 6    
                                        { 0x02, 0xE2, 0x12, 0x0A, 0x06, 0x00  }, // 7
                                        { 0x6C, 0x92, 0x92, 0x92, 0x6C, 0x00  }, // 8
                                        { 0x0C, 0x92, 0x92, 0x52, 0x3C, 0x00  }, // 9
                                        { 0x00, 0x6C, 0x6C, 0x00, 0x00, 0x00  }, // :  10
                                        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  }, // space 11
                                        { 0x82, 0xFE, 0x82, 0x82, 0x7C, 0x00  }, // D  12
                                        { 0x78, 0x94, 0x94, 0x94, 0x58, 0x00  }, // e  13
                                        { 0x98, 0x94, 0x94, 0x94, 0x64, 0x00  }, // s  14
                                        { 0xFE, 0x10, 0x10, 0x10, 0xE0, 0x00  }, // h  15
                                        { 0x00, 0x00, 0xFA, 0x00, 0x00, 0x00  }, // i  16
                                        { 0x00, 0x08, 0xFE, 0x88, 0x40, 0x00  }, // t  17
                                        { 0x48, 0xA4, 0x94, 0x78, 0x80, 0x00  }, // a  18
                                        { 0x7C, 0x82, 0x82, 0x82, 0x44, 0x00  }, // C  19
                                        { 0x00, 0xFC, 0x08, 0x08, 0xF0, 0x00  }, // m  20
                                        { 0x08, 0x08, 0xF0, 0x00, 0x00, 0x00  }, // m  21
                                        { 0xFE, 0x20, 0x50, 0x88, 0x00, 0x00  }, // k  22
                                        { 0x00, 0xFC, 0x08, 0x04, 0x04, 0x00  },};//r  23
                                                                    
///////////////////////////////GLOBAL VARIABLES//////////////////////////////////////

unsigned char time[]={0,0,10,0,0,10,0,0,11,11,12,13,14,15,16,17,15,18,11,19,15,18,20,21,16,22,18,23,18}; //create an array with having 8 elements.

unsigned int count=0,start=0, stop=0, minute=0, seconds=0, hours=0, H12OR24=0;

/////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////DECODE COUNT VALUE///////////////////////////////
void decode(){

    time[7]=seconds%10;
    //seconds value divided by 10 and take only fractional value of answer and it assign to time[7].
    time[6]=seconds/10;
    //seconds value divided by 10 and take only integer value of answer and it assign to time[6]. 
    time[5]=10;
    time[4]=minute%10;
    //minute value divided by 10 and take only fractional value of answer and it assign to time[4].
    time[3]=minute/10;
    //minute value divided by 10 and take only integer value of answer and it assign to time[3].
    time[2]=10;
    time[1]=hours%10;
    //hours value divided by 10 and take only fractional value of answer and it assign to time[1].
    time[0]=hours/10;
    //hours value divided by 10 and take only integer value of answer and it assign to time[0]. 
}
///////////////////////////////////////END of DECODE//////////////////////////////////


//////////////////////////////////////Display Part////////////////////////////////////
void display(){ 
    unsigned int i=0,j=0;
            
        for(i=0;i!=29;i++){
            for(j=0;j!=6;j++){
            
                LED_DATA = table[time[i]][j]; /*sending element which has include in array as data
 to LED at one by one according to loop */

                delay_us(250);                
            }
        }
}
//////////////////////////////////////END of Display//////////////////////////////////


/////////////////////////////////////////SET TIMER////////////////////////////////////
void ini_tim_intr(){

    OCR1A = 499;              // Set the value that you want to count to

    TIMSK |= (1 << OCIE1A);  //Set the ISR COMPA vect

    sei();                   //enable interrupts

    TCCR1B |= (1 << WGM12)|(1 << CS11); // set prescaler to 8 and start the timer    
    
}
///////////////////////////////////////END of SET TIMER////////////////////////////////

//////////////////////////// TIME1 INTERRUPT SERVICE ROOTING///////////////////////////

ISR (TIMER1_COMPA_vect)  
{  
    if(++count>=400){ // count value is increase by one and check for the count is greater than 1000
        count=0;
        if(++seconds==60){//seconds value is increase by one and check for the seconds is equql to 60
            seconds=0;
            if(++minute==60){//minute value is increase by one and check for the minute is equql to 60
               minute=0;
                if(H12OR24==1){
                
                    if(++hours==24){//hours value is increase by one and check for the hours is equql to 24
                        hours=0;
                        
                    }
                }
                else{
                    if(++hours==13){//hours value is increase by one and check for the hours is equql to 13
                    hours=1;
                    }
                }
            }            
        }
    }                
}
///////////////////////ENDT of TIME1 INTERRUPT SERVICE ROOTING//////////////////////////


////////////////////////////MAIN FUNCTION///////////////////////////////////////////////
int main(void){

//---------------Phototransistor interrupt initialising------------------------------
    sei();                    // turn on interrupts
    DDRD &= ~(1 << DDD2);    // Clear the PD2 pin    
    PORTD |= (1 << PORTD2);  // turn On the Pull-up ( now PD2 is an input with pull-up enabled)
    MCUCR |= (1 << ISC01);   // set INT0 to trigger on falling edge.
    GICR  |= (1 << INT0);    // Turns on INT0
//---------------------------------------------------------------------------------

    LED_DATA_D=0xff;    //LED PORT set as output (PORTA    )
    EXLED_DATA_D=0xff;    //LED PORT set as output (PORTB)
    SW_D=0;             //SW PORT set as input (PORTC)
    
    while(1){        //--------------------->    Infinite loop
    
    ini_tim_intr();                // Turn timer function on    
    EXLED_DATA=3;

        if((SW_IO & (1<<SW1))==0){//check for the button is pressed
                if(++minute>=60){//minute value is increase by one and check for the minute is equql to 60
                    minute=0;                                             
                }    
            delay_ms(100);   
        }
        
        if((SW_IO & (1<<SW2))==0){// check for the button is pressed                         
                if(H12OR24==1){
                    if(++hours==24){//hours value is increase by one and check for the hours is equql to 24
                        hours=0;
                    }
                }
                else{
                
                    if(++hours==13){//hours value is increase by one and check for the hours is equql to 13
                    
                        hours=1;
                    }    
                }
                
            delay_ms(100);
        }            
                
        if((SW_IO & (1<<SW3))==0){ //check for the button is pressed
                if(H12OR24==0){ 
                    H12OR24=1;  
                    delay_ms(100);
                }
                
                else{
                    H12OR24=0;
                    delay_ms(100);
                }
        }
    }
}
////////////////////////////////END OF THE MAIN FUNCTION///////////////////////////////////////////////

//////////////////////Phototransistor INTERRUPT SERVICE ROOTING////////////////////////////////////////
ISR (INT0_vect)
{    
    decode();
    display();
}
////////////////////END of Phototransistor INTERRUPT SERVICE ROOTING///////////////////////////////////

 
///////////////////////////////////////END/////////////////////////////////////////////////////////////