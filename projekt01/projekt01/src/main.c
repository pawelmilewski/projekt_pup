#define F_CPU 8000000 //czestotliwosc taktowania uC
#include "avr/io.h"	//wykorzystane biblioteki
#include "util/delay.h"
#include "HD44780.h"
#include "piny_bit.h"
#include "stdlib.h"
#include "interrupt/interrupt_avr8.h"
#include "string.h"  
#include "avr/interrupt.h"

//zmienne
unsigned long licz_ob;	//liczba biegow petli
int licz_ob1;	//liczba biegow petli1
int pomiar_ADC0;//odczyt wartosci z przetwornika A/C - port ADC0
int pomiar_ADC2;//odczyt wartosci z przetwornika A/C - port ADC2
volatile uint8_t timer0=250;
volatile uint8_t timer2=200;
volatile uint8_t licznik=0;
char bufor[4];
char bufor2[8];
//zmienne zegar
unsigned char hours = 0;
unsigned char minutes = 0;
unsigned char seconds = 0;
bool timeChanged = true;
bool timeChanged05 = true;
char time[] = "00:00:00";
#define SET_HOUR		3
#define SET_MINUTE		4
unsigned char i; 
//
#define SPR_PRES PE6
#define ON_OFF_ALL PD0
int obroty2;
float wolt;
int tablicarpm[5];

void LCD_update_time();

