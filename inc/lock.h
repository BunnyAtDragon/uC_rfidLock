#include "mfrc522.h"

void open_lock(void);			// Otwieranie zamka
void chk_ver(void);				// Sprawdzanie wersji czytnika
void request(void);				// Rozpoznawanie kart
void gateway(void);				// Interface


extern uint8_t byte;
extern uint8_t str[MAX_LEN];
extern int fake_timer;