#include "Capacitor.h"
# define M_PI           3.14159265358979323846
#include <cmath>

Capacitor::Capacitor(std::string id, double magnitude, double multiplier) : PassiveComponent(id, magnitude, multiplier)
{
	this->unit = "F";
}

void Capacitor::setImpedance(double frequency)
{
	impedance = -1* sqrt(-1) / (2 * M_PI * frequency * multiplier * magnitude);
}