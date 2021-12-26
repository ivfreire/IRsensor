/*
	Dept. of Atmospheric Sciences - IAG/USP
	Created by Ícaro Vaz Freire on 24/12/2021.
	Supervisor Prof. Márcia Yamasoe.
	São Paulo, Brazil.
*/
#include "RTClib.h"
#include "SPI.h"
#include "SD.h"
#include <Arduino.h>

// Create file where date will be stored
String createTempDataFile(String dir, DateTime now) {
	String filename = (String)now.year()+(String)now.month()+(String)now.day()+".txt";
	String path = dir + filename;
	File file = SD.open(path);
	if (!file) {
		file = SD.open(path, FILE_WRITE);
		if (file) {
			file.println("# Adafruit MRX90614 - GY-906 sensor");
			file.println("# Infra-red temperature data taken on " + now.timestamp(DateTime::TIMESTAMP_DATE) + ".");
			file.println("# Location: Guarulhos, SP");
			file.println("time,env_t,obj_t");
			Serial.println("Created new data file " + path);
		} else Serial.println("Could not create file at " + path + ".");
	}
	file.close();
	return path;
}