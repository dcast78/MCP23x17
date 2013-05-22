/****************************************
 * basic SPI demo for mcp23s17
 *
 *****************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#define CMD_WRITE	0x40
#define CMD_READ 	0x41
static char	*spiDevice = "/dev/spidev0.0" ;
static uint8_t spiMode = 0 ;
static uint8_t spiBPW = 8 ;
static uint32_t spiSpeed = 5000000 ;
static uint16_t spiDelay = 0;
// MCP23S17 Registers
#define IOCON	0x0A
#define IODIRA	0x00
#define IPOLA	0x02
#define GPINTENA 0x04
#define DEFVALA	 0x06
#define INTCONA	 0x08
#define GPPUA    0x0C
#define INTFA    0x0E
#define INTCAPA  0x10
#define GPIOA	 0x12
#define OLATA	 0x14
#define IODIRB   0x01
#define IPOLB    0x03
#define GPINTENB 0x05
#define DEFVALB  0x07
#define INTCONB  0x09
#define GPPUB    0x0D
#define INTFB    0x0F
#define INTCAPB  0x11
#define GPIOB    0x13
#define OLATB    0x15

int spi_fd;

int readconf() 
{

}

int pinsetup ()
{
  int fd = 0;

  fd = open("/sys/class/gpio/export",O_WRONLY);
  write(fd,"25\n",3);
  close(fd);

  fd = open("/sys/class/gpio/gpio25/direction",O_WRONLY);
  write(fd,"in\n",3);
  close(fd);

  fd = open("/sys/class/gpio/gpio25/edge",O_RDWR);  
  write(fd,"falling\n",8);				// i valori possibili sono rising/falling/both/none consigliato falling
  close(fd);
  
  fd = open("/sys/class/gpio/gpio25/value",O_RDWR);
  
  return fd;
}


static uint8_t readByte (uint8_t reg)
{
	uint8_t tx [4] ;
	uint8_t rx [4] ;
	struct spi_ioc_transfer spi ;

	tx [0] = CMD_READ ;
	tx [1] = reg ;
	tx [2] = 0 ;

	spi.tx_buf =(unsigned long)tx ;
	spi.rx_buf =(unsigned long)rx ;
	spi.len =3;
	spi.delay_usecs = spiDelay ;
	spi.speed_hz = spiSpeed ;
	spi.bits_per_word = spiBPW ;
	ioctl (spi_fd, SPI_IOC_MESSAGE(1), &spi) ;
	return rx [2] ;
}
static void writeByte (uint8_t reg, uint8_t data)
{
	uint8_t spiBufTx [3] ;
	uint8_t spiBufRx [3] ;
	struct spi_ioc_transfer spi ;
	spiBufTx [0] = CMD_WRITE ;
	spiBufTx [1] = reg ;
	spiBufTx [2] = data ;
	spi.tx_buf
		= (unsigned long)spiBufTx ;
	spi.rx_buf
		= (unsigned long)spiBufRx ;
	spi.len
		=3;
	spi.delay_usecs = spiDelay ;
	spi.speed_hz = spiSpeed ;
	spi.bits_per_word = spiBPW ;
	ioctl (spi_fd, SPI_IOC_MESSAGE(1), &spi) ;
}

static void ruota_led()
{
        uint8_t leds;
        leds = readByte (GPIOA) ;
        if (leds==4 || leds==0) {
                writeByte(GPIOA,128);
        } else {
                writeByte(GPIOA,leds/2);
        }
}

static void send_message(uint8_t i) {
	char messaggio[255] = "Segnalazione: ";
	char comando[300] = "/usr/bin/gammu sendsms TEXT ";
	switch(i)
	{
	case 0:
	strcat (messaggio, "Porta aperta");
	break;
	case 1:
	strcat (messaggio, "Finestra aperta");
	break;
	}
	strcat (comando," numero_telefonico ");
	strcat (comando," -text '");
	strcat (comando,messaggio);
	strcat (comando,"'");
	system(comando);
	//printf("%s",comando);
}

/*spi_open
 *
 - Open the given SPI channel and configures it.
 *
 - there are normally two SPI devices on your PI:
 *
 /dev/spidev0.0: activates the CS0 pin during transfer
 *
 /dev/spidev0.1: activates the CS1 pin during transfer
 *
 */
int spi_open(char* dev)
{
	if((spi_fd = open(dev, O_RDWR)) < 0){
		printf("error opening %s\n",dev);
		return -1;
	}
	return 0;
}
int main(int argc, char* argv[])
{
    unsigned char data = 0xAF;
    struct timeval tv;
    int pin25;
    uint8_t byte,i;
    char byte_ingressi[9];
    fd_set set, setbackup;
    char pin25buf[1024];
    
    pin25 = pinsetup();
    FD_ZERO(&set);
    FD_SET(pin25,&set);
    setbackup = set;
    
    int val;	
	if(argc <= 1){
		printf("too few args, try %s /dev/spidev0.0\n",argv[0]);
		return -1;
	}
	// open and configure SPI channel. (/dev/spidev0.0 for example)
	if(spi_open(argv[1]) < 0){
		printf("spi_open failed\n");
		return -1;
	}
	//Settaggio come uscite	
	writeByte (IODIRA, 0x00) ;
	// Port A -> Outputs
	writeByte (GPIOA, 0x00) ;
	//writeByte (IPOLB, 0x01) ; // invert lsb
	//writeByte (GPPUB, 0xFF) ; // enable pullups.
        writeByte(GPPUA, 0xFF); // set port A pullups on
        writeByte(GPPUB, 0xFF); // set port B pullups on
	//Settaggio come ingressi	
	writeByte (IODIRB, 0xFF) ;

	writeByte (GPINTENB,0xFF); //Abilita l'interrupt sul portB
	writeByte (DEFVALB,0xFF); //Setta il valore di default dei pin del port B se un un pin non è a uno emette interrupt
	writeByte (INTCONB,0x00);  //Setta il valore di per il confronto se a 1 compara con il valore di DEFVAL altrimenti controlla le variazioni di stato


        while (1)
        {
            tv.tv_sec = 30;
            tv.tv_usec = 0;

            val = select(pin25+1, NULL, NULL, &set, &tv);

        
            set = setbackup;
            if(val > 0)
            {
              if(FD_ISSET(pin25,&set))
              {
                write(1,"evento su pin 25\n",17);
                
                /* antirimbalzo */
                tv.tv_sec = 0;
                tv.tv_usec = 50000;
                val = select(0, NULL, NULL, NULL, &tv);
                /* fine antirimbalzo */  
            
                lseek(pin25,0,SEEK_SET);
                read(pin25,pin25buf,1024);
              }
              else write(1,"???\n",4);
             }
            else
            if(val < 0) write(1,"SIGNAL NON BLOCCATO\n",20);
            else write(1,"TIMEOUT\n",8);
            
            
         // Port B -> Inputs
         data = readByte (GPIOB) ;
	 byte_ingressi[0] = 0;
         for (i = 0; i < 8; i++) {
                if (data & (1 << i)) {
                        strcat (byte_ingressi,"1");
                } else {
                        strcat (byte_ingressi,"0");
			send_message(i);
		}
	 }
	  printf("%s ",byte_ingressi);
         printf("%.2X \n",data);	

	 //ruota_led();
                        
            
         }
         close(spi_fd);
         return 0;
}
