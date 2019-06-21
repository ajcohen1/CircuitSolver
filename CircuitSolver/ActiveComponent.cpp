
#include "ActiveComponent.h"
#include "BoardComponent.h"
#include "potentialElements.h"

	double magnitude = 0;
	int multiplier = 0; 
	potentialElements::allElements elementName; 

	ActiveComponent::ActiveComponent(BoardComponent* previousElement, potentialElements::allElements elementName, double magnitude, int multiplier) {
		if (previousElement != nullptr)
		{
			this->connections.push_front(previousElement);
		}

		this->ActiveComponent::elementName = elementName;
		this->magnitude = magnitude;
		this->multiplier = multiplier;

	}
