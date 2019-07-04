#pragma once
#include "BoardComponent.h"
class PowerSupply :
	public BoardComponent
{
public:
	PowerSupply(std::string id,allCompTypes componentName, double magnitude, double multiplier);
};

