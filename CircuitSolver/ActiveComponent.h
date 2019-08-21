#pragma once
#include "NonWire.h"
#include <limits>
class ActiveComponent :
	public NonWire
{
public:
	double frequency = std::numeric_limits<double>::quiet_NaN();
	double phasor = std::numeric_limits<double>::quiet_NaN();

	ActiveComponent(std::string id, double magnitude, double multiplier, std::string unit, double frequency, double phasor);
	ActiveComponent(std::string id, double magnitude, double multiplier, std::string unit);
};

