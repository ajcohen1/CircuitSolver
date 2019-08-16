#include "CurrentSource.h"

CurrentSource::CurrentSource(std::string id, double magnitude, double multiplier, double frequency, double phasor) : ActiveComponent(id, magnitude, multiplier, "A", frequency, phasor)
{
}
CurrentSource::CurrentSource(std::string id, double magnitude, double multiplier) : ActiveComponent(id, magnitude, multiplier, "A")
{
}