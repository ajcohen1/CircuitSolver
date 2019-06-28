#pragma once
#include "BoardComponent.h"
class Node :
	public BoardComponent
{
public:
	Node(std::string id);
	std::string id;
	bool isEssential;
};

