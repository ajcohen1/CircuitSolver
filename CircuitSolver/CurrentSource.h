#pragma once
#include "ActiveComponent.h"
class CurrentSource :
	public ActiveComponent
{
public:
	CurrentSource(std::string id, double magnitude, double multiplier, double frequency, double phasor);
	CurrentSource(std::string id, double magnitude, double multiplier);
};

