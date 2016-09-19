/*
 * A custom-made uart to usb serial connected made from 2 arduino nano 
 * Copyright (C) 2016 Jeune Prime M. Origines <primeyo2004@yahoo.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * */

/*  Description:
 *   1) This code lets the Arduino device run as either SPI Master or Slave Role
 *        Pin 2 = HIGH  -> Run as SPI Master
 *        Pin 2 = LOW   -> Run as SPI Slave
 *   2) Data received from SPI connection will be queued to its internal buffer and then forwared to its UART connection (e.g. External Device)
 *   3) Data received from its UART connection will be queued to its internal buffer and then forwarded to its SPI connection (i.e. SPI Master or Slave Device)
 *   4) This SPI Master (Slave) must be connected to the corresponding SPI Slave (Master) device
 *   5) This code/set-up has been tested workin on 2 Arduino Nanos
 *   
 */

 #include <string.h>
 #include "Firmata.h"

#define UART_BAUD_RATE    115200
#define INVALID_DATA      0xff
#define ROLE_PIN          2    // Master = HIGH, Slave = LOW
 
// spi data buffer
unsigned char spi_buffer [500];
int  spi_data_pos = 0;

// uart data buffer
unsigned char uart_buffer [500];
int  uart_data_pos = 0;

bool spi_master_role = false;

unsigned char spi_read_byte  = INVALID_DATA;
volatile bool spi_send_inprogress  = true;

// helper functions for spi buffer
inline void init_spi_buffer() {
   memset(spi_buffer,0,sizeof(spi_buffer));
   spi_data_pos = 0;
}

inline bool queue_spi_data(unsigned char uc) {
  
   if ( spi_data_pos < (sizeof (spi_buffer) - 1) ) {
      spi_buffer [ spi_data_pos++ ] = uc;
      return true;
   }

   return false;
}

inline bool dequeue_spi_data(unsigned char *puc) {
  
   if ( spi_data_pos > 0 && spi_data_pos < sizeof (spi_buffer) ) {
      *puc = spi_buffer [ --spi_data_pos ];
      return true;
   }

   return false;
}

// helper functions for uart  buffer
inline void init_uart_buffer() {
   memset(uart_buffer,0,sizeof(uart_buffer));
   uart_data_pos = 0;
}

inline bool queue_uart_data(unsigned char uc) {
  
   if ( uart_data_pos < (sizeof (uart_buffer) - 1) ) {
      uart_buffer [ uart_data_pos++ ] = uc;
      return true;
   }

   return false;
}

inline bool dequeue_uart_data(unsigned char *puc) {
  
   if ( uart_data_pos > 0 && uart_data_pos < sizeof (uart_buffer) ) {
      *puc = uart_buffer [ --uart_data_pos ];
      return true;
   }

   return false;
}


void setup (void) {

  pinMode(ROLE_PIN, INPUT);
  
  if ( digitalRead(ROLE_PIN) == HIGH) {
     spi_master_role = true;
  }

  Serial.begin(UART_BAUD_RATE);

  while (!Serial) {}

  init_spi_buffer();
  init_uart_buffer();

  cli();
  
  
  if ( spi_master_role  ) {
      // Initialize as SPI Master  
  
      digitalWrite(SS, HIGH);  // ensure SS stays high for now

      
      // Put SCK, MOSI, SS pins into output mode
      // also put SCK, MOSI into LOW state, and SS into HIGH state.
      // Then put SPI hardware into Master mode and turn SPI on
      pinMode(SS, OUTPUT);

      // Master
      SPCR |= _BV(MSTR); 

      // Enable SPI
      SPCR |= _BV(SPE);

      // clock rate Fosc/8 
      SPCR |= _BV(SPR0); 
      SPCR |= _BV(SPI2X); 

      // Enable Interrupt (Asynchronous)
      SPCR |= _BV(SPIE);
      
      pinMode(SCK, OUTPUT);                                         
      pinMode(MOSI, OUTPUT); 

      digitalWrite(SS, LOW);
  }
  else {
      // Initialize as SPI Slave
      SPCR |= _BV(SPE);

      // have to send on master in, *slave out*
      pinMode(MISO, OUTPUT);
      pinMode(MOSI, INPUT);

      // clock rate Fosc/8 
      SPCR |= _BV(SPR0); 
      SPCR |= _BV(SPI2X); 

      // Enable Interrupt (Asynchronous)
      SPCR |= _BV(SPIE);
  }
       
  spi_read_byte  = INVALID_DATA;
  spi_send_inprogress  = true;
  SPDR = INVALID_DATA;

  sei();
  
}  // end of setup

// SPI interrupt routine
ISR (SPI_STC_vect) {
   spi_read_byte = SPDR;  
   spi_send_inprogress = false;
}  


void loop (void) {
    unsigned char c;

    if (spi_send_inprogress == false){
       
        if ( spi_master_role  ) {
            digitalWrite(SS, HIGH);
        }

        if (spi_read_byte != INVALID_DATA){
            queue_spi_data(spi_read_byte);
        }

        if (!dequeue_uart_data(&c)){
            c = INVALID_DATA;
        }

        if ( spi_master_role  ) {
           digitalWrite(SS, LOW);
        }


        cli();
         
        spi_send_inprogress = true;
        SPDR = c; 
        
        sei();
    }

    if (Serial.available()){
       c = Serial.read();
       queue_uart_data(c);
    }

    if (dequeue_spi_data(&c)){
        Serial.write(c);
    }
}  // end of loop

