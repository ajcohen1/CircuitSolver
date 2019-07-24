#pragma once
#include <string>
class BoardComponent
{
public:
	std::string id;
	BoardComponent(std::string id);
	virtual ~BoardComponent();
};

