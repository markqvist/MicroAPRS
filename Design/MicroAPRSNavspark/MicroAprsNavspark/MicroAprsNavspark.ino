/*
Copyright 2014 Institute IRNAS Race - S56MC - Musti, musti@irnas.eu

MicroAprsNavspark is to be used with NavSpark Arduino environment, for more information visit http://navspark.com.tw

The MicroAprsNavspark is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

The MicroAprsNavspark is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with the MicroAprsNavspark. If not, see http://www.gnu.org/licenses/.
*/
#include "sti_gnss_lib.h"
#include "GNSS.h"

#define POSITIONRATE 60 // periodically send the location - in seconds
uint16_t delayer = 0;
uint8_t gpio_bit = 0;

// Generally, you shuould use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time LED was updated
float lla_deg=0;
float lla_min=0;
float llo_deg=0;
float llo_min=0;
float lla=0;
float llo=0;

void setup() {
  // put your setup code here, to run once:
  GnssConf.setNavMode(STGNSS_NAV_MODE_AUTO);
  GnssConf.setUpdateRate(STGNSS_POSITION_UPDATE_RATE_1HZ);
  GnssConf.setDopMaskMode(STGNSS_DOP_MASK_AUTO);
  GnssConf.setPdopMask(30.0);
  GnssConf.setHdopMask(30.0);
  GnssConf.setGdopMask(30.0);
  GnssConf.init(); /* do initialization for GNSS */
  gnss_gpio_set_output(gpio_bit);
  
  //set up the serail port to talk to MicroAPRS
  Serial.config(STGNSS_UART_8BITS_WORD_LENGTH, STGNSS_UART_1STOP_BITS, STGNSS_UART_NOPARITY);
  Serial.begin(9600);
  //configure the reset pin and reset MicroAPRS
  pinMode(14,OUTPUT);
  digitalWrite(14,LOW);
  delay(100);
  digitalWrite(14,HIGH);
  delay(100);
  // send settings
  configureMicroAPRS();
}

void loop() {
  // put your main code here, to run repeatedly:

}

void configureMicroAPRS(){
  Serial.println("c<CALL>"); //callsign
  Serial.println("sc7"); //SSID
  Serial.println("S"); //save
}

/*
  NOTE: "task_called_after_GNSS_update()" will be called about every second
        (for 1Hz update rate), so we display the info. here.
*/
void task_called_after_GNSS_update(void)
{
  static uint8_t val = 0;
  char buf[64];
  
  //blinking the blue LED
  if (val == 1) gnss_gpio_high(gpio_bit);
  else gnss_gpio_low(gpio_bit);
  val = 1 - val; 
  
  //triggering the position update on the defined interval
  delayer++;
  if(delayer>POSITIONRATE){
    aprs_send();
    delayer=0;
  }
  //send the counter state to PC serial port
  int len = sprintf(buf, "Counter: %d\r\n",delayer);
  gnss_uart_putline(0,(U08*)buf,len);
}

void aprs_send(){
  char buf[64];
  char lla_sign;
  char llo_sign;
  // first get the updated location
  GnssInfo.update();
  
  //check if the location has been updated - valid fix
  if (GnssInfo.isUpdated() == true) {
      //get location information
      lla=GnssInfo.location.latitude();
      llo=GnssInfo.location.longitude();
      
      //determine hemisphere
      if(lla>=0){
        lla_sign=0x4e; //capital N
      }
      else{
        lla_sign=0x53; //capital S
        lla*=-1;//make the number positive
      }
      if(llo>=0){
        llo_sign=0x45; //capital E
      }
      else{
        llo_sign=0x57; //capital W
        llo*=-1;//make the number positive
      }
      
      //convert it to deg, min format
      lla_deg=(int)lla;
      lla_min=(lla-lla_deg)*60;
      llo_deg=(int)llo;
      llo_min=(llo-llo_deg)*60;
      
      //print to serial port
      uint8_t len=sprintf(buf, "!=%02.0f%.2f%c/%03.0f%.2f%c-PHG2/3060/MicroAPRS+Navspark\r\n",lla_deg,lla_min,lla_sign,llo_deg,llo_min,llo_sign);
      Serial.print(buf);
      gnss_uart_putline(0,(U08*)buf,len);
    }
}

