#include "tps65185.h"

void tps65185_init(void)
{
  DDR_PWRUP |= (1<<PIN_PWRUP);
  TPS_POWERDOWN;
}

int8_t tps65185_write(uint8_t reg, uint8_t val)
{
  if(!(i2c_start(ADRESSE+I2C_WRITE))) 	// Slave bereit zum lesen?
  {
    i2c_write(reg);                     // Schreibe Addresse
    i2c_write(val);
    i2c_stop(); 
  }
  else
  {
    i2c_stop(); 
    return -1;    
  }
  
  return 0;
}

void tps65185_get_status(void)
{
  uint8_t i = 0;  

  if(!(i2c_start(ADRESSE+I2C_WRITE))) 	// Slave bereit zum lesen?
  {
   	i2c_write(0x00);                    // Buffer Startadresse zum Auslesen
    i2c_rep_start(ADRESSE+I2C_READ);    // Lesen beginnen
	
    for(i = 0; i < 0x10; i++)           // ersten 7 Byte lesen, darum ACK
    {
      printf("Byte [%02d]: 0x%02X\r\n", i, i2c_readAck());
    }
    printf("Byte [%02d]: 0x%02X\r\n", i, i2c_readNak());

    i2c_stop();                         // Beende Datentransfer
  }

}
