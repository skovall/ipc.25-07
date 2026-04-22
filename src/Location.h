#ifndef LOCATION_H
#define LOCATION_H

#include <iostream>
#include <string>
#include <nlohmann/json.hpp> // подключение библиотеки nlohmann/json.hpp
#include "TerminalDisplay.h"

class GeoLocationData {
public:
	//объявление переменных
	std::string ip;
	std::string country;
	std::string countryCode;
	std::string regionName;
	std::string city;
	std::string zip;
	double lat;
	double lon;
	std::string timezone;
	std::string isp;
	std::string org;
	std::string as;
	std::string status;
	std::string message;
	bool mobile;
	bool proxy;
	bool hosting;

	GeoLocationData() : ip("NONE"), country("NONE"), countryCode("NONE"),
		regionName("NONE"), city("NONE"), zip("NONE"),
		lat(0.0), lon(0.0), timezone("NONE"), isp("NONE"),
		org("NONE"), as("NONE"), status("NONE"), message("NONE")
	{
	}

	void display() const;
};

void to_json(nlohmann::json& j, const GeoLocationData& p);
void from_json(const nlohmann::json& j, GeoLocationData& p);


#endif // !LOCATION_H
