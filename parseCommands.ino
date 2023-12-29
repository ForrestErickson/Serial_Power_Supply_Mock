// Process the received command and generate a response
void handleCommand(String command) {
  //  Serial.print("Received command: ");
  //  Serial.println(command);

  // Convert the command to uppercase and trim
  String trimmedUpperCommand = command;
  trimmedUpperCommand.trim();

  // Declare the response variable
  String response;

  // Example: If the command is "*IDN?", respond with instrument identification
  if (trimmedUpperCommand == "*IDN?") {
    response = "TF800,Manufacturer,Model,Version,SerialNumber";
    //   Serial.println(response);
  }

  // Set Voltage, respond with an acknowledgment
  else if (trimmedUpperCommand.startsWith("SV ")) {
    float voltageValue = trimmedUpperCommand.substring(3).toFloat();
    // Process the voltage setting (replace with actual logic)
    g_voltageSetting = voltageValue;
    Serial.println(voltageValue);
    response = "ACK_SET_VOLTAGE";
  }

  //ADDS <adds> Device Addressing
  /*
    Syntax: ADDS <adds>
    Parameter: 0 <= adds <= 7
    Description: When device receives a command, even if the addressing flag is set to 1 or clear to 0, UART will
    execute this command. Only if the addressing of device is the same with <adds>, UART will set the device
    addressing flag to 1 and reply “ = > CR LF ” to express that the execution is completed. Otherwise, the
    addressing of the device isn't the same with <adds>, UART will set the device addressing flag to 0, but will not
    reply. If there are no devices exist, all device addressing flag will be clear to 0, and device will not reply.
  */
  // Check Address,
  else if (trimmedUpperCommand.startsWith("ADDS ")) {
    g_addressSetting = trimmedUpperCommand.substring(4);
    float voltageValue = trimmedUpperCommand.substring(4).toFloat();
    Serial.print("My g_addressSetting= ");
    Serial.println(g_addressSetting);

    //If the address received = ADDRESS_SET, set address flat to 1 and reply “ = > CR LF ”

    if (g_addressSetting.toInt() == ADDRESS_SET) {
      Serial.print("g_addressSetting= ADDRESS_SET = ");
      Serial.println(response);
      //Set UART will set the device addressing flag to 1
      match_address_flag = true;
      response = "=>";
    }
    else { //Did not match so no reply
      Serial.print("g_addressSetting != ADDRESS_SET = ");
      Serial.println(response);
      //Clear the device addressing flag to 0
      match_address_flag = false;
      response = "";
    }
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

  // Read Temperature", respond with a temperature value of the uController
  else if (trimmedUpperCommand == "RT?") {
    float result = 0;
    result = float(temprature_sens_read());
    result = ((result - 32) / 1.8);  //Convert to C
    response = "TEMPERATURE:" + String(result, 0) + "C";
  }

  // Unknown command
  else {
    response = "UNKNOWN_COMMAND";
  }

  // Send the response back to TF800
  Serial.println(response);
  SerialTF8001.println(response);

}
// end handleCommand()
