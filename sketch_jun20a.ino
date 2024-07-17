#include <HardwareSerial.h>

// Define the serial port used to communicate with the SIM7672S module
HardwareSerial simSerial(1);

// Define the DTR and RI pins used for hardware flow control
const int dtrPin = 10;
const int riPin = 11;

void setup() {
  // Start the serial ports at the appropriate baud rates
  Serial.begin(115200);
  simSerial.begin(115200, SERIAL_8N1, 18, 17);

  // Set the DTR and RI pins as outputs and initialize them to HIGH
  pinMode(dtrPin, OUTPUT);
  digitalWrite(dtrPin, HIGH);
  pinMode(riPin, OUTPUT);
  digitalWrite(riPin, HIGH);

  // Wait for the serial ports to initialize
  delay(1000);
  
  // Print a message to the Arduino IDE Serial Monitor to indicate that the program is ready
  Serial.println("AT command program ready");
}

void loop() {
  // Send an AT command to the SIM7672S module and print the response
  simSerial.println("AT");
  delay(1000);
  printResponse();

  // Send a second AT command to the SIM7672S module and print the response
  simSerial.println("AT+CSQ");
  delay(1000);
  printResponse();
}

void printResponse() {
  // Set the DTR pin LOW to signal the SIM7672S module to start sending data
  digitalWrite(dtrPin, LOW);

  // Read any available data from the serial port and write it to the Arduino IDE Serial Monitor
  while (simSerial.available()) {
    Serial.write(simSerial.read());
  }

  // Set the RI pin HIGH to signal the SIM7672S module that the data has been received
  digitalWrite(riPin, HIGH);
}