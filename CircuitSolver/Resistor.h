#pragma once
#include "PassiveComponent.h"
class Resistor :
	public PassiveComponent
{
public:
	Resistor(std::string id, double magnitude, double multiplier);
	void setImpedance(double frequency);
};

