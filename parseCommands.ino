
/*
   COMMAND DESCRIPTION from TF800 Communication protocol User's Manual
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


// Process the received command and generate a response
void handleCommand(String command) {
  // Convert the command to uppercase and trim
  String trimmedUpperCommand = command;
  trimmedUpperCommand.trim();

  // Declare the response variable
  String response;

  // Example: If the command is "*IDN?", respond with instrument identification
  if (trimmedUpperCommand == "*IDN?") {
    response = String(COMPANY_NAME) + String(PROG_NAME) + String(VERSION) + String(g_chip_Id);
  }

  // Get INFO<type> , respond one of six types
  else if (trimmedUpperCommand.startsWith("INFO ")) {
    if (match_address_flag) {
      int my_infoType = trimmedUpperCommand.substring(4).toInt();
      switch (my_infoType) {
        case Manufacture:
          response = COMPANY_NAME;
          break;
        case Model:
          response = DEVICE_UNDER_TEST;
          break;
        case Output_Voltage:
          response = g_voltageSetting;
          break;
        case Revision:
          response = VERSION;
          break;
        case Date_of_MFG:
          response = F(__DATE__ " " __TIME__);
          break;
        case Serial_Number:
          response = "Serial_Number";
          break;
        case Country_of_MFG:
          response = ORIGIN;
          break;
        default:
          // Do nothing for unknown INFO type
          break;
      }
    }
  }

  // Set Voltage, respond with an acknowledgment if the address matches
  else if (trimmedUpperCommand.startsWith("SV ")) {
    if (match_address_flag) {
      float voltageValue = trimmedUpperCommand.substring(3).toFloat();
      // Process the voltage setting (replace with actual logic)
      g_voltageSetting = voltageValue;
      response = "=>";  // Simplified acknowledgment
    }
  }

  // Read Voltage setting Query
  else if (trimmedUpperCommand == "SV?") {
    // Process the voltage setting query (replace with actual logic)
    float voltageValue = g_voltageSetting;  // Replace with actual voltage value
    response = "VOLTAGE:" + String(voltageValue, 3) + "V";
  }

  // Check Address,
  else if (trimmedUpperCommand.startsWith("ADDS ")) {
    g_parsedAddressSetting = trimmedUpperCommand.substring(4);
    if (g_parsedAddressSetting.toInt() == ADDRESS_SET) {
      // Set UART will set the device addressing flag to 1
      match_address_flag = true;
      response = "=>";
    }
    else {
      // Clear the device addressing flag to 0
      match_address_flag = false;
      // No response for incorrect address setting
    }
  }

  // Read Voltage
  else if (trimmedUpperCommand.startsWith("RV?")) {
    if (match_address_flag) {
      float currentValue = getCurrentValue(); // Replace with actual function
      response = "VOLTAGE:" + String(currentValue, 3) + "V";
    }
  }

  // Set Current, respond with an acknowledgment if the address matches
  else if (trimmedUpperCommand.startsWith("SI ")) {
    if (match_address_flag) {
      float currentValue = trimmedUpperCommand.substring(3).toFloat();
      // Process the current setting (replace with actual logic)
      g_currentLimitSetting = currentValue;
      response = "ACK_SET_CURRENT";
    }
  }

  // Get Current set, respond with an acknowledgment if the address matches
  else if (trimmedUpperCommand.startsWith("SI?")) {
    if (match_address_flag) {
      // Process the current setting (replace with actual logic)
      float currentValue = g_currentLimitSetting;
      response = "ACK_GET_CURRENT_SET";
    }
  }

  // Read Current, respond with a current value if the address matches
  else if (trimmedUpperCommand == "RI?") {
    if (match_address_flag) {
      float currentValue = getCurrentValue(); // Replace with actual function
      response = "CURRENT:" + String(currentValue, 3) + "A";
    }
  }

  // Read Temperature, respond with a temperature value of the uController if the address matches
  else if (trimmedUpperCommand == "RT?") {
    if (match_address_flag) {
      float result = float(temprature_sens_read());
      result = ((result - 32) / 1.8);  // Convert to C
      response = "TEMPERATURE:" + String(result, 0) + "C";
    }
  }

  // Other commands requiring ADDS flag check
  else if (match_address_flag) {
    // Example: If the command is "*IDN?", respond with instrument identification
    if (trimmedUpperCommand == "*IDN?") {
      response = String(COMPANY_NAME) + String(PROG_NAME) + String(VERSION) + String(g_chip_Id);
    }

    // Get INFO<type>, respond one of six types
    else if (trimmedUpperCommand.startsWith("INFO ")) {
      int my_infoType = trimmedUpperCommand.substring(4).toInt();
      switch (my_infoType) {
        case Manufacture:
          response = COMPANY_NAME;
          break;
        case Model:
          response = DEVICE_UNDER_TEST;
          break;
        case Output_Voltage:
          response = g_voltageSetting;
          break;
        case Revision:
          response = VERSION;
          break;
        case Date_of_MFG:
          response = F(__DATE__ " " __TIME__);
          break;
        case Serial_Number:
          response = "Serial_Number";
          break;
        case Country_of_MFG:
          response = ORIGIN;
          break;
        default:
          // No details yet implemented
          response = "=> No details yet implemented for INFO type " + String(my_infoType);
          break;
      }
    }

    // Set Voltage, respond with an acknowledgment
    else if (trimmedUpperCommand.startsWith("SV ")) {
      float voltageValue = trimmedUpperCommand.substring(3).toFloat();
      // Process the voltage setting (replace with actual logic)
      g_voltageSetting = voltageValue;
      response = "=>";  // Simplified acknowledgment
    }

    // Read Voltage setting Query
    else if (trimmedUpperCommand == "SV?") {
      // Process the voltage setting query (replace with actual logic)
      float voltageValue = g_voltageSetting;  // Replace with actual voltage value
      response = "VOLTAGE:" + String(voltageValue, 3) + "V";
    }

    // Set Current, respond with an acknowledgment
    else if (trimmedUpperCommand.startsWith("SI ")) {
      float currentValue = trimmedUpperCommand.substring(3).toFloat();
      // Process the current setting (replace with actual logic)
      g_currentLimitSetting = currentValue;
      response = "=>";  // Simplified acknowledgment
    }

    // Get Current set, respond with an acknowledgment
    else if (trimmedUpperCommand.startsWith("SI?")) {
      // Process the current setting (replace with actual logic)
      float currentValue = g_currentLimitSetting;
      response = "ACK_GET_CURRENT_SET";
    }

    // Read Current, respond with a current value
    else if (trimmedUpperCommand == "RI?") {
      float currentValue = getCurrentValue(); // Replace with actual function
      response = "CURRENT:" + String(currentValue, 3) + "A";
    }

    // Read Temperature, respond with a temperature value
    else if (trimmedUpperCommand == "RT?") {
      float result = float(temprature_sens_read());
      result = ((result - 32) / 1.8);  // Convert to C
      response = "TEMPERATURE:" + String(result, 0) + "C";
    }

    // Global Power ON
    else if (trimmedUpperCommand == "GRPWR 1") {
      // Process the command (replace with actual logic)
      response = "=>";  // Simplified acknowledgment
    }

    // Global Power OFF
    else if (trimmedUpperCommand == "GRPWR 0") {
      // Process the command (replace with actual logic)
      response = "=>";  // Simplified acknowledgment
    }

    // Global Control O/P Voltage Setting
    else if (trimmedUpperCommand.startsWith("GSV ")) {
      float globalVoltageValue = trimmedUpperCommand.substring(4).toFloat();
      // Process the global voltage setting (replace with actual logic)
      response = "=>";  // Simplified acknowledgment
    }

    // Global Control O/P Current Setting
    else if (trimmedUpperCommand.startsWith("GSI ")) {
      float globalCurrentValue = trimmedUpperCommand.substring(4).toFloat();
      // Process the global current setting (replace with actual logic)
      response = "=>";  // Simplified acknowledgment
    }

    // Global Power ON / OFF Control
    else if (trimmedUpperCommand.startsWith("GLOB ")) {
      int globType = trimmedUpperCommand.substring(5).toInt();
      // Process the global power control (replace with actual logic)
      response = "=>";  // Simplified acknowledgment
    }

    // Power ON / OFF / Query
    else if (trimmedUpperCommand.startsWith("POWER ")) {
      int powerType = trimmedUpperCommand.substring(6).toInt();
      // Process the power control (replace with actual logic)
      response = "=>";  // Simplified acknowledgment
    }

    // Voltage setting Query for global control
    else if (trimmedUpperCommand == "GSV?") {
      // Process the global voltage setting query (replace with actual logic)
      float globalVoltageValue = 0.0;  // Replace with actual global voltage value
      response = "VOLTAGE:" + String(globalVoltageValue, 3) + "V";
    }

    // Current setting Query for global control
    else if (trimmedUpperCommand == "GSI?") {
      // Process the global current setting query (replace with actual logic)
      float globalCurrentValue = 0.0;  // Replace with actual global current value
      response = "CURRENT:" + String(globalCurrentValue, 3) + "A";
    }

    // Remote ON / OFF / Query
    else if (trimmedUpperCommand.startsWith("REMS ")) {
      int remoteType = trimmedUpperCommand.substring(5).toInt();
      // Process the remote control (replace with actual logic)
      response = "=>";  // Simplified acknowledgment
    }

    // Device Status Query
    else if (trimmedUpperCommand.startsWith("STUS ")) {
      int statusType = trimmedUpperCommand.substring(5).toInt();
      // Process the status query (replace with actual logic)
      response = "=>";  // Simplified acknowledgment
    }

    // Rate V/I Query
    else if (trimmedUpperCommand == "RATE?") {
      // Process the rate query (replace with actual logic)
      response = "=>";  // Simplified acknowledgment
    }

    // Device Name Query
    else if (trimmedUpperCommand == "DEVI?") {
      // Process the device name query (replace with actual logic)
      // The SL Power supply TF800 replied with Manufacturier and model number, 
      // for the mock supply model user PROG_NAME
      response = String(COMPANY_NAME) + String(PROG_NAME);       
    }

    // Unknown command
    else {
      // No response for unknown command
    }
  }
  // Send the response back to TF800
  if (!response.isEmpty()) {
    Serial.println(response);
    SerialTF8001.println(response);
    SerialTF8001.println("=>");  //This assumes all responses are valid.
  }
}
