/*
  SD TFT picture frame
  by moty22.co.uk
  
   SD card SPI:
 * MOSI - pin 11
 * MISO - pin 12
 * CLK - pin 13
 * CS - pin 10
*/

#include <SPI.h>
#include <SD.h>

#define CS   7 // pin definition for Arduino UNO
#define DC   9
#define SDA  1
#define SCK  4
#define RST  8

  File f1;

void setup() {
  
  pinMode(SDA, OUTPUT);  //tft SDA
  pinMode(CS, OUTPUT);  //tft CS
  pinMode(SCK, OUTPUT);  //tft SCK
  pinMode(RST, OUTPUT);  //tft RESET
  pinMode(DC, OUTPUT);  //tft AO/DC

  SD.begin(10);
  
  //TFT USART-SPI
  UBRR0 = 0;
  //XCKn_DDR |= (1<<XCKn);  // Setting the XCKn port pin
  UCSR0C = (1<<UMSEL01)|(1<<UMSEL00)|(1<<UCPHA0)|(1<<UCPOL0);  // MSPI mode and SPI data mode 3.
  UCSR0B = (1<<RXEN0)|(1<<TXEN0);   // Enable receiver and transmitter.
  UBRR0 = 8;  // Set baud rate. must be at the end 

 TFTinit();

}

void loop() {

  if(SD.exists("1.bmp")){
    f1 = SD.open("1.bmp");
    display();
    delay(10000);
  }
  if(SD.exists("2.bmp")){
    f1 = SD.open("2.bmp");
    display();
    delay(10000);
  }
  if(SD.exists("3.bmp")){
    f1 = SD.open("3.bmp");
    display();
    delay(10000);
  }
  if(SD.exists("4.bmp")){
    f1 = SD.open("4.bmp");
    display();
    delay(10000);
  }
  if(SD.exists("5.bmp")){
    f1 = SD.open("5.bmp");
    display();
    delay(10000);
  }
  if(SD.exists("6.bmp")){
    f1 = SD.open("6.bmp");
    display();
    delay(10000);    
  }
    
}

void display(){
  unsigned int i=0;
  unsigned char c;

    area(0,0,127,159);
    digitalWrite(DC, HIGH); // Command Mode
    digitalWrite(CS, LOW); // Select the LCD (active low)
  
    while (f1.available()) {
      ++i;
      c=f1.read();
      if(i>53){spi(c);}
    }
    digitalWrite(CS, HIGH); // Deselect LCD (active low)
    f1.close();
}

unsigned char spi(unsigned char data)   // send character over spi - talking
{
    while ( !( UCSR0A & (1<<UDRE0)) );  /* Wait for empty transmit buffer */
    UDR0 = data;    /* Put data into buffer, sends the data */
    while ( !(UCSR0A & (1<<RXC0)) );    /* Wait for data to be received */
    return UDR0;    /* return received data from buffer */
}

void command(unsigned char cmd)
{
  digitalWrite(DC, LOW); // Command Mode
  digitalWrite(CS, LOW); // Select the LCD (active low)
  spi(cmd); // set up data on bus
  digitalWrite(CS, HIGH); // Deselect LCD (active low)
}

void send_data(unsigned char data)
{
  digitalWrite(DC, HIGH);    // data mode
  digitalWrite(CS, LOW);   // chip selected
  spi(data);  // set up data on bus
  digitalWrite(CS, HIGH);       // deselect chip
}

void TFTinit(void)
{
  unsigned char i;
  digitalWrite(RST, HIGH);   //hardware reset
  delay(200);
  digitalWrite(RST, LOW);
  delay(10);
  digitalWrite(RST, HIGH);
  delay(10);

  command(0x01); // sw reset
  delay(200);

  command(0x11); // Sleep out
  delay(200);

  command(0x3A); //color mode
  send_data(0x06);  //18 bits
  //send_data(0x05);  //16 bits
  command(0x36); //Memory access ctrl (directions)
  send_data(0x40);  //0x60=0B1100000
  //    command(0x21); //inversion on

  command(0x2D);  //color look up table
  send_data(0); for (i = 1; i < 32; i++) {
    send_data(i * 2);
  }
  for (i = 0; i < 64; i++) {
    send_data(i);
  }
  send_data(0); for (i = 1; i < 32; i++) {
    send_data(i * 2);
  }

  command(0x13); //Normal display on
  command(0x29); //Main screen turn on
}

void area(unsigned char x0,unsigned char y0, unsigned char x1,unsigned char y1)
{
  command(0x2A); // Column addr set
  send_data(0x00);
  send_data(x0);     // XSTART 
  send_data(0x00);
  send_data(x1);     // XEND

  command(0x2B); // Row addr set
  send_data(0x00);
  send_data(y0);     // YSTART
  send_data(0x00);
  send_data(y1);     // YEND

  command(0x2C); // write to RAM
}  
  
void rectan(unsigned char x0,unsigned char y0, unsigned char x1,unsigned char y1, unsigned long color) 
{
 unsigned int i;
  area(x0,y0,x1,y1);
  for(i=(y1 - y0 + 1) * (x1 - x0 + 1); i > 0; i--) {      

    digitalWrite(DC, HIGH);   // data mode
    digitalWrite(CS, LOW);
    spi(color >> 16);
    spi(color >> 8);
    spi(color);
    digitalWrite(CS, HIGH);
  }
}

void pixel(unsigned char x,unsigned char y, unsigned long color)
{
    area(x,y,x+1,y+1);
    send_data(color >> 16);
    send_data(color >> 8);
    send_data(color);
}
