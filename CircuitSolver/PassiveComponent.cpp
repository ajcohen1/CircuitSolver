#include "PassiveComponent.h"

PassiveComponent::PassiveComponent(std::string id, double magnitude, double multiplier) : NonWire(id, magnitude, multiplier)
{
}

void PassiveComponent::setImpedance(double frequency) {
}