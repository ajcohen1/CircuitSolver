#pragma once
#include "PassiveComponent.h"
class Inductor :
	public PassiveComponent
{
public:
	Inductor(std::string id, double magnitude, double multiplier);
	void setImpedance(double frequency);
};

