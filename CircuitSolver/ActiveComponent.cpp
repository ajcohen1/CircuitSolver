#include "ActiveComponent.h"
#include "BoardComponent.cpp"
#include "potentialElements.cpp"

class ActiveComponent :
	public BoardComponent {

	double magnitude = 0;
	int multiplier = 0; 
	potentialElements::allElements elementName; 
	int numId = 0;

	ActiveComponent(BoardComponent& previousElement, potentialElements::allElements elementName, double magnitude, int multiplier, int numId) {
		this->connections.push_front(previousElement);
		this->elementName = elementName;
		this->magnitude = magnitude;
		this->multiplier = multiplier;
		this->numId = numId;

	}


};