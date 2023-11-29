/*  Program: Serial_Power_Supply_Mock
  Simulates a Power Solutions TF800 or Helios HPSAE programable power supply
  Returns a result from "*IDN?"
  The set "SET_VOLTAGE:" comand will set the DAC output on pin #define DAC1 25

*/


#include <HardwareSerial.h>

#define BAUD_RATE 9600

// UART0
#define RX_PIN RX
#define TX_PIN TX
HardwareSerial SerialTF800(0);

// Define UART1 pins
//#define RX_PIN 16
//#define TX_PIN 17
//HardwareSerial SerialTF800(1);

//Defin some hardware
// dacWrite(DAC1, DAC1_Value);
#define DAC1 25
int DAC1_Value = 0; //Initial value of DAC1

//response = "TF800,Manufacturer,Model,Version,SerialNumber";

class Flasher
{
    // Class Member Variables
    // These are initialized at startup
    int ledPin;      // the number of the LED pin
    long OnTime;     // milliseconds of on-time
    long OffTime;    // milliseconds of off-time

    // These maintain the current state
    int ledState;                 // ledState used to set the LED
    unsigned long previousMillis;   // will store last time LED was updated

    // Constructor - creates a Flasher
    // and initializes the member variables and state
  public:
    Flasher(int pin, long on, long off)
    {
      ledPin = pin;
      pinMode(ledPin, OUTPUT);

      OnTime = on;
      OffTime = off;

      ledState = LOW;
      previousMillis = 0;
    }

    void Update()
    {
      // check to see if it's time to change the state of the LED
      unsigned long currentMillis = millis();

      if ((ledState == HIGH) && (currentMillis - previousMillis >= OnTime))
      {
        ledState = LOW;  // Turn it off
        previousMillis = currentMillis;  // Remember the time
        digitalWrite(ledPin, ledState);  // Update the actual LED
      }
      else if ((ledState == LOW) && (currentMillis - previousMillis >= OffTime))
      {
        ledState = HIGH;  // turn it on
        previousMillis = currentMillis;   // Remember the time
        digitalWrite(ledPin, ledState);   // Update the actual LED
      }
    }
};

// Flasher led1(12, 100, 400);  //Pins for UNO
// Flasher led2(13, 350, 350);

Flasher led1(2, 100, 400);      //Pins for ESP32
Flasher led2(3, 350, 350);

void setup() {
  Serial.begin(BAUD_RATE);
  SerialTF800.begin(BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN);
  Serial.println("End setup.");
}

void loop() {
  // Check for incoming commands from TF800
  if (SerialTF800.available()) {
    String command = SerialTF800.readStringUntil('\r\n');

    Serial.print("Received command: ");
    Serial.println(command);

    handleCommand(command);
  }

  led1.Update();
  led2.Update();
}

void handleCommand(String command) {
  // Process the received command and generate a response
  String response;

  // Serial.print("Handling Command: ");
  // Serial.println(command);

  // Convert the command to uppercase
  String upperCommand;
  for (int i = 0; i < command.length(); i++) {
    upperCommand += char(toupper(command[i]));
  }

  // Trim the command before comparison
  String trimmedUpperCommand = upperCommand;
  trimmedUpperCommand.trim();

  // Example: If the command is "*IDN?", respond with instrument identification
  if (trimmedUpperCommand == "*IDN?") {
    response = "TF800,Manufacturer,Model,Version,SerialNumber";
  }

  // Example: If the command starts with "SET_VOLTAGE:", respond with an acknowledgment
  else if (trimmedUpperCommand.startsWith("SET_VOLTAGE:")) {
    float voltageValue = trimmedUpperCommand.substring(12).toFloat();
    // Process the voltage setting (replace with actual logic)
    response = "ACK_SET_VOLTAGE";
  }

  // Example: If the command is "GET_CURRENT", respond with a current value
  else if (trimmedUpperCommand == "GET_CURRENT") {
    float currentValue = getCurrentValue(); // Replace with actual function
    response = "CURRENT:" + String(currentValue, 3) + "A";
  } else {
    response = "UNKNOWN_COMMAND";
  }

  // Send the response back to TF800
  SerialTF800.println(response);
}

float getCurrentValue() {
  // Replace this function with your actual code to retrieve current value
  // For the sake of example, returning a random current value
  return random(0, 10) / 1000.0; // Random value between 0 and 10 mA
}
