//Intitial Testcode used to make sure the WiFi module responds
//and that Serial and software serial works as intended. 


  
  #include <SoftwareSerial.h>

  const int softRX = 10; //Digital pin 10 
  const int softTX = 11; //Digital pin 11
  SoftwareSerial softSerial(softRX, softTX); // RX, TX
  

void setup()
{
   softSerial.begin(9600); // Open soft serial connection to report values to host
   softSerial.println("Starting up");
   
   Serial.begin(9600); //Open serial to communicate with ESP8266
   Serial.setTimeout(5000);
}
void loop()
{
Serial.println("AT+RST"); // reset and test if module is ready. 
  if(Serial.find("ready")) {
    softSerial.println("WiFi - Module is ready");
  }else{
    softSerial.println("Module doesn't respond.");
  }
  delay(10000);
}
