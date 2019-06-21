#pragma once
#include "BoardComponent.h"
class Node :
	public BoardComponent
{
public:
	Node(BoardComponent* previousElement);
};

