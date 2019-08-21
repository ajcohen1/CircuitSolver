#include "Inductor.h"
# define M_PI           3.14159265358979323846
#include <cmath>

Inductor::Inductor(std::string id, double magnitude, double multiplier) : PassiveComponent(id, magnitude, multiplier, "H")
{
}
void Inductor::setImpedance(double frequency)
{
	impedance = (0, 2 * M_PI * magnitude * multiplier);
}