#pragma once
#include "NonWire.h"
#include <complex>
class PassiveComponent :
	public NonWire
{
public:
	std::complex<double> impedance;
	PassiveComponent(std::string id, double magnitude, double multiplier, std::string unit);
	virtual void setImpedance(double frequency);
};

