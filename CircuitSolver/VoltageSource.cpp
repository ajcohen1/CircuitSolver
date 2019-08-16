#include "VoltageSource.h"
VoltageSource::VoltageSource(std::string id, double magnitude, double multiplier, double frequency, double phasor) : ActiveComponent(id, magnitude, multiplier, "V", frequency, phasor)
{
}
VoltageSource::VoltageSource(std::string id, double magnitude, double multiplier) : ActiveComponent(id, magnitude, multiplier, "V") 
{
}