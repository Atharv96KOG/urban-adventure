// Viral Science www.viralsciencecreativity.com www.youtube.com/c/viralscience
// Blynk IOT Smart Plant Monitoring System

/* Connections
Relay. D3
Btn.   D7
Soil.  A0
PIR.   D5
SDA.   D2
SCL.   D1
Temp.  D4
*/

//Include the library files
#include <LiquidCrystal_I2C.h>
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

#define BLYNK_TEMPLATE_ID "TMPL3VSNezdrB"
#define BLYNK_TEMPLATE_NAME "Smart Plant"
#define BLYNK_AUTH_TOKEN "zxv7awFHHbs2lP_52X1cTi8YL5Qc4Vvp"

//Initialize the LCD display
LiquidCrystal_I2C lcd(0x3F, 16, 2);


char auth[] = "zxv7awFHHbs2lP_52X1cTi8YL5Qc4Vvp     ";  //Enter your Blynk Auth token
char ssid[] = "Atharv";  //Enter your WIFI SSID
char pass[] = "5484844";  //Enter your WIFI Password

DHT dht(D4, DHT11);//(DHT sensor pin,sensor type)  D4 DHT11 Temperature Sensor
BlynkTimer timer;

//Define component pins
#define soil A0     //A0 Soil Moisture Sensor
#define PIR D5      //D5 PIR Motion Sensor#define trig D5   // Trig pin
#define TRIGGER_PIN D6
#define ECHO_PIN D7
#define RELAY_PIN_1 D3
int PIR_ToggleValue;
int depth =20;


void checkPhysicalButton();
int relay1State = LOW;
int pushButton1State = HIGH;
#define RELAY_PIN_1       D3   //D3 Relay
#define PUSH_BUTTON_1     D7   //D7 Button
#define VPIN_BUTTON_1    V12 

//Create three variables for pressure
double T, P;
char status;



void setup() {
  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();
  pinMode(PIR, INPUT);


 pinMode(RELAY_PIN_1, OUTPUT);
 digitalWrite(RELAY_PIN_1, LOW);
  pinMode(PUSH_BUTTON_1, INPUT_PULLUP);
  digitalWrite(RELAY_PIN_1, relay1State);


  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  dht.begin();
  //

//  Serial.begin(9600);
//   Blynk.begin(auth, ssid, pass);
//   pinMode(TRIGGER_PIN, OUTPUT);
//   pinMode(ECHO_PIN, INPUT);
//   pinMode(PIR, INPUT);
//   pinMode(RELAY_PIN_1, OUTPUT);
//   pinMode(PUSH_BUTTON_1, INPUT_PULLUP);


  lcd.setCursor(0, 0);
  lcd.print("  Initializing  ");
  for (int a = 5; a <= 10; a++) {
    lcd.setCursor(a, 1);
    lcd.print(".");
    delay(500);
  }
  lcd.clear();
  lcd.setCursor(11, 1);
  lcd.print("W:OFF");
  //Call the function
  timer.setInterval(100L, soilMoistureSensor);
  timer.setInterval(100L, DHT11sensor);
  timer.setInterval(500L, checkPhysicalButton);
  //  timer.setInterval(1000L, sendUltrasonicData);
}


//Get the DHT11 sensor values
void DHT11sensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);

  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(t);

  lcd.setCursor(8, 0);
  lcd.print("H:");
  lcd.print(h);

}


//Get the soil moisture values
void soilMoistureSensor() {
  int value = analogRead(soil);
  // Serial.print("Raw sensor value: ");
  // Serial.println(value);
  // value = map(value, 0, 1024, 0, 100);
  // Serial.print("Mapped sensor value: ");
  // Serial.println(value);
  // value = (value-100) * -1;
  // Serial.print("Adjusted sensor value: ");
  // Serial.println(value);
   value = map(value,400,1023,100,0);
   Blynk.virtualWrite(V2, value);
    Serial.print("Soil Moisture : ");
    Serial.print(value);

}


/////////////////////
// long duration, distance;
// void readUltrasonicSensor() {
//   digitalWrite(TRIGGER_PIN, LOW);
//   delayMicroseconds(2);
//   digitalWrite(TRIGGER_PIN, HIGH);
//   delayMicroseconds(10);
//   digitalWrite(TRIGGER_PIN, LOW);
//   duration = pulseIn(ECHO_PIN, HIGH);
//   distance = duration * 0.034 / 2;
// }

// void printSensorValues() {
//   readUltrasonicSensor(); // Read distance from ultrasonic sensor
//   Serial.print("Distance: ");
//   Serial.println(distance);

//   // Map the distance value
//   int mappedValue = map(distance, 0, 1024, 0, 100);
//   Serial.print("Mapped sensor value: ");
//   Serial.println(mappedValue);

//   // Adjust the mapped value
//   int adjustedValue = (mappedValue - 100) * -1;
//   Serial.print("Adjusted sensor value: ");
//   Serial.println(adjustedValue);
// }



///////////////////////////
//Get the PIR sensor values
void PIRsensor() {
  bool value = digitalRead(PIR);
  if (value) {
    Blynk.logEvent("pirmotion","WARNNG! Motion Detected!"); //Enter your Event Name
    WidgetLED LED(V5);
    LED.on();
  } else {
    WidgetLED LED(V5);
    LED.off();
  }  
  }

BLYNK_WRITE(V6)
{
 PIR_ToggleValue = param.asInt();  
}


BLYNK_CONNECTED() {
  // Request the latest state from the server
  Blynk.syncVirtual(VPIN_BUTTON_1);
}

BLYNK_WRITE(VPIN_BUTTON_1) {
  relay1State = param.asInt();
  digitalWrite(RELAY_PIN_1, relay1State);
}

void checkPhysicalButton()
{
  if (digitalRead(PUSH_BUTTON_1) == LOW) {
    // pushButton1State is used to avoid sequential toggles
    if (pushButton1State != LOW) {

      // Toggle Relay state
      relay1State = !relay1State;
      digitalWrite(RELAY_PIN_1, relay1State);

      // Update Button Widget
      Blynk.virtualWrite(VPIN_BUTTON_1, relay1State);
    }
    pushButton1State = LOW;
  } else {
    pushButton1State = HIGH;
  }
}
///


void loop() {
    if (PIR_ToggleValue == 1)
    {
    lcd.setCursor(5, 1);
    lcd.print("M:ON ");
      PIRsensor();
      }
     else
     {
    lcd.setCursor(5, 1);
    lcd.print("M:OFF");
    WidgetLED LED(V5);
    LED.off();
     }

if (relay1State == HIGH)
{
  lcd.setCursor(11, 1);
  lcd.print("W:ON ");
  }
  else if (relay1State == LOW)
  {
    lcd.setCursor(11, 1);
    lcd.print("W:OFF");
    }



  // printSensorValues();
  // delay(1000);


  Blynk.run();//Run the Blynk library
  timer.run();//Run the Blynk timer

  }
