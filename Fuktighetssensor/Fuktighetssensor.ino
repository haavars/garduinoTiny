/**
 * Garduino 
 * Fuktsensor og pumpe for vanning
 * 
 * SHT10 fuktsensor og temperaturmåler, https://www.adafruit.com/product/1298
 * Pumpe https://www.adafruit.com/products/1150
 * 
 */
#include <SoftwareSerial.h>
#include <SHT1x.h>

// Specify data and clock connections for SHT10. 
// init SHT10 
  const int dataPin = 7;
  const int clockPin = 6;
  SHT1x sht1x(dataPin, clockPin);
  
  unsigned long interval = 20000; // the time the code waits, this should be 10/15 minutes.. 
  unsigned long previousMillis = 0; //
  
  //pin to enable waterpump.
  const int motorPin = 2;
  
  const int softRX = 10; //Digital pin 4 
  const int softTX = 11; //Digital pin 5
  SoftwareSerial softSerial(softRX, softTX); // RX, TX
  
  //Thingspeak and wireless settings
  String ssid = "YourSSID";
  String password = "YourWIFIPassword";
  String ip = "184.106.153.149"; // thingspeak.com
  String get = "GET /update?key=[ThingspeakAPIKey]]&field1=";

void setup()
{
   softSerial.begin(9600); // Open soft serial connection to report values to host
   softSerial.println("Starting up");
   
   Serial.begin(9600); //Open serial to communicate with ESP8266
   Serial.setTimeout(5000);
   
   pinMode(motorPin, OUTPUT);
   
  Serial.println("AT+RST"); // reset and test if module is ready. 
  if(Serial.find("ready")) {
    softSerial.println("WiFi - Module is ready");
    //connect to the wifi
    boolean connected = false;
    for (int i = 0; i < 5; i++)
    {
      if (connectWiFi())
      {
        connected = true;
        break;
      }
    }
    if (!connected)
    {
      softSerial.println("Something wrong!!");
    }
  }else{
    softSerial.println("Module doesn't respond. Check cables and retry!");
  }
  delay(1000);
}

void loop()
{
    float temp_c;
    float humidity;

   if((unsigned long)(millis() - previousMillis) >= interval)
  {
    previousMillis = millis();
    
    //do some magic here to set motorPin to high if humidity is low. 
    // run the motor for x seconds to get x ml of water. 
    // Do some testing to check how much water is pumped pr sec. 
    // motorPin

    // Read values from the sensor
    temp_c = sht1x.readTemperatureC();
    humidity = sht1x.readHumidity();
  
    // Print the values to the serial port
    softSerial.print("Temperature: ");
    softSerial.print(temp_c, 2);
    softSerial.print("C ");
    softSerial.print("Humidity: ");
    softSerial.print(humidity);
    softSerial.println("%");
    sendData(40);
  }
}

boolean sendData(int humidity){
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += ip;
  cmd += "\",80";
  Serial.println(cmd);
  if(Serial.find("Error")){
    softSerial.print("RECEIVED: Error");
    return false;
  }
  cmd = get;
  cmd += humidity;
  cmd += "\r\n";
  Serial.print("AT+CIPSEND=");
  Serial.println(cmd.length());
  if(Serial.find(">")){
    softSerial.print(">");
    softSerial.print(cmd);
    Serial.print(cmd);
  }else{
  Serial.println("AT+CIPCLOSE");
  }
    if(Serial.find("OK")){
    softSerial.println("RECEIVED: OK");
    return true;
  }else{
    softSerial.println("RECEIVED: Error");
  }
}


boolean connectWiFi()
{
  Serial.println("AT+CWMODE=1");
  String cmd = "AT+CWJAP=\"";
  cmd += ssid;
  cmd += "\",\"";
  cmd += password;
  cmd += "\"";
  softSerial.println(cmd);
  Serial.println(cmd);
  delay(2000);
  if (Serial.find("OK"))
  {
    softSerial.println("OK, Connected to WiFi.");
    return true;
  }
  else
  {
    softSerial.println("Can not connect to the WiFi.");
    return false;
  }
}
