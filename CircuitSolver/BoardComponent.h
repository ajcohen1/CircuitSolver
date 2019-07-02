#pragma once

#include <list>
#include <string>
#include "potentialElements.h"
class BoardComponent
{
public:
	std::string id;
	std::list<BoardComponent*> connections;
	BoardComponent(std::string id);
	~BoardComponent();
};
