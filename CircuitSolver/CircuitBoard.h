#pragma once
#include "CircuitBuilder.h"
#include <list>

class CircuitBoard
{
public:
	std::list<BoardComponent*> cg;
	CircuitBoard(CircuitBuilder* cb);
	void printBoard();
};

