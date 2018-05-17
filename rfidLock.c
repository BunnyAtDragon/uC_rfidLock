/*	**	**	**	**	**	**	**	**	**	**	** **	**	**	**	**	**	**	**	**	**	**
Filename:		rfidLock.c
Version:		0.7 rev1
Description:	Zamek na karty magnetyczne
				Transmisja RF->AT via SPI

	Pinout:
	
	|	PB0		r_LED		|	PC0		_clr_ADC0		|	PD0		LCD_RS
	|	PB1		y_LED		|	PC1		_clr_ADC1		|	PD1		LCD_RW
	|	PB2		rf_SDA		|	PC2		_clr_ADC2		|	PD2		LCD_E
	|	PB3		rf_MOSI		|	PC3		_clr_ADC3		|	PD3		_LOCK_SIGNAL
	|	PB4		rf_MISO		|	PC4		_clr_SDA		|	PD4		LCD_D4
	|	PB5		rf_SCK		|	PC5		_clr_SCL		|	PD5		LCD_D5
	|	PB6		ex_XTAL		|	PC6		RESET			|	PD6		LCD_D6
	|	PB7		ex_XTAL		|							|	PD7		LCD_D7
	
	Fuse & Lock Bits:
	
	|	FF C9				|	3F
	|	FF C1	- lock EEPROM
	
Revision History:		
				
				
					____Obsluga pamieci EEPROM
					____Obsluga tagow credit
					____Obsluga RTC
					____Obsluga klawiatury (6-klawiszowa)
					____Obsluga ekspandera
				
				
				v0.6r2	Przepisanie czesci funkcji, oczyszczenie kodu
				v0.6r1	Obsluga czyrnika MFRC522
				v0.5	Obsluga wyswietlacza LCD 16x2 (HD44780)
				v0.4r3	Wykorzystanie zewnetrzenego kwarcu 16MHz.
				v0.4r1	Ustawienie pinow oraz fusebitow, dodany opis, oczyszczony kod.
				v0.3	Rozszerzenie obslugi zamka elektrycznego.
				v0.2	Dodana obsluga zamka elektrycznego (lock.c).
				v0.1	Otwarcie projektu.
	
**	**	**	**	**	**	**	**	**	**	** **	**	**	**	**	**	**	**	**	**	**	*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <stdlib.h>

#include "inc/lock.h"
#include "inc/spi.h"
#include "inc/hd44780.h"
#include "inc/mfrc522.h"

int main(void)
{
	DDRB = 0x2F;				// Kierunek przesylu informacji RC522
	PORTB = 0x01;				// Hihg-state na r_LED (PB0)
	DDRD = 0xFF;				// Kierunek przesylu informacji (1 - out, 0 - in)
	PORTD = 0b11110101;			// LCD R/W (PD1) i output (PD3) na low-state
//	DDRD = (1<<DDB3);			// Kierunek przesylu informacji		// lock
	
	lcd_init();
	lcd_clrscr();
	_delay_ms(100);
	
	spi_init();					// Inicjalizacja SPI
	mfrc522_init();				// Inicjalizacja MFRC522

	chk_ver();					// Sprawdzanie wersji czytnika
	sei();
	_delay_ms(1000);
	
	while (1) 
    {
		lcd_clrscr();
	//	if(byte == 0x90 || byte == 0x91 || byte == 0x92)
			lcd_puts("Przyloz karte");
	//	else
	//		lcd_puts("Zrestartuj mikrokontroler");		// Nie zatrzymywac uC, zeby podtrzymac reszte funkcji
		_delay_ms(100);
		
		if(fake_timer%10 == 0)							// Debug
		{
			lcd_goto(0x40);
			lcd_puts("test ");
			lcd_puts((int)fake_timer);
			_delay_ms(100);
		}
		request();				// Otwieranie zamka
    }
	return 0;
}