/*  Program: Serial_Power_Supply_Mock
     By: Forrest Lee Erickson
     Date: 20231129
     License
  In general we intend to follow the [Public Invention Licensed Guidelines](https://github.com/PubInv/PubInv-License-Guidelines)
  If you are starting your own project, please feel free to fork the license guidelines as a starting point if you like our license policy.
  This program includes free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
  See the GNU Affero General Public License for more details. You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

   Notes on simulation

  https://wokwi.com/projects/382758334051087361
  Simulates a Power Solutions TF800 or Helios HPSAE programable power supply
  Returns a result from "*IDN?"
  The set "SET_VOLTAGE:" comand will set the DAC output on pin #define DAC1 25

  Compile as ESP Dev Module.  PC Port 6.
*/


enum infoType { Manufacture, Model,  Output_Voltage, Revision,  Date_of_MFG,  Serial_Number,  Country_of_MFG };


#define COMPANY_NAME "pubinv.org "
#define PROG_NAME "Serial_Power_Supply_Mock "
#define VERSION "V0.5 "
#define DEVICE_UNDER_TEST "ESP32 S2:_"  //A model number
#define LICENSE "GNU Affero General Public License, version 3 "
#define ORIGIN "USA"


#include <HardwareSerial.h>
//#include "driver/temp_sensor.h"
//#include <temp_sensor.h>

#define BAUD_RATE 115200 // For ESP32 Debug serial.  
#define BAUD_RATE_PS1 4800 // Required for Power Supply interface.
#define BAUD_RATE_PS2 4800


// UART0  For debug communication and Terminal control of the MOCK supply
#define RX_PIN RX
#define TX_PIN TX

// Define UART1 pins  for the eventual MOCK interface to a system which thinks this is a power supply
#define UART1_RX_PIN 16
#define UART1_TX_PIN 17
HardwareSerial SerialTF8001(1);

// Globals for the state of Power Supply

//The serial address setting.  TODO: Set address with a switch or an eprom setting.
#define ADDRESS_SET 1 // Set different from the phicial TF800 at0. 

float g_voltageSetting = 0.0;
float g_currentLimitSetting = 0.0;
bool match_address_flag = true; // Address flag is 1

String g_parsedAddressSetting = "";

//Define some hardware
// dacWrite(DAC1, DAC1_Value);
#define DAC1 25
int DAC1_Value = 0; //Initial value of DAC1

//response = "TF800,Manufacturer,Model,Version,SerialNumber";

#ifdef __cplusplus
extern "C" {
#endif
uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif
uint8_t temprature_sens_read();

void initTempSensor() {
  //  temp_sensor_config_t temp_sensor = TSENS_CONFIG_DEFAULT();
  //  temp_sensor.dac_offset = TSENS_DAC_L2;  // TSENS_DAC_L2 is default; L4(-40°C ~ 20°C), L2(-10°C ~ 80°C), L1(20°C ~ 100°C), L0(50°C ~ 125°C)
  //  temp_sensor_set_config(temp_sensor);
  //  temp_sensor_start();

  Serial.print((temprature_sens_read() - 32) / 1.8);
  Serial.println(" C");

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

uint32_t g_chip_Id = 9999999;

void setup() {
  //Splash
  Serial.begin(BAUD_RATE);
  while (!Serial);
  Serial.println("===================================");
  Serial.print(DEVICE_UNDER_TEST);
  Serial.print(PROG_NAME);
  Serial.println(VERSION);
  Serial.print("Compiled at: ");
  Serial.println(F(__DATE__ " " __TIME__) ); //compile date that is used for a unique identifier

  Serial.println(returnChipID());
  g_chip_Id = returnChipID();


  initTempSensor();           //Just to have something to return as the power supply temprature.

  SerialTF8001.begin(BAUD_RATE_PS1, SERIAL_8N1, UART1_RX_PIN, UART1_TX_PIN); //UART1_RX_PIN
  Serial.println("End setup.");
}

void loop() {
  // Check for incoming commands from TF800
  if (Serial.available()) {
    String command = Serial.readStringUntil('\r\n');
    handleCommand(command);
  }


  if (SerialTF8001.available()) {
    String UART1command = SerialTF8001.readStringUntil('\r\n');
    Serial.print("Got command on UART1: ");
    Serial.println(UART1command);
    handleCommand(UART1command);
  }


  led1.Update();
  led2.Update();
}


float getCurrentValue() {
  // Replace this function with your actual code to retrieve current value
  // For the sake of example, returning a random current value
  return random(0, 10) / 1000.0; // Random value between 0 and 10 mA
}
