#include "CircuitBoard.h"
#include "BoardComponent.h"
#include "Node.h"
#include <string>
#include <iostream>


	std::string testingVar = "";

	CircuitBoard::CircuitBoard(BoardComponent* firstComponent)
	{
		this->firstComponentPtr = firstComponent;
		testingVar += 'T';

	};

	void CircuitBoard::printCircuit() {
		std::cout << testingVar << std::endl;
	}

	BoardComponent* firstComponentPtr;

	void CircuitBoard::addElement(BoardComponent* newComponent)
	{
		addNode();
		firstComponentPtr->connections.push_front(newComponent);
		testingVar += 'T';
	}


	void CircuitBoard::addNode() {
		BoardComponent* node = new Node(firstComponentPtr);
		firstComponentPtr->connections.push_front(node);
		firstComponentPtr = node;
		testingVar += 'N';
	}

