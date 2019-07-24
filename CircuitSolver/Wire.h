#pragma once
#include "BoardComponent.h"
#include "NonWire.h"
#include <list>
class Wire :
	public BoardComponent
{
public:
	std::list<NonWire*> connections;
	Wire(std::string id);
};

