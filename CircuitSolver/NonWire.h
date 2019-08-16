#pragma once
#include "BoardComponent.h"
class Wire;
class NonWire :
	public BoardComponent
{
public:
	Wire* prev;
	Wire* next;
	double magnitude;
	double multiplier;
	std::string unit;

	NonWire(std::string id, double magnitude, double multiplier, std::string unit);
	~NonWire();
};
