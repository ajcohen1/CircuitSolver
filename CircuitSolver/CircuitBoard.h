#pragma once

#include "BoardComponent.h"
#include <string>
#include <unordered_map>

class CircuitBuilder
{
private:
	BoardComponent firstComponent;
	int nodeCount;
public:
	std::unordered_map<std::string, BoardComponent*> componentLocator;

	//methods

	//sets the first element in circuit
	CircuitBuilder(BoardComponent firstComponent);

	//returns memory location of element
	BoardComponent* locate(std::string id);

	BoardComponent* getNode();

	//for all additions to circuit board, a node will first be added, then the element
	void connectToSingle(std::string id, BoardComponent* newComponent);
	void connectBetween(BoardComponent* newComponent, std::string idOfCompBefore, std::string idOfCompAfter);
	void connectToAll(BoardComponent* newComponent, std::string allConnections);

	void replace(std::string idOfComponentToBeReplaced, BoardComponent* replaceeComponent);
	
	void modifyComponent(std::string idOfCompToBeModified, double newMagnitude, int newMultiplier);
	
	BoardComponent* getCircuitBoard();
	std::unordered_map<std::string, BoardComponent*>* getComponentLocator();

};


