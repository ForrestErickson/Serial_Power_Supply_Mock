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
//    temp_sensor_read_celsius(&result);
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
