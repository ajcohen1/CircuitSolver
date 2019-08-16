#include "PassiveComponent.h"

PassiveComponent::PassiveComponent(std::string id, double magnitude, double multiplier, std::string unit) : NonWire(id, magnitude, multiplier, unit)
{
}

void PassiveComponent::setImpedance(double frequency) {
}