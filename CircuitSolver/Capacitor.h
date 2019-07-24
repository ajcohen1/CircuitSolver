#pragma once
#include "PassiveComponent.h"
class Capacitor :
	public PassiveComponent
{
public:
	Capacitor(std::string id, double magnitude, double multiplier);
	void setImpedance(double frequency);
};

