#include <stdint.h>
#include <avr/eeprom.h>

#include "ee_util.h"
#include "lock.h"

//uint8_t eeprom_val[505] __attribute__((section(".eeprom")));
#define LOG_CAP	64				// Ilosc struktur logow w EEPROM
#define EEPROM __attribute__((section(".eeprom")))

uint8_t log_count = 0;

typedef struct {				// 7-bajtowa struktura
     uint8_t time;				// Zajete:	7*64 = 448B
     uint8_t card[5];         	// Wolne:	512 - 448 = 64B
     uint8_t no;   				// Adresy: 2-449
} data;

data EEMEM eemv[LOG_CAP];		// Definicja struktury w EEPROM
//data EEPROM eemv;				// Definicja struktury w EEPROM
data ramv[LOG_CAP];				// Definicja struktury w RAM

void log_struct(void)
{
	ramv[log_count].no = log_count;		// Numer wejscia
	ramv[log_count].time = 0;			// Czeka na RTC :D
	for(byte=0;byte<5;byte++)
	{
		ramv[log_count].card[byte] = str[byte];
	}
	
	ee_save_log(&ramv[log_count]);		// Przepisywanie do pamieci EEPROM
	
	log_count++;
	
	if(log_count == 64)					// Max 64 pozycje logow w pamieci EEPROM
		log_count = 0;
}

	/*	**	**	**	**	**	**	**	**	**	**	** **	**	**	**	**	**	**	**	**	**	**
		eeprom_write_block();
		Adres RAM wartosci zapisywanej; adres wartosci zapisywanej w EEPROM; rozmiar danych
		
		Zapisywanie od 3 pozycji w pamieci (64 7-bitowe struktury) w adresach 2-449
		
	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/
void ee_save_log(uint8_t data)
{
	eeprom_busy_wait();
	eeprom_write_block(&ramv[log_count], &eemv[log_count*7 + 2], sizeof(data));
}

void ee_read_log(void)
{
	eeprom_busy_wait();
	// Wartosc odczytywana; adres w EEPROM; rozmiar danych	//
	eeprom_read_block(&ramv[log_count], &eemv[log_count*7 + 2], sizeof(data));
}