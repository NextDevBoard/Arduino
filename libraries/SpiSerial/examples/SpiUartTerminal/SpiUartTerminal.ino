/*
  SpiUartTerminal - tool to help troubleshoot problems with NextDev

  This code will initialize and test the SC16IS750 UART-SPI bridge then enable you
  to send commands to the NextDev board.

 */

#include "SpiUart.h"
SpiUartDevice SpiSerial;


void setup() {

  Serial.begin(9600);
  Serial.println("SPI UART on NextDev Board Terminal tool");
  Serial.println("--------------------------------------");  
  Serial.println();
  Serial.println("This is a tool to help you troubleshoot problems with the NextDev board.");
  Serial.println("For consistent results unplug & replug power to your board.");
  Serial.println("(Ensure the serial monitor is not open when you remove power.)");  
  Serial.println();
  
  Serial.println("Attempting to connect to SPI UART...");
  SpiSerial.begin();
  Serial.println("Connected to SPI UART.");
  Serial.println();
  
  Serial.println("Waiting for input.");
  Serial.println();    
  
}


void loop() {
  // Terminal routine

  // Always display a response uninterrupted by typing
  // but note that this makes the terminal unresponsive
  // while a response is being received.
  while(SpiSerial.available() > 0) {
    Serial.write(SpiSerial.read());
  }
  
  if(Serial.available()) { // Outgoing data
    SpiSerial.writeSerial.read());
  }

}
