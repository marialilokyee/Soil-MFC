# Materials:
	•	2 x 4-channel 16-bit ADC (ADS1115) —  AZ-delivery
	•	1 x ESP32-WROOM dev board 
	•	Microbial Fuel Cells

# Software:
	•	Arduino IDE
	•	Adafruit ADS1115 library
	•	ESP32 by Espressif Systems add-on in IDE board manager

# Circuit wiring:
| 1st ADS1115                   | 2nd ADS1115                  | ESP32|
|-------------------------------|------------------------------|------|
| SDA                           | SDA                          | 21   |
| SCL                           | SCL                          | 22   |
| GND                           | GND                          | GND  |
| VDD                           | VDD                          | 3.3V |
| ADDR>> GND (I2C addrerss 0x48)|ADDR>> SDA (I2C addrerss 0x4A)|   /  |

| MFC     | ADS1115          |
|---------|------------------|
|anode(-) | GND              |
|cathode(+)| A0/A1/A2/A3     |

![Datalogger wiring](https://github.com/marialilokyee/phytonet/blob/d63f8a8ab85cab9f89614e190a7abc43facc1641/Photos/8%20shannel%20Datalogger_ESP32_ADS1115.png)

