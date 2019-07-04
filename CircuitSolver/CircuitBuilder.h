#pragma once

#include "BoardComponent.h"
#include <string>
#include <map>
#include <vector>

class CircuitBuilder
{
private:
	std::list<BoardComponent*> circuitGraph;
public:
	std::map<std::string, BoardComponent*> componentLocator;

	//methods
	CircuitBuilder(BoardComponent* initialComponent);
	BoardComponent* locate(std::string id);
	void remove(std::string id);
	void replace(std::string idOfComponentToBeReplaced, BoardComponent* replaceeComponent);
	void connectToSingle(std::string id, BoardComponent* newComponent);
	void connectToAll(BoardComponent* newComponent, std::string allConnections[], int numComponents);
	std::list<BoardComponent*> getCircuitGraph();
	std::vector<BoardComponent*> getNodes();
};


