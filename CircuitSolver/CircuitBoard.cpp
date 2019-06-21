#include "CircuitBoard.h"
#include "BoardComponent.h"
#include "Node.h"
#include "BoardComponent.cpp"
#include <string>
#include <iostream>

using namespace std;

class CircuitBoard
{
public:

	string testingVar = "";

	CircuitBoard(BoardComponent* firstComponent)
	{
		this->firstComponentPtr = firstComponent;

	};

	void printCircuit() {
		cout << testingVar << endl;
	}
private:
	int count;
	BoardComponent* firstComponentPtr;

	void addElement(BoardComponent* newComponent)
	{
		firstComponentPtr->connections.push_front(newComponent);
		testingVar += 'T';
	}


	void addNode() {
		BoardComponent* node = new Node(firstComponentPtr);
		firstComponentPtr->connections.push_front(node);
		firstComponentPtr = node;
		testingVar += 'N';
	}
};
