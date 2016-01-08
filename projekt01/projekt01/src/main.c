#define F_CPU 8000000 //czestotliwosc taktowania uC
#include <avr/io.h>	//wykorzystane biblioteki
#include <util/delay.h>
#include <HD44780.h>
//PE0 & PE1 MOSI MISO
#define  SK1_STEP_1 PORTE|=(1<<PE2);//zdefiniowane porty
#define  SK1_STEP_0 PORTE&=~(1<<PE2);
#define  SK2_EN_1 PORTE|=(1<<PE3);
#define  SK2_EN_0 PORTE&=~(1<<PE3);
#define  SK2_DIR_1 PORTE|=(1<<PE4);
#define  SK2_DIR_0 PORTE&=~(1<<PE4);
#define  SK2_STEP_1 PORTE|=(1<<PE5);
#define  SK2_STEP_0 PORTE&=~(1<<PE5);

//#define  SPR_PRES_1 PORTE|=(1<<PE6); //INT6
//#define  SPR_PRES_0 PORTE&=~(1<<PE6);
//#define  PE7_1 PORTE|=(1<<PE7); //INT7
//#define  PE7_0 PORTE&=~(1<<PE7);

#define  PB0_1 PORTB|=(1<<PB0);
#define  PB0_0 PORTB&=~(1<<PB0);
#define  SK1_EN_1 PORTB|=(1<<PB3); 
#define  SK1_EN_0 PORTB&=~(1<<PB3);
#define  SK1_DIR_1 PORTB|=(1<<PB2);
#define  SK1_DIR_0 PORTB&=~(1<<PB2);
#define  PB4_1 PORTB|=(1<<PB4);
#define  PB4_0 PORTB&=~(1<<PB4);
#define  PB5_1 PORTB|=(1<<PB5);
#define  PB5_0 PORTB&=~(1<<PB5);
#define  PB6_1 PORTB|=(1<<PB6);
#define  PB6_0 PORTB&=~(1<<PB6);
#define  PB7_1 PORTB|=(1<<PB7);
#define  PB7_0 PORTB&=~(1<<PB7);

//PG3 & PG4 TOSC2 TOSC1
//#define  PG2_1 PORTG|=(1<<PG2);//ALE
//#define  PG2_0 PORTG&=~(1<<PG2);
//#define  PG1_1 PORTG|=(1<<PG1);//RD
//#define  PG1_0 PORTG&=~(1<<PG1);
//#define  PG0_1 PORTG|=(1<<PG0);//WR
//#define  PG0_0 PORTG&=~(1<<PG0);

//#define  ON/OFF_ALL_1 PORTD|=(1<<PD0); //INT0
//#define  ON/OFF_ALL_0 PORTE&=~(1<<PD0);
//#define  O/O_vA_1 PORTD|=(1<<PD1); //INT1
//#define  O/O_A_0 PORTD&=~(1<<PD1);

//WYSWIETLLACZ DIS_D2=PD2; DIS_D3=PD3; DIS_D4=PD4; DIS_D5=PD5; DIS_D6=PD6; DIS_D7=PD7; DIS_RS=PA0; DIS_R/W=PA1; DIS_E=PA2; DIS_C1=PA3; DIS_C2=PA4; DIS_RST=PA5; DIS_D0=PA6; DIS_D1=PA7; DIS_ON/OFF=PC7
//ADC0=PF0; ADC1=PF1; ADC2=PF2; ADC3=PF3; ******* ADC4(TCK)=PF4; ADC5(TMS)=PF5; ADC6(TDO)=PF6; ADC7(TDI)=PF7;

#define  TR_B_1 PORTC|=(1<<PC6);
#define  TR_B_0 PORTC&=~(1<<PC6);
#define  TR_A_1 PORTC|=(1<<PC5);
#define  TR_A_0 PORTC&=~(1<<PC5);
#define  TR_EOZW_SPOW_1 PORTC|=(1<<PC4);
#define  TR_EOZW_SPOW_0 PORTC&=~(1<<PC4);
#define  TR_EOWZ_ZBIO_1 PORTC|=(1<<PC3);
#define  TR_EOZW_ZBIO_0 PORTC&=~(1<<PC3);
#define  TR_EOZR_TUBA_1 PORTC|=(1<<PC2);
#define  TR_EOZR_TUBA_0 PORTC&=~(1<<PC2);
#define  TR_SPRE_1 PORTC|=(1<<PC1);
#define  TR_SPRE_TUBA_0 PORTC&=~(1<<PC1);
#define  TR_WENT_1 PORTC|=(1<<PC0);
#define  TR_WENT_0 PORTC&=~(1<<PC0);
int main(void)
{//petla glowna
	DDRE=0x3C; // port e jako wyjœcie 00111100
	PORTE=0x3C;
	DDRB=0xFD; 
	PORTB=0xFD;
	
	SK1_EN_0;
	SK1_DIR_1;
	SK2_EN_0;
	SK2_DIR_1;

	for(;;)
	{
		/*_delay_us(300);
		SK1_STEP_1;
		SK2_STEP_1;
		_delay_us(300);
		SK1_STEP_0;
		SK2_STEP_0;*/
				LCD_Clear();
				LCD_Home();
				LCD_Initalize();
				LCD_WriteText("love");
				LCD_GoTo(0,1);
	}
}