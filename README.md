# Serial_Power_Supply_Mock
Simulates a Power Solutions TF800 or Helios HPSAE programable power supply

## Hardware
Runs on ESP32 S2
The interface is a serial port UART different from the ESP32 prgraming UART.  The programing UART allows for monitoring the mock power supply and changing its opperation including asserting faults.


## The motivation 
To have an inexpensive ESP32 mock an expensive (about $500) power supply for development of firmware in another product where the actual power supply is not needed by every developer but the system under development and test would throw errors if the power suppy is not responding to serial commands.
To have a mock supply where a tester can simulate faults such as the fan failing and capture results of the system under test to a power supply fan failure.

## Target Commands
_Lifted from the HPSAE document_

 COMMAND DESCRIPTION from Communication Protocol in User's Manual(s)
  --------------------------------------------------  
| Command | Description                        | Notes | Implimented |
|---------|------------------------------------|-------|-------------|
| ADDS    | Device Addressing                  |       |             |
| GLOB    | Global Power ON / OFF Control      |       |             |
| POWER   | Power ON / OFF / Query             |       |             |
| GSV     | Global control O/P voltage setting |       |             |
| GSI     | Global control O/P current setting |       |             |
| GRPWR   | GRPWR 1 Global Power ON            |       |             |
| GRPWR   | GRPWR 0 Global Power OFF           |       |             |
| SV nn.n | O/P Voltage Setting in Volts       |       | - [ ]       |
| SI nn.n | O/P Current Setting in Amps        |       | - [ ]       |
| SV?     | Voltage setting Query              |       | - [ ]       |
| SI?     | Current setting Query              |       | - [ ]       |
| RV?     | O/P Voltage Query                  |       | - [ ]       |
| RI?     | O/P Current Query                  |       | - [ ]       |
| RT?     | Temperature Query                  |       | - [ ]       |
| REMS    | Remote ON / OFF / Query            |       |             |
| STUS    | Device Status Query                |       |             |
| INFO    | Information Query                  |       |             |
| RATE?   | Rate V/I Query                     |       |             |
| DEVI?   | Device Name Query                  |       |             |
| *IDN?   | Identification Query               |       | - [ ]       |
|         |                                    |       |             |


## Wokwi Simulation
https://wokwi.com/projects/382758334051087361
![image](https://github.com/ForrestErickson/Serial_Power_Supply_Mock/assets/5836181/b2c73d48-b1da-4c08-a774-0d38d61859d2)

## License
In general we intend to follow the [Public Invention Licensed Guidelines](https://github.com/PubInv/PubInv-License-Guidelines)  
If you are starting your own project, please feel free to fork the license guidelines as a starting point if you like our license policy.

This program includes free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

See the GNU Affero General Public License for more details. You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