int main(void)
{//petla glowna
		DDRE=0x3C; // port e jako wyjœcie 00111100
		PORTE=0x3C;
		DDRB=0xFD;
		PORTB=0xFD;
		DDRG=0x02;
		PORTG=0x02;
		DDRC=0x7F;
		PORTC=0x7F;
		PORTG = (1<<SET_HOUR | 1<<SET_MINUTE); //piny zegara
		//######## konfiguracja ADC ##############
		ADCSRA=(1<<ADEN)			//ustawienie bitu ADEN=1 - wlaczenie  przetwornika A/C
		|(1<<ADPS0)	// ustawienie preskalera na 128 // ustawienie
		|(1<<ADPS1)	// czestotliwosc taktowania przetwornika A/C, f=8Mhz/128
		|(1<<ADPS2);
		ADMUX=(0<<REFS1)|(0<<REFS0); 	// wybor zewnetrznego napiecia odniesienia
		//###########################################
		//######## konfiguracja timera0 ##############
		TIMSK |= (1<<TOIE2)|(1<<TOIE0)|1<<OCIE1A;          //Przerwanie overflow (przepe³nienie timera)//ICIE1A ---- ZEGAR
		TCCR0 |= (1<<CS02) | (1<<CS01) | (0<<CS00); // Ÿród³em CLK, preskaler 256
		TCNT0 = timer0;//          //Pocz¹tkowa wartoœæ licznika
		//###########################################
		//######## konfiguracja timera2 ##############
		TCCR2 |= (0<<CS22) | (1<<CS21) | (1<<CS20); // Ÿród³em CLK, preskaler 32
		TCNT2 = timer2;//          //Pocz¹tkowa wartoœæ licznika
		//###########################################		
		//######## konfiguracja timera1 ##############
		TCCR1B = (1<<CS12|1<<WGM12);
		OCR1A = 15265-1; //dla 8Mhz ////// 0,5 s
		//###########################################	
		sei();//Globalne uruchomienie przerwañ
	
		SK1_EN_0; //poczatkowe wartosci
		SK1_DIR_1;
		SK2_EN_0;
		SK2_DIR_1;
		TR_SPRE_1;
		TR_WENT_1;
		TR_EOWZ_ZBIO_0;
		TR_EOZW_SPOW_0;
		TR_A_0;
		TR_EOZR_TUBA_0;		
		
		LCD_Initalize();
		LCD_GoTo(0,0);
		LCD_WriteText("Ps.00rpm|Ak00.0V");
		LCD_GoTo(8,1);
		LCD_WriteText(time);
		
	for(;;)
	{
		if(bit_is_clear(PINE, SPR_PRES))
		{
			PG1_1;
		}
		if(bit_is_set(PINE, SPR_PRES))
		{
			PG1_0;		
		}

		//----ZEGAR
		if(!(PING & (1<<SET_HOUR)))
		{
			hours++;
			if(hours > 23)
			hours = 0;
		}
		if(!(PING & (1<<SET_MINUTE)))
		{
			minutes++;
			if(minutes > 59)
			minutes = 0;
		}
		//----
		//##############USREDNIANIE

			ADCSRA |= (1<<ADSC);							//uruchomienie pojedynczej konwersji
			while(ADCSRA & (1<<ADSC));						//czeka na zakoñczenie konwersji
			ADMUX=(0<<MUX3)|(0<<MUX2)|(0<<MUX1)|(0<<MUX0);			// wybór kana³u ADC0
			//pomiar_ADC0=ADC;
			pomiar_ADC0=1000;
			ADMUX=(0<<MUX3)|(0<<MUX2)|(1<<MUX1)|(0<<MUX0); // wybór kana³u ADC2
			ADCSRA |= (1<<ADSC);
			while(ADCSRA & (1<<ADSC));
		//for(int a, a++, a==5)
		//{
		//	tablicarpm[a]=pomiar_ADC0;
		//}
			if(timeChanged05)
			{		
				pomiar_ADC2=ADC;
				//pomiar_ADC2=500;
				timer2=pomiar_ADC0/5;
				obroty2=9375/(2*(255-timer2));
				wolt=0.0138*pomiar_ADC2-0.0332;
				LCD_GoTo(0,0);
				LCD_WriteText("Ps.00rpm|Ak00.0V");
				itoa(obroty2, bufor, 10);
				LCD_GoTo(3,0);
				LCD_WriteText(bufor);
				dtostrf(wolt, 3, 1, bufor2);
				LCD_GoTo(11,0);
				LCD_WriteText(bufor2);
				timeChanged05 = false;
			}
			if(timeChanged)
			{
				LCD_update_time();
				timeChanged = false;
			}	
	}	
}
//-----funcja LCD_update_time	
void LCD_update_time()
{
	unsigned char temp;
	LCD_GoTo(8,1);
	itoa(hours/10,temp,10);
	LCD_WriteText(temp);
	itoa(hours%10,temp,10);
	LCD_WriteText(temp);
	LCD_WriteText(":");
	
	itoa(minutes/10,temp,10);
	LCD_WriteText(temp);
	itoa((minutes%10),temp,10);
	LCD_WriteText(temp);
	LCD_WriteText(":");
	
	itoa(seconds/10,temp,10);
	LCD_WriteText(temp);
	itoa(seconds%10,temp,10);
	LCD_WriteText(temp);
}
//-----
//############ Procedura obs³ugi przerwania od przepe³nienia timera0 ############
ISR(TIMER0_OVF_vect) //SILNIK 1 %%%% POMPA
{
	TCNT0 = timer0;			//Pocz¹tkowa wartoœæ licznika

									//suma modulo 2 (XOR) stanu poprzedniego
		PORTE ^=(1<<PE2);			//na porcie i pinu(zmiana stanu na przeciwny)				
							
}
//##############################################################################
//############ Procedura obs³ugi przerwania od przepe³nienia timera2 ############
ISR(TIMER2_OVF_vect) //SILNIK 2 %%%% POSUW
{
	TCNT2 = timer2;			//Pocz¹tkowa wartoœæ licznika

								//suma modulo 2 (XOR) stanu poprzedniego
	PORTE ^=(1<<PE5);			//na porcie i pinu(zmiana stanu na przeciwny) 
	
}
//##############################################################################
//############ Procedura obs³ugi compare timera1 ############
ISR(TIMER1_COMPA_vect)
{
	licz_ob1++;
	if(licz_ob1==2)
	{
	licz_ob1=0;
	seconds++;
	if(seconds == 60)
	{
		seconds = 0;
		minutes++;
	}
	if(minutes == 60)
	{
		minutes = 0;
		hours++;
	}
	if(hours > 23)
	hours = 0;
	timeChanged = true;
	}
	licz_ob++;
	if (licz_ob==4)
	{
		licz_ob=0;
		timeChanged05 = true;
	}
	
}
//##############################################################################


