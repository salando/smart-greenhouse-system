float readTemperatureC() {
  int raw = analogRead(TEMP_PIN);    // Read the sensor: a number from 0 to 1023
  if (raw <= 0 || raw >= 1023) {     // 0 or 1023 means the cable is loose or unplugged
    sensorError = true;              // Remember that something is wrong
    return temperatureC;             // Keep the last good temperature instead of a nonsense number
  }                                  // End of the error check
  sensorError = false;               // The reading looks fine
  float resistance = (1023.0 - raw) * 10000.0 / raw;   // Work out the sensor's resistance from the reading
  float kelvin = 1.0 / (log(resistance / 10000.0) / B_VALUE + 1.0 / 298.15);  // Thermistor formula gives Kelvin
  return kelvin - 273.15;            // Convert Kelvin to Celsius and send it back
}                                 
