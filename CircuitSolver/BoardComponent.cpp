#include "BoardComponent.h"
#include <iostream>

BoardComponent::BoardComponent(std::string id = ""): id(id) {
	this->id = id;
}

BoardComponent::~BoardComponent() {
	
	//access each individual connection
	auto allConnections = this->connections;
	for (auto aConnection : allConnections) {

		//access neighbor connections
		auto allNeighborConnections = aConnection->connections;
		allNeighborConnections.remove(this);

	}
}