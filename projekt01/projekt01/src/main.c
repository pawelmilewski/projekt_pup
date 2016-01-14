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
volatile uint16_t pomiar_ADC0;//odczyt wartosci z przetwornika A/C - port ADC0 PF0
volatile uint16_t pomiar_ADC2;//odczyt wartosci z przetwornika A/C - port ADC2 PF2
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
bool ChangedADC = true;
bool AktuADC0 = true;
bool AktuADC2 = true;
char time[] = "00:00:00";
#define SET_HOUR		3
#define SET_MINUTE		4
unsigned char i; 
//
#define SPR_PRES PE6
#define ON_OFF_ALL PD0
int obroty2;
float wolt;
int tablicarpm[16]={0};
int tablicawol[16]={0};
int sr_ADC0;
int sr_ADC2;
int a=0, b=0, c=0, d=0, e=0;
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
		//ADC
		//DDRF=0b00000000;
		//PORTF=0b00000000;
		//
		PORTG = (1<<SET_HOUR | 1<<SET_MINUTE); //piny zegara
		//######## konfiguracja ADC ##############
		ADCSRA=(1<<ADEN)|(1<<ADPS0)|(1<<ADPS1)|(1<<ADPS2);
		//ustawienie bitu ADEN=1 - wlaczenie  przetwornika A/C				
		// ustawienie preskalera na 64 // ustawienie
		// czestotliwosc taktowania przetwornika A/C, f=8Mhz/64
		
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
		OCR1A = 3053-1; //dla 8Mhz ////// 0,5 s
		//###########################################	
		
	
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
	sei();//Globalne uruchomienie przerwañ	
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

		if (AktuADC0)
		{
			ADCSRA |= (1<<ADSC);							//uruchomienie pojedynczej konwersji
			while(ADCSRA & (1<<ADSC));						//czeka na zako?czenie konwersji
			ADMUX=(0<<MUX3)|(0<<MUX2)|(0<<MUX1)|(0<<MUX0);			// wyb?r kana?u ADC0
			pomiar_ADC0=ADC;
			ADMUX=0;
			AktuADC0 = false;
			
		}
		if (AktuADC2)
		{
			ADCSRA |= (1<<ADSC);							//uruchomienie pojedynczej konwersji
			while(ADCSRA & (1<<ADSC));						//czeka na zako?czenie konwersji
			ADMUX=(0<<MUX3)|(0<<MUX2)|(1<<MUX1)|(0<<MUX0);			// wyb?r kana?u ADC0
			pomiar_ADC2=ADC;
			ADMUX=0;
			AktuADC2 = false;
		}




	
		if (ChangedADC)
		{

			b++;
			tablicarpm[b]=pomiar_ADC0;
			tablicawol[b]=pomiar_ADC2;
			if(b==11)
			{	
				sr_ADC0=(tablicarpm[1]+tablicarpm[2]+tablicarpm[3]+tablicarpm[4]+tablicarpm[5]+tablicarpm[6]+tablicarpm[7]+tablicarpm[8]+tablicarpm[9]+tablicarpm[10]+tablicarpm[11])/b;
				sr_ADC2=(tablicawol[1]+tablicawol[2]+tablicawol[3]+tablicawol[4]+tablicawol[5]+tablicawol[6]+tablicawol[7]+tablicawol[8]+tablicawol[9]+tablicawol[10]+tablicawol[11])/b;
				timer2=sr_ADC0/5;
				obroty2=9375/(2*(255-timer2));
				wolt=0.0138*sr_ADC2-0.0332;
				b=0;					
			}
			ChangedADC = false;
		}

		
		if(timeChanged05)
		{		
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
	e++;
	if (e==100)
	{
		AktuADC0 = true;
	}
	if (e==200)
	{
		e==0;
		AktuADC2 = true;
	}
			
		
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
	if(licz_ob1==10)
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
		{
		hours = 0;
		}
	timeChanged = true;
	}
	//	
	licz_ob++;	
	if (licz_ob==20)
	{
		licz_ob=0;
		timeChanged05 = true;
	}
	ChangedADC = true;
	//
}

