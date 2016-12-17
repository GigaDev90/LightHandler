/*
 * File:   adc_test.c
 * Author: corrado
 */

#define _XTAL_FREQ   8000000

#include <xc.h>
#include <pic16f690.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
//#include <delays.h>

// PIC16F690 Configuration Bit Settings

// CONFIG
#pragma config FOSC = INTRCIO   // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA4/OSC2/CLKOUT pin, I/O function on RA5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select bit (MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Selection bits (BOR enabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)

#define OFF  0
#define ON   1
#define T_DELAY 125
#define ONOFF 12
#define UP 28
#define DOWN 29
#define LEFT 44
#define RIGHT 43


int i;
int x;
int valore = 0;
char flip = 0;
char isStillPressed = 0;
bool status = OFF;
bool bits[13];

char c;

void putch(unsigned char byte)
{
	/* output one byte */
	TXREG = byte;
	while (!TXSTAbits.TRMT); /* set when register is empty */
}

void setLum(int i)
{
	CCP1CONbits.DC1B = 00 | i;
	CCPR1L = 00000000 | ( i >> 2  );
}
void onOff()
{
	if ( status == OFF )
	{
		for ( i = 0; i < 1024; i++)
		{
			setLum(i);
			__delay_ms(1);
		}

		status	= ON;
	}
	else
	{
		i = 0000000000 | ( CCPR1L << 2 );
		i = i | CCP1CONbits.DC1B;
		for ( ; i >= 0; i--)
		{
			setLum(i);
			__delay_ms(1);
		}

		status	= OFF;
	}
	//printf("%d %d \n\r",CCPR1L, CCP1CONbits.DC1B);

}

void increaseLum()
{
	x = 0000000000 | ( CCPR1L << 2 );
	x = x | CCP1CONbits.DC1B;
        x++
        
	if ( x < 1024 )
	{
		setLum(x);
		__delay_ms(5);
	}

}

void decreaseLum()
{
	x = 0000000000 | ( CCPR1L << 2 );
	x = x | CCP1CONbits.DC1B;
        x--;
        
	if ( x > 0 )
	{
		setLum(x);
		__delay_ms(5);
	}

}

void maxLum()
{
	i = 0000000000 | ( CCPR1L << 2 );
	i = i | CCP1CONbits.DC1B;

	for ( ; i < 1024; i++)
	{
		setLum(i);
		__delay_ms(1);
	}
}

void minLum()
{
	i = 0000000000 | ( CCPR1L << 2 );
	i = i | CCP1CONbits.DC1B;
	for ( ; i > 0; i--)
	{
		setLum(i);
		__delay_ms(1);
	}	
}

// void decodeFromSerial( char c )
// {
// 	switch ( c )
// 	{
// 		case '1':
// 			setLum( 1 * 100 );
// 			break;
// 		case '2':
// 			setLum( 2 * 100 );
// 			break;
// 		case '3':
// 			setLum( 3 * 100 );
// 			break;
// 		case '4':
// 			setLum( 4 * 100 );
// 			break;	
// 		case '5':
// 			setLum( 5 * 100 );
// 			break;
// 		case '6':
// 			setLum( 6 * 100 );
// 			break;	
// 		case '7':
// 			setLum( 7 * 100 );
// 			break;	
// 		case '8':
// 			setLum( 8 * 100 );
// 			break;	
// 		case '9':
// 			setLum( 9 * 100 );
// 			break;	
// 		case 'U':
// 			increaseLum();
// 			break;	
// 		case 'D':
// 			decreaseLum();
// 			break;
// 		case 'L':
// 			minLum();
// 			break;
// 		case 'R':
// 			maxLum();
// 			break;
// 		case 'A':
// 			if ( !status )
// 				onOff();
// 			break;
// 		case 'S':
// 			if ( status )
// 				onOff();
// 	}
// 	printf("%c ok",c);
// 
// 
// }

void interrupt isr(void)
{
	// scatta ogni millisecondo
	if (PIR1bits.RCIF == 1)
	{
		//c = RCREG;
		//decodeFromSerial(c);

	}
}


