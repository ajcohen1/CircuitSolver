#include "Inductor.h"
# define M_PI           3.14159265358979323846
#include <cmath>

Inductor::Inductor(std::string id, double magnitude, double multiplier) : PassiveComponent(id, magnitude, multiplier)
{
	this->unit = "H";
}
void Inductor::setImpedance(double frequency)
{
	impedance = sqrt(-1) * 2 * M_PI * magnitude * multiplier;
}