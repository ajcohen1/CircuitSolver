#pragma once
#include "BoardComponent.h"
#include "potentialElements.h"


class ActiveComponent :
	virtual public BoardComponent
{
public:
	ActiveComponent(potentialElements::allElements elementName, std::string id,double magnitude, int multiplier);
	std::string id;

private:
	double magnitude;
	int multiplier;
	potentialElements::allElements elementName;
};
