#pragma once

#include "BoardComponent.h"
#include "Node.h"
#include <string>
#include <unordered_map>

class CircuitBuilder
{
private:
	BoardComponent* firstComponent;
	std::list<BoardComponent*> circuitGraph;
public:
	std::unordered_map<std::string, BoardComponent*> componentLocator;

	//methods
	CircuitBuilder(BoardComponent* initialComponent);
	BoardComponent* locate(std::string id);
	void remove(std::string id);
	void replace(std::string idOfComponentToBeReplaced, BoardComponent* replaceeComponent);
	void connectToSingle(std::string id, BoardComponent* newComponent);
	void connectToAll(BoardComponent* newComponent, std::string allConnections[], int numComponents);
	std::list<BoardComponent*> getCircuitGraph();
};


