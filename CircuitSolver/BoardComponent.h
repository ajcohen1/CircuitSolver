#pragma once

#include <list>
#include <string>
class BoardComponent
{
public:
	std::string id;
	std::list<BoardComponent*> connections;
};
