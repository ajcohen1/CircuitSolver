#pragma once

#include <list>
#include <string>
#include <iostream>
#include <sstream>

class BoardComponent
{
public:
	enum allCompTypes { RESISTOR, CAPACITOR, INDUCTOR, AC_Voltage, AC_Current, DC_Voltage, DC_Current, WIRE };
	allCompTypes compType;
	BoardComponent(std::string id, allCompTypes componentName, double magnitude, double multiplier);
	~BoardComponent();
	std::list<BoardComponent*> connections;
	std::string id;
	double magnitude;
	double multiplier;
};

