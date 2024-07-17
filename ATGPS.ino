#include <HardwareSerial.h>

// Define the serial port used to communicate with the SIM7672S module
HardwareSerial simSerial(2);

// Define the DTR and RI pins used for hardware flow control
const int dtrPin = 18;
const int riPin = 19;

// ThingSpeak API details
const String apiKey = "0W3UQ3FC9HSSCYRL";
const String server = "api.thingspeak.com";

// APN details for your cellular network provider
const String apn = "YOUR_APN";

void setup() {
  // Start the serial ports at the appropriate baud rates
  Serial.begin(115200);
  simSerial.begin(115200, SERIAL_8N1, 16, 17);

  // Set the DTR and RI pins as outputs and initialize them to HIGH
  pinMode(dtrPin, OUTPUT);
  digitalWrite(dtrPin, HIGH);
  pinMode(riPin, OUTPUT);
  digitalWrite(riPin, HIGH);

  // Wait for the serial ports to initialize
  delay(1000);
  
  // Print a message to the Arduino IDE Serial Monitor to indicate that the program is ready
  Serial.println("AT command program ready");

  // Initialize SIM7672S module and connect to the network
  initializeSIM7672S();
}

void loop() {
  // Send AT command to get GPS info and print the response
  simSerial.println("AT+CGPSINFO");
  delay(2000);
  String gpsData = readResponse();
  Serial.println("GPS Data: " + gpsData);

  // Extract latitude and longitude from GPS data
  float latitude = 0.0, longitude = 0.0;
  if (parseGPSData(gpsData, latitude, longitude)) {
    Serial.print("Latitude: ");
    Serial.println(latitude, 6);
    Serial.print("Longitude: ");
    Serial.println(longitude, 6);

    // Publish to ThingSpeak
    publishToThingSpeak(latitude, longitude);
  }

  // Wait for some time before sending the next command
  delay(10000);
}

String readResponse() {
  String response = "";
  // Set the DTR pin LOW to signal the SIM7672S module to start sending data
  digitalWrite(dtrPin, LOW);

  // Read any available data from the serial port
  while (simSerial.available()) {
    char c = simSerial.read();
    response += c;
  }

  // Set the RI pin HIGH to signal the SIM7672S module that the data has been received
  digitalWrite(riPin, HIGH);
  
  return response;
}

bool parseGPSData(String gpsData, float &latitude, float &longitude) {
  // Sample response: +CGPSINFO: 3739.8496,N,12225.3014,W,092946.0,170214,07
  int index = gpsData.indexOf("+CGPSINFO: ");
  if (index == -1) {
    return false;
  }
  
  // Extract the relevant part of the response
  gpsData = gpsData.substring(index + 11);
  gpsData.trim();
  if (gpsData.length() == 0 || gpsData == "0.0") {
    return false;
  }

  // Parse latitude and longitude
  int commaIndex = gpsData.indexOf(',');
  if (commaIndex == -1) return false;
  String latStr = gpsData.substring(0, commaIndex);

  gpsData = gpsData.substring(commaIndex + 1);
  commaIndex = gpsData.indexOf(',');
  if (commaIndex == -1) return false;
  String latDir = gpsData.substring(0, commaIndex);

  gpsData = gpsData.substring(commaIndex + 1);
  commaIndex = gpsData.indexOf(',');
  if (commaIndex == -1) return false;
  String lonStr = gpsData.substring(0, commaIndex);

  gpsData = gpsData.substring(commaIndex + 1);
  commaIndex = gpsData.indexOf(',');
  if (commaIndex == -1) return false;
  String lonDir = gpsData.substring(0, commaIndex);

  // Convert latitude and longitude to float
  latitude = convertToDecimalDegrees(latStr, latDir);
  longitude = convertToDecimalDegrees(lonStr, lonDir);
  
  return true;
}

float convertToDecimalDegrees(String coord, String dir) {
  // Convert DDDMM.MMMM to decimal degrees
  float decimal = coord.substring(0, 2).toFloat() + (coord.substring(2).toFloat() / 60.0);
  if (dir == "S" || dir == "W") {
    decimal *= -1;
  }
  return decimal;
}

void initializeSIM7672S() {
  // Ensure module is in command mode
  sendATCommand("AT");
  sendATCommand("ATE0"); // Echo off
  sendATCommand("AT+CGNSPWR=1"); // Turn on GPS
  sendATCommand("AT+CGATT=1"); // Attach to GPRS
  sendATCommand("AT+CGDCONT=1,\"IP\",\"" + apn + "\""); // Set APN
  sendATCommand("AT+CGACT=1,1"); // Activate PDP context

  // Enable Hot Start
  sendATCommand("AT+CGPSHOT"); // Hot start

  // Configure and download A-GPS data
  configureAGPS();
}

void sendATCommand(String command) {
  simSerial.println(command);
  delay(1000);
  String response = readResponse();
  Serial.println(response);
}

void configureAGPS() {
  // Configure the A-GPS server and port
  sendATCommand("AT+CGPSURL=\"supl.google.com\"");
  sendATCommand("AT+CGPSADDR=\"216.239.35.3\",\"7275\""); // IP of supl.google.com

  // Download A-GPS data
  sendATCommand("AT+CGPSAGPS=1"); // Enable A-GPS
  sendATCommand("AT+CGPSAGNSS=1"); // Start A-GPS data download
}

void publishToThingSpeak(float latitude, float longitude) {
  String postData = "api_key=" + apiKey + "&field1=" + String(latitude, 6) + "&field2=" + String(longitude, 6);

  // Start the HTTP POST request
  simSerial.println("AT+HTTPINIT");
  delay(1000);
  Serial.println(readResponse());
  
  simSerial.println("AT+HTTPPARA=\"CID\",1");
  delay(1000);
  Serial.println(readResponse());
  
  simSerial.println("AT+HTTPPARA=\"URL\",\"http://" + server + "/update\"");
  delay(1000);
  Serial.println(readResponse());
  
  simSerial.println("AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded\"");
  delay(1000);
  Serial.println(readResponse());
  
  simSerial.println("AT+HTTPDATA=" + String(postData.length()) + ",10000");
  delay(1000);
  Serial.println(readResponse());
  
  simSerial.print(postData);
  delay(1000);
  Serial.println(readResponse());
  
  simSerial.println("AT+HTTPACTION=1");
  delay(1000);
  Serial.println(readResponse());
  
  simSerial.println("AT+HTTPREAD");
  delay(1000);
  String response = readResponse();
  Serial.println("HTTP response: " + response);
  
  simSerial.println("AT+HTTPTERM");
  delay(1000);
  Serial.println(readResponse());
}

