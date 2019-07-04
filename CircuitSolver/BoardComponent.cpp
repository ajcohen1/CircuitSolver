#include "BoardComponent.h"

BoardComponent::BoardComponent(std::string id, allCompTypes componentName, double magnitude, double multiplier) {
	this->id = id;
	this->compType = componentName;
	this->magnitude = magnitude;
	this->multiplier = multiplier;
}

BoardComponent::~BoardComponent(){

	auto allConnections = this->connections;
	for (auto aConnection : allConnections) {

		//access neighbor connections
		auto allNeighborConnections = aConnection->connections;
		allNeighborConnections.remove(this);

	}

}