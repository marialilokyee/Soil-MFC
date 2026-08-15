/*
Datalogger for reading voltage output (single channel) via ADS1115, then send to ThingSpeak channel
Microcontroller: ESP32 
Analogue to Digital module: ADS1115
Total no. of ADS chennel: 8

Written by Maria Lok-Yee Li 
Version: 03/08/2026
*/

// Include libraries
#include <Adafruit_ADS1X15.h>
#include <Wire.h>  //I2C communication
#include <WiFi.h> //Wifi library
#include <ThingSpeak.h> // always include thingspeak header file after other header files and custom macros
#include "secrets.h"

Adafruit_ADS1115 ads1;  // First ADS1115 for the (0x48) address object
Adafruit_ADS1115 ads2; // Second ADS1115 for the (0x4A) address object

//Initialise WiFI configurations
char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

void setup(void)
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client); // Initialise ThingSpeak

  Serial.println("Getting single-ended readings from AIN0..3");
  Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV/ADS1015, 0.1875mV/ADS1115)");


  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  ads1.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV 
  ads2.setGain(GAIN_FOUR);
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV (cal:2mV)
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV (cal: 21.4-18 = 3.4mV)

  if (!ads1.begin(0x48)) {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }
  if (!ads2.begin(0x4A)) {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }
}

void loop(void) {
  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect");
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(SECRET_SSID, SECRET_PASS);
      delay(60000);
    }
    Serial.println("\nConnected.");
  }

  // read the value from the ADS1115:
  int16_t adc1_0, adc1_1, adc1_2, adc1_3, adc2_0, adc2_1, adc2_2, adc2_3;
  float volts1_0, volts1_1, volts1_2, volts1_3, volts2_0, volts2_1, volts2_2, volts2_3;

  adc1_0 = ads1.readADC_SingleEnded(0);
  adc1_1 = ads1.readADC_SingleEnded(1);
  adc1_2 = ads1.readADC_SingleEnded(2);
  adc1_3 = ads1.readADC_SingleEnded(3);
  adc2_0 = ads2.readADC_SingleEnded(0);
  adc2_1 = ads2.readADC_SingleEnded(1);
  adc2_2 = ads2.readADC_SingleEnded(2);
  adc2_3 = ads2.readADC_SingleEnded(3);

  // ComputeVolts equation volts = adc * Vgain / 2^15 [unit in V]
  volts1_0 = ads1.computeVolts(adc1_0)*1000;
  volts1_1 = ads1.computeVolts(adc1_1)*1000;
  volts1_2 = ads1.computeVolts(adc1_2)*1000;
  volts1_3 = ads1.computeVolts(adc1_3)*1000;
  volts2_0 = ads2.computeVolts(adc2_0)*1000;
  volts2_1 = ads2.computeVolts(adc2_1)*1000;
  volts2_2 = ads2.computeVolts(adc2_2)*1000;
  volts2_3 = ads2.computeVolts(adc2_3)*1000;

  // print sensor value
  Serial.println("-----------------------------------------------------------");
  Serial.print("AIN0: "); Serial.print(adc1_0); Serial.print("  "); Serial.print(volts1_0); Serial.println("mV");
  Serial.print("AIN1: "); Serial.print(adc1_1); Serial.print("  "); Serial.print(volts1_1); Serial.println("mV");
  Serial.print("AIN2: "); Serial.print(adc1_2); Serial.print("  "); Serial.print(volts1_2); Serial.println("mV");
  Serial.print("AIN3: "); Serial.print(adc1_3); Serial.print("  "); Serial.print(volts1_3); Serial.println("mV");
  Serial.print("AIN4: "); Serial.print(adc2_0); Serial.print("  "); Serial.print(volts2_0); Serial.println("mV");
  Serial.print("AIN5: "); Serial.print(adc2_1); Serial.print("  "); Serial.print(volts2_1); Serial.println("mV");
  Serial.print("AIN6: "); Serial.print(adc2_2); Serial.print("  "); Serial.print(volts2_2); Serial.println("mV");
  Serial.print("AIN7: "); Serial.print(adc2_3); Serial.print("  "); Serial.print(volts2_3); Serial.println("mV");
  
  // Set the ThingSpeak fields with ADS1115 values
  ThingSpeak.setField(1, volts1_0); // field 1
  ThingSpeak.setField(2, volts1_1); // field 2
  ThingSpeak.setField(3, volts1_2); // field 3
  ThingSpeak.setField(4, volts1_3); // field 4
  ThingSpeak.setField(5, volts2_0); // field 5
  ThingSpeak.setField(6, volts2_1); // field 6
  ThingSpeak.setField(7, volts2_2); // field 7
  ThingSpeak.setField(8, volts2_3); // field 8


  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(SECRET_CH_ID, SECRET_WRITE_APIKEY);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  
  delay(60000); //upload data via WiFi every 1 min
}
