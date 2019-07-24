#include "ActiveComponent.h"

ActiveComponent::ActiveComponent(std::string id, double magnitude, double multiplier, double frequency, double phasor) : NonWire(id, magnitude, multiplier)
{
	this->frequency = frequency;
	this->phasor = phasor;
}


ActiveComponent::ActiveComponent(std::string id, double magnitude, double multiplier) : NonWire(id, magnitude, multiplier)
{
	this->frequency = NULL;
	this->phasor = NULL;
}