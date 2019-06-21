#pragma once
#include "BoardComponent.h"
#include "potentialElements.h"


class ActiveComponent :
	public BoardComponent
{
public:
	ActiveComponent(BoardComponent* previousElement, potentialElements::allElements elementName, double magnitude, int multiplier);

private:
	double magnitude;
	int multiplier;
	potentialElements::allElements elementName;
};
