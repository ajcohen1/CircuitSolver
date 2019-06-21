#ifndef BOARDCOMPONENT_H
#define BOARDCOMPONENT_H

#include "BoardComponent.h"
#include "potentialElements.h"


class ActiveComponent :
	public BoardComponent
{
public:
	ActiveComponent(BoardComponent* previousElement, potentialElements::allElements elementName, double magnitude, int multiplier);
};

#endif