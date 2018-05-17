#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "lock.h"
#include "hd44780.h"
#include "mfrc522.h"
#include "ee_util.h"

#define BLUE 	2
#define WHITE 	3

uint8_t byte;
uint8_t str[MAX_LEN];
uint8_t person_recognised = 0;

	/*	**	**	**	**	**	**	**	**	**	**	**
		Zapisaæ numery kart do pamieci EEPROM
		Pamiec na karty otwieracjace 10*6B = 60B
		Zbic w strukture: NAME + CARD ID (1+5)
		Adresy: 451-510
	**	**	**	**	**	**	**	**	**	**	**	*/
uint8_t Blue[5]  ={0xd0,0x40,0x7c,0x89,0x65};		// TAG
uint8_t White[5] ={0x20,0xb6,0x9b,0x7c,0x71};		// KARTA

int fake_timer = 0;


void open_lock(void)
{
	_delay_ms(10);
	PORTD = (1<<PD3);	// PD3 na high-state otwiera zamek
	_delay_ms(10);
	PORTB = (0<<PB0);	// PB0 sygnalizujace zamkniety zamek
	PORTB = (1<<PB1);	// PB1 sygnalizujace otwarty zamek
	
	_delay_ms(10);
	lcd_clrscr();
	lcd_goto(0x40);
	lcd_puts("Zamek otwarty");
	
	_delay_ms(5000);
	PORTD = (0<<PD3);
	_delay_ms(10);		// Opoznienie, bo nie wyrabia z puszczeniem napiecia
	PORTB = (0<<PB1);
	PORTB = (1<<PB0);
	
	_delay_ms(10);
	lcd_clrscr();
	lcd_goto(0x40);
	lcd_puts("Zamek zamkniety");
}

void chk_ver(void)
{
	byte = mfrc522_read(VersionReg);
	
	if(byte == 0x92)
	{
		lcd_clrscr();
		lcd_puts("Wykryto czytnik");
		lcd_goto(0x40);
		lcd_puts("MIFARE RC522v2");
	}
	else if(byte == 0x91 || byte==0x90)
	{
		lcd_clrscr();
		lcd_puts("Wykryto czytnik");
		lcd_goto(0x40);
		lcd_puts("MIFARE RC522v1");
	}
	else
	{
		lcd_clrscr();
		lcd_puts("Nie wykryto czytnika");
		lcd_goto(0x40);
		lcd_puts("Zrestartuj mikrokontroler");
		_delay_ms(5000);
	}
}

void request(void)
{
	byte = mfrc522_request(PICC_REQALL,str);
		
	if(byte == CARD_FOUND)
	{
		byte = mfrc522_get_card_serial(str);
		
		if(byte == CARD_FOUND)
		{
			person_recognised = 1;
			for(byte=0;byte<5;byte++)
			{
				if(Blue[byte] != str[byte])
					break;
			}
			if(byte == 5)
				person_recognised = BLUE;
			else
			{
				for(byte=0;byte<5;byte++)
				{
					if(White[byte] != str[byte])
						break;
				}
				if(byte == 5)
					person_recognised = WHITE;
			}
			gateway();
		}
		else
		{
			lcd_clrscr();
			lcd_puts("Blad odczytu");
		}
	}
	
	fake_timer++;
}

void gateway(void)
{
	char bbuf[2];
	switch(person_recognised)
	{
		case BLUE : 
		{
			lcd_clrscr();
			lcd_puts("Uzytkownik BLUE");
			lcd_goto(0x40);
			for(byte=0;byte<5;byte++)
			{
				sprintf(bbuf, "%02X", str[byte]);
				lcd_puts(bbuf);
				lcd_puts(" ");
			}
			log_struct();
//			ee_save_log(&ramv[log_count-1]);
			_delay_ms(3000);
			open_lock();
			break;
		}
		case WHITE : 
		{
			lcd_clrscr();
			lcd_puts("Uzytkownik WHITE");
			lcd_goto(0x40);
			for(byte=0;byte<5;byte++)
			{
				sprintf(bbuf, "%02X", str[byte]);
				lcd_puts(bbuf);
				lcd_puts(" ");
			}
			_delay_ms(3000);
			open_lock();
			break;
		}
		default :
		{
			lcd_clrscr();
			lcd_puts("Brak uzytkownika");
			lcd_goto(0x40);

			for(byte=0;byte<5;byte++)
			{
				sprintf(bbuf, "%02X", str[byte]);
				lcd_puts(bbuf);
				lcd_puts(" ");
			}
			_delay_ms(3000);
			break;
		}
	}
}

