#pragma once

#include "BoardComponent.h"
#include <string>

class CircuitBoard
{
public:
	std::string testingVar;
	CircuitBoard(BoardComponent* firstComponent);
	void printCircuit();

private:
	BoardComponent* firstComponentPtr;
	void addElement(BoardComponent* newComponent);
	void addNode();

};


