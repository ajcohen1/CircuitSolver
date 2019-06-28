#include "CircuitBuilder.h"
#include "BoardComponent.h"
#include "Node.h"
#include <list>
#include <string>
#include <iostream>

//this will act as a cursor that connects a user to the overall linked list
BoardComponent firstComponent;

//count of nodes
int nodeCount = 0;

//this will be a map to all the components of the circuit
std::unordered_map<std::string, BoardComponent*> componentLocator;

//this sets the first link in the circuit linked list
CircuitBuilder::CircuitBuilder(BoardComponent initialComponent) {

	//make a shallow copy of the parameter into our initial 
	firstComponent = initialComponent;
	std::string firstCompId = initialComponent.id;
	
	//add to locator map
	componentLocator[firstCompId] = &firstComponent;
}

//given the id of an element, you will get its memory address
BoardComponent* CircuitBuilder::locate(std::string id) {
	return componentLocator.at(id);
}

//creates a new node
Node* CircuitBuilder::createNode() {
	nodeCount++;
	std::string nodeId = "N" + nodeCount;
	Node* newNode = new Node(nodeId);
	return newNode;
}


//updates attributes of node whenever a node is modified or added
void CircuitBuilder::updateNode(Node* node) {

	//updates if essential
	if (node->connections.size > 1)
		node->isEssential = true;

	/*

	NOTE:

	CONSIDER CREATING A VECTOR OF ESSENTIAL NODES

	*/

	//updates id

	/*


	NOTE:

	NEED TO REMOVE KEY FROM HASHMAP IF THEY DIFFER
	AND ADD IN NEW HASH KEY


	*/
	std::string updatedId = "";

	for (BoardComponent* component : node->connections)
		updatedId += component->id;

	node->id = updatedId;
}

//connects a new component to a single element on the circuit board
void CircuitBuilder::connectToSingle(std::string id, BoardComponent* newComponent) {
	BoardComponent* adjacentComponent = locate(id);

	Node* newNode = createNode();

	//add node to adjacent component connections
	adjacentComponent->connections.push_back(newNode);

	//add adjacent component to node connections
	newNode->connections.push_front(adjacentComponent);

	//add new component to node connections
	newNode->connections.push_front(newComponent);

	//update newNode
	updateNode(newNode);

	//add node to newComponenet connections
	newComponent->connections.push_front(newNode);
}

//connects a new component to multiple elements on the circuit board
void CircuitBuilder::connectToAll(BoardComponent* newComponent, std::string allConnections[], int numComponents) {
	for (int numComponent = 0; numComponent < numComponents; numComponent++) {
		connectToSingle(allConnections[numComponent], newComponent);
	}
}

void CircuitBuilder::remove(std::string id)
{
	BoardComponent* compToBeRemoved = locate(id);
	auto allConnections = compToBeRemoved->connections;
	for (BoardComponent* singleConnection : allConnections) {

		//remove connections to compToBeReomved

		//get to connected element and its connections
		BoardComponent* connectedComponent = allConnections.front;
		std::list<BoardComponent*> connectedComponentConnections = connectedComponent->connections;

		//remove compToBeRemoved
		connectedComponentConnections.remove(compToBeRemoved);

	}

	//removed connections from compToBeRemoved
	allConnections.erase;
}

