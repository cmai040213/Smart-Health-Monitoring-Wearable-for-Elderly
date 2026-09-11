//gyro
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <WiFiS3.h>
#include <Arduino_JSON.h>

//GPS
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

//Heart rate
#include "MAX30105.h"
#include "heartRate.h"

//gyro
Adafruit_MPU6050 mpu;
bool freeFallDetected = false; //indicate not falling
unsigned long fallStartTime = 0; //to calculate time fall

//GPS
SoftwareSerial gpsSerial(8,9); //Tx - pin 8, RX - pin 9
TinyGPSPlus gps;
float lattitude,longitude; //gps coordinates

//Heart rate
MAX30105 particleSensor;
const byte RATE_SIZE = 4; //number of samples to average
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred
float beatsPerMinute;
int beatAvg;
bool isFoundWrist = true; //track if it can find wrist or not
String heartRateStatus = "";

//WIFI
// WIFI
char ssid[] = "";
char pass[] = "";


//set a default IP address
IPAddress localIP(172, 20, 10, 13);
IPAddress gateway(172, 20, 10, 1);
IPAddress subnet(255, 255, 255, 240);
IPAddress dns(8, 8, 8, 8);


WiFiServer server(80);


void setup() {
  
  Serial.begin(115200);
  // gyro
  mpu.begin();

  mpu.setAccelerometerRange(MPU6050_RANGE_4_G); //set range to +-4g
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);//set gyro range +-500 degree/s
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ); //reduce noise

  delay(100);

  //WIFI
  // Connect to WiFi
Serial.begin(115200);

  // Start WiFi
  Serial.print("Connecting to WiFi..."); WiFi.config(localIP, dns, gateway, subnet); WiFi.begin(ssid, pass); while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); } Serial.println("\nConnected!"); Serial.print("IP Address: "); Serial.println(WiFi.localIP()); server.begin(); // Start web server 

  //GPS
  gpsSerial.begin(9600);

  //Heart rate
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1); //stop when couldnt find it
  }
  Serial.println("Place your index wrist on the sensor with steady pressure.");

  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED

}

void loop() {
// 👇 Disable sensor printing for first 5 seconds
static bool printedIP = false;
if (!printedIP) {
  printedIP = true;
  Serial.println("\n\n==== SYSTEM READY ====");
  Serial.println("Your Arduino IP address:");
  Serial.println(WiFi.localIP());
  Serial.println("======================\n\n");
  delay(5000); // Wait 5 seconds so you can see the IP clearly
}


  // GYRO PART
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp); //read acceleration + gyro data
  //calculate total acceleration magnitude
  float totalAcc = sqrt(
      a.acceleration.x * a.acceleration.x +
      a.acceleration.y * a.acceleration.y +
      a.acceleration.z * a.acceleration.z) / 9.81; //divide vy 9.81 to get in g

  Serial.print("Total Acc (g): ");
  Serial.println(totalAcc, 2);

  //Detect free-fall
  // === FALL DETECTION ===

// Acceleration magnitude (g)
float totalAccG = totalAcc;

// Thresholds
float IMPACT_THRESHOLD = 2.5;   // strong impact
float RESET_TIME = 2000;        // ms

// Fall flag
static bool fallDetected = false;
static unsigned long lastFallTime = 0;

// Detect strong impact
if (totalAccG > IMPACT_THRESHOLD && !fallDetected) {
  fallDetected = true;
  lastFallTime = millis();
  Serial.println(" HARD IMPACT → POSSIBLE FALL!");
}

// Reset fall status after 2 seconds
if (fallDetected && (millis() - lastFallTime > RESET_TIME)) {
  fallDetected = false;
}

// Send this new fall state instead of freeFallDetected
bool fallStateToSend = fallDetected;

delay(30);

  //GPS PART
  while (gpsSerial.available())
  {
    int data = gpsSerial.read();
    if (gps.encode(data)) //decode imcoming GPS data
    {
      lattitude = (gps.location.lat());
      longitude = (gps.location.lng());
      Serial.print ("lattitude: ");
      Serial.println (lattitude);
      Serial.print ("longitude: ");
      Serial.println (longitude);
    }
  }

  //HEAR RATE PART
  long irValue = particleSensor.getIR(); //get infrared radiation

  if (checkForBeat(irValue) == true)
  {
    
    long time = millis() - lastBeat; 
    lastBeat = millis(); //set new time

    beatsPerMinute = 60 / (time / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  Serial.print("IR=");
  Serial.print(irValue);
  Serial.print("BPM=");
  Serial.print(beatsPerMinute);
  Serial.print(", Avg BPM=");
  Serial.print(beatAvg);


  if (irValue < 50000){
    Serial.print(" Cound not find wrist");
    isFoundWrist = false;
  }
  else{
    isFoundWrist = true;
  }
  Serial.println();
  

  if(isFoundWrist == true){
  if (beatsPerMinute < 80){
    Serial.println("low heart rate");
    heartRateStatus = "Low heart rate";

  }
  else if (beatsPerMinute >= 80 && beatsPerMinute <= 112){
    Serial.println("Moderate-intensity activities");
    heartRateStatus = "Moderate-intensity activities";
  }
  else if(beatsPerMinute > 112 && beatsPerMinute <= 128){
    Serial.println("Vigorous physical activity");
    heartRateStatus = "Vigorous physical activity";

  }
  else if (beatsPerMinute > 128){
    Serial.println("High heart rate");
    heartRateStatus = "High heart rate";
  }
  }
  WiFiClient client = server.available();

if (client) {
  String request = client.readStringUntil('\r');

  // Prepare JSON
  JSONVar data;
  data["acc"] = totalAcc;
  data["fall"] = fallStateToSend;
  data["lat"] = ;
  data["lng"] = ;
  data["bpm"] = beatsPerMinute;
  data["avgBpm"] = beatAvg;
  data["wrist"] = isFoundWrist;
  data["heartRateStatus"] = heartRateStatus;

  String jsonString = JSON.stringify(data);

  // Send response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println("Access-Control-Allow-Origin: *");
  client.println();
  client.print(jsonString);

  client.stop();
}

}
