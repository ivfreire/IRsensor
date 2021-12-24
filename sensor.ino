#include <SPI.h>
#include <SD.h>
#include <RTClib.h>
#include <Adafruit_MLX90614.h>


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

String createTempDataFile(String dir) {
	DateTime now = rtc.now();
	String filename = (String)now.year()+(String)now.month()+(String)now.day()+".txt";
	String path = dir + "/" + filename;
	File file = SD.open(path);
	if (!file) {
		file = SD.open(path, FILE_WRITE);
		if (file) {
			file.println("# Infra-red temperature data taken on " + now.timestamp(DateTime::TIMESTAMP_DATE) + ".");
			file.println("# Adafruit MRX90614 - GY-906 sensor");
			file.println("time,env_t,obj_t");
			file.close();
		} else Serial.println("Could not create file at " + path + ".");
	} else file.close();
	return path;
}

void setup() {
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW);

	// Initialize serial port
	Serial.begin(9600);
	while (!Serial) blink(5);

	Serial.println("Adafruit MLX90614 infra-red temperature sensoring");

	// Initialize RTC
	Serial.println("Initializing RTC...");
	rtc = RTC_DS1307();
	rtc.begin();
	if (!rtc.isrunning()) {
		Serial.println("Error initializing RTC.");
		while (1) blink(10);
	}
	Serial.println("RTC initialized.");

	// Initialize SD card and check for errors
	Serial.println("Initializing SD card...");
	if (!SD.begin(chipSelect)) {
		Serial.println("SD card failed.");
		while(1) blink(10);
	}
	Serial.println("SD card initialized.");

	// Initilize MLX90614 infra-red sensor and check for errors
	if (!mlx.begin()) {
		Serial.println("Error connecting to MLX sensor. Check wiring.");
		while(1) blink(10);
	};

	// Define paths to save the data
	DateTime now = rtc.now();
	tempDataPath = createTempDataFile("data");

	today = rtc.now();

	Serial.println("System initilized.");
	digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
	DateTime now = rtc.now();

	// Check if a day has elapsed
	if (now.day() != today.day()) {
		tempDataPath = createTempDataFile("data");
		today = now;
	}

	String dataString = "";

	dataString += now.timestamp(DateTime::TIMESTAMP_TIME);
	dataString += ",";
	dataString += mlx.readAmbientTempC();
	dataString += ",";
	dataString += mlx.readObjectTempC();

	File file = SD.open(tempDataPath, FILE_WRITE);
	if (file) {
		file.println(dataString);
		file.close();
		Serial.println(dataString);
	} else Serial.println("Failed!");

	delay(1000);
}