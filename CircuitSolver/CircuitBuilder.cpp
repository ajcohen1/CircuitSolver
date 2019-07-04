#include "CircuitBuilder.h"
#include "BoardComponent.h"
#include <list>
#include <string>
#include "CircuitBoard.h"

//this is the overall graph
std::list<BoardComponent*> circuitGraph;

//this will be a map to all the components of the circuit
std::map<std::string, BoardComponent*> componentLocator;

//this sets the first link in the circuit linked list
CircuitBuilder::CircuitBuilder(BoardComponent* initialComponent) {

	std::string testID = initialComponent->id;


	//add to locator map
	componentLocator[initialComponent->id] = initialComponent;

	//add to graph
	circuitGraph.push_back(initialComponent);
}

//given the id of an element, you will get its memory address
BoardComponent* CircuitBuilder::locate(std::string id) {
	return componentLocator.at(id);
}

//connects a new component to a single element on the circuit board
void CircuitBuilder::connectToSingle(std::string id, BoardComponent* newComponent) {
	
	//finds element that newComp will be connected to
	BoardComponent* neighborComp = locate(id);

	//connects newComp to neighbor's connections
	neighborComp->connections.push_back(newComponent);

	//connects the neighbor to newComp's  connections
	newComponent->connections.push_back(neighborComp);

	//adds newComp to map
	componentLocator[newComponent->id] = newComponent;

	//adds newComp to graph
	circuitGraph.push_back(newComponent);
}

//connects a new component to multiple elements on the circuit board
void CircuitBuilder::connectToAll(BoardComponent* newComponent, std::string allConnections[], int numComponents) {

	//add new component to Circuit and componentLocator
	componentLocator[newComponent->id] = newComponent;
	circuitGraph.push_back(newComponent);


	for (int numComponent = 0; numComponent < numComponents; numComponent++) {

		//get the neigboring component
		std::string neighborCompId = allConnections[numComponent];
		BoardComponent* neighborComp = locate(neighborCompId);

		//connect the newComp to neighbors connections
		neighborComp->connections.push_back(newComponent);

		//connect the neighbor to newComps connections
		newComponent->connections.push_back(neighborComp);

	}
}


void CircuitBuilder::remove(std::string id)
{
	BoardComponent* compToBeRemoved = locate(id);
	compToBeRemoved->~BoardComponent();
	componentLocator.erase(id);
	circuitGraph.remove(compToBeRemoved);
	delete compToBeRemoved;
}

void CircuitBuilder::replace(std::string idOfComponentToBeReplaced, BoardComponent* replaceeComponent)
{
	BoardComponent* compToBeReplaced = locate(idOfComponentToBeReplaced);
	auto remainingConnections = compToBeReplaced->connections;
	remove(idOfComponentToBeReplaced);

	for (auto aConnection : remainingConnections) {
		replaceeComponent->connections.push_back(aConnection);
		aConnection->connections.push_back(replaceeComponent);
	}

	componentLocator[replaceeComponent->id] = replaceeComponent;
	circuitGraph.push_back(replaceeComponent);
}

std::list<BoardComponent*> CircuitBuilder::getCircuitGraph()
{
	return circuitGraph;
}

std::vector<BoardComponent*> CircuitBuilder::getNodes() {
	
	//set up container of nodes
	std::vector<BoardComponent*> allNodes;

	//begin searching
	bool isWire;
	for (auto const& mapper : componentLocator) {

		BoardComponent* component = mapper.second;
		auto componentType = component->compType;

		isWire = (componentType == BoardComponent::WIRE);

		if (isWire) {
			allNodes.push_back(component);
		}
	}

	return allNodes;
}
