#include "Capacitor.h"
# define M_PI           3.14159265358979323846
#include <cmath>

Capacitor::Capacitor(std::string id, double magnitude, double multiplier) : PassiveComponent(id, magnitude, multiplier, "F")
{
}

void Capacitor::setImpedance(double frequency)
{
	impedance = (0, -1 / (2 * M_PI * frequency * multiplier * magnitude));
}