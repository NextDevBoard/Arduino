#include "NextDev.h"

NextDev disp;

void setup() {
  Serial.begin(9600);
  disp.begin();
  Serial.print("Current page = ");
  Serial.println(disp.getPage());
  Serial.print("Screen brightness = ");
  Serial.println(disp.getIntVariable("dim"));
  Serial.print("ADC reading = ");
  int a = 0;
  for (int i = 0; i < 100; i++) {
    delay(1);
    a += analogRead(A0)  / 10;
  }
      
  Serial.println(a / 100);
}

void loop() {

    if (disp.available()) {
       Event e = disp.getEvent();
       String v = "b";
       v.concat(e.getComponent() - 1);
       v.concat(".txt");
  
       Serial.println("Value for " + v + " = " + disp.getStringVariable(v));
    }
   

/*
    disp.sendCommand("cls RED");
    
    for (int r = 0; r < 100; r =r + 2) {
        // cir 250,150,30,RED 
        String cmd = String("cir 250,150," + (String) r + ',' + (String) random(0xFFFF)); 
        disp.sendCommand(cmd);
        delay(10);
    }
    delay(100);
*/

}
