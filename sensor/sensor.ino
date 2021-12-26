/*
	Dept. of Atmospheric Sciences - IAG/USP
	Created by Ícaro Vaz Freire on 24/12/2021.
	Supervisor Prof. Márcia Yamasoe.
	São Paulo, Brazil.
*/
#include <SPI.h>
#include <SD.h>
#include <RTClib.h>
#include <Adafruit_MLX90614.h>

// References to external functions
String createTempDataFile(String dir, DateTime now);


// Instanciate global variables
const int chipSelect = 10;
RTC_DS1307 rtc;
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
String tempDataPath;
DateTime today;


// Use built-in LED as a status indicator
void blink(int freq) {
	int period = (int)(1000.0f / freq);
	digitalWrite(LED_BUILTIN, HIGH);
	delay(period);
	digitalWrite(LED_BUILTIN, LOW);
	delay(period);
};

// Initialize RTC
RTC_DS1307 initRTC() {
	Serial.println("Initializing RTC...");

	RTC_DS1307 rtc = RTC_DS1307();
	rtc.begin();
	if (!rtc.isrunning()) {
		Serial.println("Error initializing RTC.");
		while (1) blink(10);
	}
	Serial.println("RTC initialized.");
	return rtc;
}

// Initialize SD card and check for errors
void initSDcard() {
	Serial.println("Initializing SD card...");
	if (!SD.begin(chipSelect)) {
		Serial.println("SD card failed.");
		while(1) blink(10);
	}
	Serial.println("SD card initialized.");
}

// Initilize MLX90614 infra-red sensor and check for errors
void initIRsensor() {
	if (!mlx.begin()) {
		Serial.println("Error connecting to MLX sensor. Check wiring.");
		while(1) blink(10);
	};
}


// Main function
void setup() {
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW);

	// Initialize serial port
	Serial.begin(9600);
	while (!Serial) blink(5);

	Serial.println("Adafruit MLX90614 infra-red temperature sensoring");


	// Initialize stuff
	rtc = initRTC();
	initSDcard();
	initIRsensor();
	

	// Define paths to save the data
	today = rtc.now();
	tempDataPath = createTempDataFile("data/", today);

	Serial.println("System initilized.");
	digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
	DateTime now = rtc.now();

	// Check if a day has elapsed
	if (today.day() != now.day()) {
		tempDataPath = createTempDataFile("data/", now);
		Serial.println("A day has elapsed.");
		today = now;
	}

	// Create string that will be stored in the text file
	String dataString = "";
	dataString += (String)now.timestamp(DateTime::TIMESTAMP_TIME) + ",";
	dataString += (String)mlx.readAmbientTempC() + ",";
	dataString += (String)mlx.readObjectTempC();

	// Write to the text file
	File file = SD.open(tempDataPath, FILE_WRITE);
	if (file) {
		file.println(dataString);
		Serial.println(dataString);
	} else Serial.println("Failed!");
	file.close();

	// Wait
	delay(1000);
}