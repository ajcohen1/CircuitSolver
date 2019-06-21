#include "Node.h"
#include "BoardComponent.h"

	bool isEssential = NULL;

	Node::Node(BoardComponent *previousElement) {
		this->connections.push_front(previousElement);
	}
