#include <LiquidCrystal.h>
#include <Ethernet.h>
#include <Keypad.h>
#include <PubSubClient.h>

#define MAC_6    0x73
#define WIN_SPEED A7          
#define WIN_DIRECTION A6         
#define WIND_SPEED_FACTOR 0.1f 
#define SAMPLE_PERIOD 5000     
#define outTopic   "ICT4_out_2020" 

byte server[] = { 10, 6, 0, 23 }; 
unsigned int Port = 1883;  
EthernetClient ethClient; 
PubSubClient client(server, Port, ethClient); 

static uint8_t mymac[6] = { 0x44,0x76,0x58,0x10,0x00,0x62 };

char* clientId = "a731fsd4"; 

IPAddress localIP;


const int rs = 6, en = 7, d4 = 5, d5 = 4, d6 = 3, d7 = 8;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const byte ROWS = 4;
const byte COLS = 4;
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {9, A4, 2, A5};
byte colPins[COLS] = {A0, A1, A2, A3};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

unsigned long prevStartTime = 0;
unsigned int sampleCount = 0;
float directionSum = 0.0;
int displayMode = 0;
int dataOption = 1;

char* IOTJS = "";

char* format_sensor_data(float windSpeed, float windDir, int dataOption) {
    static char result[200];
    char speedStr[20];
    char dirStr[20];

    dtostrf(windSpeed, 6, 2, speedStr);
    dtostrf(windDir, 6, 2, dirStr);

    if (dataOption == 0) {
        snprintf(result, sizeof(result),
                 "IOTJS={\"S_name1\":\"NTC_windSpeed\",\"S_value1\":%s}\n",
                 speedStr);
    } else if (dataOption == 1) {
        snprintf(result, sizeof(result),
                 "IOTJS={\"S_name2\":\"NTC_windDir\",\"S_value2\":%s}",
                 dirStr);
    } else {
        snprintf(result, sizeof(result),
                 "IOTJS={\"S_name1\":\"NTC_windSpeed\",\"S_value1\":%s,"
                 "\"S_name2\":\"NTC_windDir\",\"S_value2\":%s}",
                 speedStr, dirStr);
    }

    return result;
}

void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);
  pinMode(WIN_SPEED, INPUT);
  pinMode(WIN_DIRECTION, INPUT);
  pinMode(10, OUTPUT);
   
  prevStartTime = millis();
  fetchIP();
}

void loop() {
  char key = customKeypad.getKey();
  if (key) {
    Serial.print("Key: ");
    Serial.println(key);
    if (key == 'A') {
      displayMode = (displayMode + 1) % 4;
      lcd.clear();
    }
  }
  handleData();
}

void send_MQTT_message(char* message) {
  if (!client.connected()) {
      connect_MQTT_server();
  }
  if (client.connected()) { 
      client.publish(outTopic, message); 
      Serial.print("Message sent to MQTT server."); 
      Serial.print(message);
  } else {
      Serial.println("Failed to send message: not connected to MQTT server."); 
  }
}

void connect_MQTT_server() { 
  Serial.println("Connecting to MQTT"); 
  if (client.connect(clientId)) { 
      Serial.println("Connected OK"); 
  } else {
      Serial.println("Connection failed."); 
  }    
}

float calculateAvgWSpeed() {
  unsigned long startTime = millis();
  unsigned long endTime = startTime + 1000;
  unsigned int cycles = 0;
  float threshold = 2.5;
  bool wasAbove = analogRead(WIN_SPEED) * (5.0 / 1023.0) > threshold;
  
  while (millis() < endTime) {
    float voltage = analogRead(WIN_SPEED) * (5.0 / 1023.0);
    bool isAbove = voltage > threshold;
    if (wasAbove && !isAbove) {
      cycles++;
    }
    wasAbove = isAbove;
    delayMicroseconds(100);
  }
  
  float frequency = cycles;
  Serial.print("Cycles: ");
  Serial.print(cycles);
  Serial.print(" Freq: ");
  Serial.println(frequency);
  
  return frequency * WIND_SPEED_FACTOR;
}

void handleData() {
  unsigned long currentTime = millis();
  
  if (currentTime - prevStartTime >= SAMPLE_PERIOD) {
    float avgWSpeed = calculateAvgWSpeed();
    float averageDegrees = sampleCount > 0 ? directionSum / sampleCount : 0.0;
    String averageDirection = getDirection(averageDegrees);

    lcd.clear();
    if (displayMode == 0) {
      dataOption = 0;
      lcd.setCursor(0, 0);
      lcd.print("Wind Speed:");
      lcd.setCursor(0, 1);
      lcd.print(avgWSpeed, 1);
      lcd.print(" m/s");
      
      Serial.print("Wind Speed: ");
      Serial.print(avgWSpeed);
      Serial.println(" m/s");
    } 
    else if(displayMode == 1){
      dataOption = 1;
      lcd.setCursor(0, 0);
      lcd.print("Wind Direction:");
      lcd.setCursor(0, 1);
      lcd.print(averageDirection);
      lcd.print(" (");
      lcd.print(averageDegrees, 1);
      lcd.print(")");
      
      Serial.print("Direction: ");
      Serial.print(averageDirection);
      Serial.print(" (");
      Serial.print(averageDegrees);
      Serial.println(" deg)");
    }else if(displayMode == 2){
      dataOption = 2;
      lcd.setCursor(0, 0);
      lcd.print("Both");
    } else if(displayMode == 3){
      lcd.setCursor(0, 0);
      lcd.print("Connected with IP: ");
      lcd.setCursor(0, 1); 
      lcd.print(localIP);
    }

    char* message = format_sensor_data(avgWSpeed, averageDegrees, displayMode);
    send_MQTT_message(message);

    prevStartTime = currentTime;
    directionSum = 0.0;
    sampleCount = 0;
  }
  
  directionSum += readDirection();
  sampleCount++;
  
  delay(50);
}

float readDirection() {
  float voltage = analogRead(WIN_DIRECTION) * (5.0 / 1023.0);
  return (voltage / 5.0) * 360.0;
}

String getDirection(float degrees) {
  if ((degrees >= 337.5) || (degrees < 22.5)) return "N";
  else if (degrees < 67.5) return "NE";
  else if (degrees < 112.5) return "E";
  else if (degrees < 157.5) return "SE";
  else if (degrees < 202.5) return "S";
  else if (degrees < 247.5) return "SW";
  else if (degrees < 292.5) return "W";
  else return "NW";
}

void fetchIP() {
  byte connection = 1;
  connection = Ethernet.begin(mymac);

  Serial.print(connection);
  
  Serial.print(F("\nW5100 Revision "));
  if (connection == 0) {
    Serial.println(F("Failed to access Ethernet controller"));
  }
  
  Serial.println(F("Setting up DHCP"));
  Serial.print("Connected with IP: ");
  localIP = Ethernet.localIP();
  Serial.println(localIP);
  delay(1500);
}


