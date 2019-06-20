#include "Wire.h"
#include "BoardComponent.cpp"

class Wire: 
public BoardComponent{
public:
	bool isNode = false;

	Wire(BoardComponent &previousElement) {
		this->connections.push_front(previousElement);
	}

};