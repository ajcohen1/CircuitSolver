#pragma once
#include "NonWire.h"
class ActiveComponent :
	public NonWire
{
public:
	double frequency;
	double phasor;

	ActiveComponent(std::string id, double magnitude, double multiplier, std::string unit, double frequency, double phasor);
	ActiveComponent(std::string id, double magnitude, double multiplier, std::string unit);
};

