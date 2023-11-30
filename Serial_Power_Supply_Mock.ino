/*  Program: Serial_Power_Supply_Mock
 *   By: Forrest Lee Erickson
 *   Date: 20231129
 *   License
  In general we intend to follow the [Public Invention Licensed Guidelines](https://github.com/PubInv/PubInv-License-Guidelines)
  If you are starting your own project, please feel free to fork the license guidelines as a starting point if you like our license policy.
  This program includes free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
  See the GNU Affero General Public License for more details. You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 * Notes on simulation

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

#define BAUD_RATE 9600 // For Wokwi.  For deployment use the rate of a real power supply

// UART0  For communication and control of the MOCK supply
#define RX_PIN RX
#define TX_PIN TX
HardwareSerial SerialTF800(0);

// Define UART1 pins  for the eventual MOCK interface to a system which thinks this is a power supply
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

//Use some flashing LEDs so we can see the main loop is running.
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

Flasher led1(2, 100, 400);      //Pins for ESP32
Flasher led2(3, 350, 350);

void setup() {
  initTempSensor();                       //Just to have something to return as the power supply temprature.
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


float getCurrentValue() {
  // Replace this function with your actual code to retrieve current value
  // For the sake of example, returning a random current value
  return random(0, 10) / 1000.0; // Random value between 0 and 10 mA
}
