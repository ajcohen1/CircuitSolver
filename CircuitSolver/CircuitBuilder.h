#pragma once

#include "BoardComponent.h"
#include "Node.h"
#include <string>
#include <unordered_map>

class CircuitBuilder
{
private:
	BoardComponent firstComponent;
public:
	std::unordered_map<std::string, BoardComponent*> componentLocator;

	//methods
	CircuitBuilder(BoardComponent initialComponent);
	BoardComponent* locate(std::string id);
	void remove(std::string id);
	void replace(std::string idOfComponentToBeReplaced, BoardComponent replaceeComponent);
	void connectToSingle(std::string id, BoardComponent* newComponent);
	void connectBetween(BoardComponent newComponent, std::string idOfCompBefore, std::string idOfCompAfter);
	void connectToAll(BoardComponent* newComponent, std::string allConnections[], int numComponents);
	
	Node* createNode();
	void updateNode(Node* node);


};


