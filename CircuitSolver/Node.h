#ifndef NODE_H
#define NODE_H
#include "BoardComponent.h"
class Node :
	public BoardComponent
{
public:
	Node(BoardComponent* previousElement);
};

#endif