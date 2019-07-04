#pragma once

#include "BoardComponent.h"
#include <string>
class PassiveComponent :
	public BoardComponent
{
public:
	PassiveComponent(std::string id,allCompTypes componentName, double magnitude, double multiplier);
};

