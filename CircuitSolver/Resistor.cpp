#include "Resistor.h"

Resistor::Resistor(std::string id, double magnitude, double multiplier) : PassiveComponent(id, magnitude, multiplier, "\u03A9")
{
}

void Resistor::setImpedance(double frequency)
{
	impedance = (this->magnitude * this->multiplier, 0);
}
