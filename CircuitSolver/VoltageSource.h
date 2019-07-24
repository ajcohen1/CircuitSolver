#pragma once
#include "ActiveComponent.h"
class VoltageSource :
	public ActiveComponent
{
public:
	VoltageSource(std::string id, double magnitude, double multiplier, double frequency, double phasor);
	VoltageSource(std::string id, double magnitude, double multiplier);
};

