/*  Program: Serial_Power_Supply_Mock
  https://wokwi.com/projects/382758334051087361

  Simulates a Power Solutions TF800 or Helios HPSAE programable power supply
  Returns a result from "*IDN?"
  The set "SET_VOLTAGE:" comand will set the DAC output on pin #define DAC1 25

*/

/*
   COMMAND DESCRIPTION from Communication protocol User's Manual
  --------------------------------------------------
  ADDS <adds> Device Addressing
  GLOB <type> Global Power ON / OFF Control
  POWER <type> Power ON / OFF / Query
  GSV <value> Global control O/P voltage setting
  GSI <value> Global control O/P current setting
  GRPWR 1 Global Power ON
  GRPWR 0 Global Power OFF
  SV <value> O/P Voltage Setting
  SI <value> O/P Current Setting
  SV? Voltage setting Query
  SI? Current setting Query
  RV? O/P Voltage Query
  RI? O/P Current Query
  RT? Temperature Query
  REMS <type> Remote ON / OFF / Query
  STUS <type> Device Status Query
  INFO <type> Information Query
  RATE? Rate V/I Query
  DEVI? Device Name Query
  IDN? Identification Query


  The strings of reply and the represented results are shown as below:
  = > CR LF -> Command executed successfully.
  ? > CR LF -> Command error, not accepted.
  ! > CR LF -> Command correct but execution error (e.g. parameters out of range)

  While addressing effective devices execute the command with query function, SMPS
  will transmit the string of query result first, then use “CR LF” for termina

  Test commands
   IDN?
  SV 6.5
  RV?
  SI 21.5
  SI?
  RI?
  RT?


*/

#include <HardwareSerial.h>
#include "driver/temp_sensor.h"

#define BAUD_RATE 9600

// UART0
#define RX_PIN RX
#define TX_PIN TX
HardwareSerial SerialTF800(0);

// Define UART1 pins
//#define RX_PIN 16
//#define TX_PIN 17
//HardwareSerial SerialTF800(1);

// Globals for the state
float g_voltageSetting = 0.0;
float g_currentLimitSetting = 0.0;

//Define some hardware
// dacWrite(DAC1, DAC1_Value);
#define DAC1 25
int DAC1_Value = 0; //Initial value of DAC1

//response = "TF800,Manufacturer,Model,Version,SerialNumber";

void initTempSensor() {
  temp_sensor_config_t temp_sensor = TSENS_CONFIG_DEFAULT();
  temp_sensor.dac_offset = TSENS_DAC_L2;  // TSENS_DAC_L2 is default; L4(-40°C ~ 20°C), L2(-10°C ~ 80°C), L1(20°C ~ 100°C), L0(50°C ~ 125°C)
  temp_sensor_set_config(temp_sensor);
  temp_sensor_start();
}


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
  initTempSensor();
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

// Process the received command and generate a response
void handleCommand(String command) {
  // Convert the command to uppercase and trim
  String trimmedUpperCommand = command;
  trimmedUpperCommand.trim();

  // Declare the response variable
  String response;

  // Example: If the command is "*IDN?", respond with instrument identification
  if (trimmedUpperCommand == "*IDN?") {
    response = "TF800,Manufacturer,Model,Version,SerialNumber";
  }

  // Set Voltage, respond with an acknowledgment
  else if (trimmedUpperCommand.startsWith("SV ")) {
    float voltageValue = trimmedUpperCommand.substring(3).toFloat();
    // Process the voltage setting (replace with actual logic)
    g_voltageSetting = voltageValue;
    Serial.println(voltageValue);
    response = "ACK_SET_VOLTAGE";
  }

  // Read Voltage
  else if (trimmedUpperCommand.startsWith("RV?")) {
    float currentValue = getCurrentValue(); // Replace with actual function
    response = "VOLTAGE:" + String(currentValue, 3) + "V";
  }

  // Set Current, respond with an acknowledgment
  else if (trimmedUpperCommand.startsWith("SI ")) {
    float currentValue = trimmedUpperCommand.substring(3).toFloat();
    // Process the current setting (replace with actual logic)
    g_currentLimitSetting = currentValue;
    Serial.println(currentValue);
    response = "ACK_SET_CURRENT";
  }

  // Get Current set, respond with an acknowledgment
  else if (trimmedUpperCommand.startsWith("SI?")) {
    // Process the current setting (replace with actual logic)
    float currentValue = g_currentLimitSetting;
    Serial.println(currentValue);
    response = "ACK_GET_CURRENT_SET";
  }

  // Read Current", respond with a current value
  else if (trimmedUpperCommand == "RI?") {
    float currentValue = getCurrentValue(); // Replace with actual function
    response = "CURRENT:" + String(currentValue, 3) + "A";
  }

  // Read Temperature", respond with a temperature value
  else if (trimmedUpperCommand == "RT?") {
    float result = 0;
    temp_sensor_read_celsius(&result);
    response = "TEMPERATURE:" + String(result, 3) + "C";
  }

  // Unknown command
  else {
    response = "UNKNOWN_COMMAND";
  }

  // Send the response back to TF800
  SerialTF800.println(response);
}
// end handleCommand()

float getCurrentValue() {
  // Replace this function with your actual code to retrieve current value
  // For the sake of example, returning a random current value
  return random(0, 10) / 1000.0; // Random value between 0 and 10 mA
}