int main(void) {

	OSCCONbits.IRCF = 0b111;

	ANSEL = 0;
	ANSELH = 0;

	/* configure digital I/O */
	TRISBbits.TRISB4 = 1;
	TRISCbits.TRISC0 = 0;
	TRISCbits.TRISC1 = 0;
	TRISCbits.TRISC2 = 0;
	TRISCbits.TRISC3 = 0;
	TRISCbits.TRISC4 = 0;
	TRISCbits.TRISC5 = 1;
	TRISCbits.TRISC7 = 1;
	TRISCbits.TRISC6 = 1;
	TRISBbits.TRISB7 = 0; // TX PIN set to output
	TRISBbits.TRISB5 = 1; // RX PIN set to input

	PORTCbits.RC0 = 0;
	PORTCbits.RC1 = 0;
	PORTCbits.RC2 = 0;
	PORTCbits.RC3 = 0;
	PORTCbits.RC4 = 0;
	//PORTCbits.RC5 = 1;

	TXSTAbits.TX9 = 0; // 8 bit data
	TXSTAbits.TXEN = 1; // enable transmitter
	TXSTAbits.BRGH = 1; // high speed transmission

	// setup UART receiver (porta seriale)
	RCSTAbits.SPEN = 1; // enable serial port
	RCSTAbits.RX9 = 0; // 8 bit data
	RCSTAbits.CREN = 1; // enable receiver

	// baud rate generator control
	BAUDCTLbits.BRG16 = 1; // 16 bit baud rate generator

	// baud rate generator value
	SPBRGH = 0;
	SPBRG = 207;


	/* configure timer */
	OPTION_REGbits.T0CS = 0;  //tmr0 clock source
	OPTION_REGbits.PS = 0b010;  //prescaler
	OPTION_REGbits.PSA = 0; // prescaler is on tmr0
	OPTION_REGbits.T0SE = 0;
	INTCONbits.T0IF = 0;  // FLAG
	INTCONbits.T0IE = 0;  // Enable interrupt on tmr0

	INTCONbits.GIE = 1;  //global interrupt flag
	INTCONbits.PEIE = 1;

	/* configure ADC */
	//     ANSELbits.ANS0 = 1; // AN0 on pin 19
	//     ANSELHbits.ANS8 = 0;
	//     ANSELHbits.ANS9 = 0;
	// 
	//     ADCON0bits.ADFM = 1;  // right justified
	//     ADCON0bits.VCFG = 0;  // Reference = VDD
	//     ADCON0bits.CHS = 0;   // Select channel 0
	//     ADCON0bits.ADON = 1;  // turn adc ok
	// 
	//     ADCON1bits.ADCS = 0b111;   // ADC clock = FOSC/64


	//configurazione pwm
	PR2 = 255;  // Periodo pwm

	CCP1CONbits.P1M = 0b00; 
	CCP1CONbits.CCP1M = 0b1100;
	CCP1CONbits.DC1B = 0b00;
	CCPR1L = 0;

	PIR1bits.TMR2IF = 0; // FLAG
	T2CONbits.T2CKPS = 0b01; // prescaler tmr2
	//PIE1bits.TMR2IE = 1;  //enable interrupt on tmr2
	T2CONbits.TMR2ON = 1; // enable tmr2

	//TRISBbits.TRISB5 = 0;

	while ( PIR1bits.TMR2IF == 0 );
		TRISCbits.TRISC5 = 0;

	for (;;)
	{
		attesa:

		OPTION_REGbits.PS = 0b111;  //prescaler
 		TMR0 = -195;
		INTCONbits.T0IF = 0;	

		while(	INTCONbits.T0IF == 1 )
		{
			if ( PORTBbits.RB4 == 0 )
			{
				TMR0 = -195;
				INTCONbits.T0IF = 0;
			}
		}

		
		i = 0;
 		TMR0 = 0;
		INTCONbits.T0IF = 0;

		while ( PORTBbits.RB4 == 1 )
		{
			
			if ( INTCONbits.T0IF == 1 )
			{
				if ( i == 3 )
				{
					isStillPressed = 0;

					while ( PORTBbits.RB4 == 1 );
				}
				else
				{
					i++;
					TMR0 = 0;
					INTCONbits.T0IF = 0;
				}
			}	
		}

		OPTION_REGbits.PS = 0b010;  //prescaler

		for( i = 0; i < 13; i++ )
		{
			
			__delay_us(1320);
			bits[i] = PORTBbits.RB4;
			TMR0 = -T_DELAY;
			INTCONbits.T0IF = 0;
			while (bits[i] == PORTBbits.RB4)
			{	
				if ( INTCONbits.T0IF == 1 )
					goto attesa;
			}
		}

		valore = 0;

		if ( (bits[0] == 1) && (bits[2] == 0) ) //&& ( flip != bits[3] || flip == 2 ) )
		{
			int j = 6;
			for ( i = 6; i < 13; i++ )
			{
				valore = valore | (bits[i] << j);
				j--;
			}

			if ( valore == ONOFF )
			{
				if ( flip != bits[1] || !isStillPressed )
					onOff();
			}
			else if ( status )
			{
				switch (valore)
				{
					case UP:
						increaseLum();
						break;
					case DOWN:
						decreaseLum();
						break;
					case LEFT:
						minLum();
						break;
					case RIGHT:
						maxLum();
						break;
				}
				if ( valore <= 9 )
					setLum( valore * 100 );
			}

			flip = bits[1];
			isStillPressed = 1;
		}				
	}
}