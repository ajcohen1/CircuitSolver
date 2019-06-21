#pragma once

#include "BoardComponent.h"
#include <string>

class CircuitBoard
{
public:
	std::string testingVar;
	BoardComponent* firstComponentPtr;
	CircuitBoard(BoardComponent* firstComponent);
	void printCircuit();
	void addElement(BoardComponent* newComponent);
	void addNode();


};


