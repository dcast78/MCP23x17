/****************************************
  * basic i2c demo for mcp23017
  *****************************************/
 
 #include <stdlib.h>
 #include <stdio.h>
 #include <fcntl.h>
 #include <sys/ioctl.h>
 #include <linux/i2c-dev.h>
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <unistd.h>
 #include <stdint.h>

 // MCP23x17 Registers
 #define IOCON  0x0A
 #define IODIRA	0x00
 #define IPOLA	0x02
 #define GPINTENA 0x04
 #define DEFVALA 0x06
 #define INTCONA 0x08
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

 //MPC23017 address
 #define DEVICE_ADDRESS 0x20 

 int i2c_fd;

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
  write(fd,"falling\n",8);// i valori possibili sono rising/falling/both/none consigliato falling
  close(fd);

  fd = open("/sys/class/gpio/gpio25/value",O_RDWR);
  return fd;
 }

 int readByte(uint8_t regist){
	uint8_t pkt_d[2];
	pkt_d[0] = regist;
        pkt_d[1] = 0;
	if (write(i2c_fd, &pkt_d, 1) != 1) {
		perror("write error in readByte");
		return -1;
	}
        if (read(i2c_fd, &pkt_d, 2) != 2) {
		perror("read error in readByt");
		return -1;
	}
        return (int) pkt_d[0];
 }

 int writeByte( uint8_t regist, uint8_t value){
	uint8_t pkt_d[2];
        pkt_d[0] = regist;
	pkt_d[1] = value;
        if (write(i2c_fd, &pkt_d, 2) != 2) {
		perror("write error in writeByte");
		return -1;
	}
        return 1;
 }


 int i2c_open(char* dev)
 {
	if((i2c_fd = open(dev, O_RDWR)) < 0){
		printf("error opening %s\n",dev);
		return -1;
	}
        if (ioctl(i2c_fd, I2C_SLAVE, DEVICE_ADDRESS) < 0) {
		perror("error setting I2C_SLAVE");
		return -1;
	}
        return 0;
 }
 int main(int argc, char* argv[])
 {
    unsigned char data = 0xAF;
    struct timeval tv;
    int pin25;
    fd_set set, setbackup;
    char pin25buf[1024];

    pin25 = pinsetup();

    FD_ZERO(&set);
    FD_SET(pin25,&set);

    int val;
        if(argc <= 1){
		printf("too few args, try %s /dev/i2c-0\n",argv[0]);
		return -1;
	}
	// open and configure i2c channel. (/dev/i2c-0 for example)
	if(i2c_open(argv[1]) < 0){
		printf("i2c_open failed\n");
		return -1;
	}
        
        writeByte(GPPUA, 0xFF); // PORTA pullups on
	      writeByte(IODIRA, 0xFF); // PORTA for inputs
        writeByte(GPINTENA,0xFF); //enable interrupt on PORTA
        writeByte(DEFVALA,0xFF); //Set default comparition value( for another configuration of interrupts )
        writeByte(INTCONA,0x00);  // set the registry for sending interrupt to status changes
        writeByte(GPIOA,0x00); //set GPIOA
	      writeByte(IODIRB,0x00); // PORTB for outputs
        
        while (1)
        {
            tv.tv_sec = 30;
            tv.tv_usec = 0;
            val = select(pin25+1, NULL, NULL, &set, &tv);
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
                read(pin25,NULL,0);
               }
              else write(1,"???\n",4);
              }
            else {
		 FD_ZERO(&set);
		 FD_SET(pin25,&set);
		 write(1,"TIMEOUT\n",8);
		 }
          // Port A -> Inputs
          data = readByte (GPIOA) ;
	  data = (data^0xff); // metto in xor con una maschera di 1 per avere il giusto valore in uscita
          printf("RECEIVED: %.2X\n",data);
	  // Port B -> Output
	  writeByte(GPIOB,data);

         }
         close(i2c_fd);
         return 0;
 }
