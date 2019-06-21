#pragma once

#include <list>
class BoardComponent
{
public:
	std::list<BoardComponent*> connections;
};
