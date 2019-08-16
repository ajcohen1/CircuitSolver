#include "ActiveComponent.h"

ActiveComponent::ActiveComponent(std::string id, double magnitude, double multiplier, std::string unit, double frequency, double phasor) : NonWire(id, magnitude, multiplier, unit)
{
	this->frequency = frequency;
	this->phasor = phasor;
}


ActiveComponent::ActiveComponent(std::string id, double magnitude, double multiplier, std::string unit) : NonWire(id, magnitude, multiplier, unit)
{
	this->frequency = NULL;
	this->phasor = NULL;
}