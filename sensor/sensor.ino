/*
	Dept. of Atmospheric Sciences - IAG/USP
	Created by Ícaro Vaz Freire on 24/12/2021.
	Supervisor Prof. Márcia Akemi Yamasoe.
	São Paulo, Brazil.
*/
#include <SPI.h>
#include <SD.h>
#include <RTClib.h>
#include <Adafruit_MLX90614.h>

#define CHIP 10
#define DATA_DIR "data/"

// Instanciates global variables
RTC_DS1307 rtc;
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
String tempDataPath;
DateTime today;
int delayTime;

// Creates temperature data file path
String createTempDataPath(DateTime now) {
  String filename = (String)now.year()+(String)now.month()+(String)now.day()+".txt";
  return DATA_DIR + filename;
}

// Checks if file exists. And if it does not, creates it
void checkFile(String path) {
  if (!SD.exists(path)) {
    File file = SD.open(path, FILE_WRITE);
    if (file) {
      file.println("# Adafruit MRX90614 - GY-906 sensor");
      file.println("# Infra-red temperature measurements from the atmosphere and the environment.");
      file.println("# Location: São Paulo, SP");
      file.println("time,env_t,obj_t");  
      file.close();
    } else {
      Serial.println("Could not create data file " + path);
      while(1);
    }
  }
}

void loadConfig(String path) {
  // Load configuration from file at path
}

// Main function
void setup() {
	// Initializes serial port
	Serial.begin(9600);
	while (!Serial);

	Serial.println("Adafruit MLX90614 infra-red temperature sensoring");


	// Initializes stuff
	rtc = RTC_DS1307();
  rtc.begin();
  if (!rtc.isrunning()) {
    Serial.println("RTC failed.");
    while(1);
  }
  
  if (!SD.begin(CHIP)) {
    Serial.println("SD card failed.");
    while(1);
  }
  
	if (!mlx.begin()) { 
    Serial.println("MLX90614 initialized.");
	  while(1);
	}

  Serial.println("Everything initialized.");

  // Loading configuration file
  loadConfig("config.txt");

	// Defines path to save the data
	today = rtc.now();
	tempDataPath = createTempDataPath(today);

	Serial.println("System ready.");
}

// Main loop
void loop() {
	DateTime now = rtc.now();

	// Checks if a day has elapsed
	if (today.day() != now.day()) {
		tempDataPath = createTempDataPath(now);
		today = now;
    Serial.println("A day has elapsed.");
	}

	// Creates string that will be stored in the text file
	String dataString;
	dataString  = (String)now.timestamp(DateTime::TIMESTAMP_TIME) + ",";
	dataString += (String)mlx.readAmbientTempC() + ",";
	dataString += (String)mlx.readObjectTempC();

  checkFile(tempDataPath);

	// Writes to the text file
	File file = SD.open(tempDataPath, FILE_WRITE);
	if (file) {
		file.println(dataString);
		Serial.println(dataString);
	} else Serial.println("Failed!");
	file.close();

	// Waits
	delay(60000);
}
