#include "Wire.h"
#include "BoardComponent.cpp"

class Node: 
public BoardComponent{
public:
	bool isEssential = NULL;

	Node(BoardComponent *previousElement) {
		this->connections.push_front(previousElement);
	}

};