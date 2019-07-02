
#include "ActiveComponent.h"
#include "BoardComponent.h"
#include "potentialElements.h"

#include <iostream>

	double magnitude = 0;
	int multiplier = 0; 
	potentialElements::allElements elementName; 

	ActiveComponent::ActiveComponent(potentialElements::allElements elementName, std::string id, double magnitude, int multiplier) : BoardComponent(id), id(id){
		this->ActiveComponent::elementName = elementName;
		this->magnitude = magnitude;
		this->multiplier = multiplier;
		this->id = id;
	}
