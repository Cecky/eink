#include "bootloader.h"

void bootloader_reset_wdt(void)
{
  if((MCUSR & (1 << WDRF)))
	{
  	MCUSR &= ~(1<<WDRF);
    wdt_disable();
	}
}

void bootloader_start(void)
{
  wdt_enable(WDTO_15MS);
  while(1);
}
