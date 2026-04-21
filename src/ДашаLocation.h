//проверка на повторный запуск
#ifndef ДАШАLOCATION_H
#define ДАШАLOCATION_H

#include <iostream>
#include <string>
#include <nlohmann/json.hpp> // подключение библиотеки nlohmann/json.hpp
#include "TerminalDisplay.h"

using namespace std;

class GeoLocationData {
public:
	//объявление переменных
	string ip;
	string country;
	string countryCode;
	string regionName;
	string city;
	string zip;
	double lat;
	double lon;
	string timezone;
	string isp;
	string org;
	string as;
	string status;
	string message;
	bool mobile;
	bool proxy;
	bool hosting;

	GeoLocationData() : ip("NONE"), country("NONE"), countryCode("NONE"),
		regionName("NONE"), city("NONE"), zip("NONE"),
		lat(0.0), lon(0.0), timezone("NONE"), isp("NONE"),
		org("NONE"), as("NONE"), status("NONE"), message("NONE") {
	}

	void display() const;
};

void to_json(nlohmann::json& j, const GeoLocationData& p);
void from_json(const nlohmann::json& j, GeoLocationData& p);


#endif // !ДАШАLOCATION_H
