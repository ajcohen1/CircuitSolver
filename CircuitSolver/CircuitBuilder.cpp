#include "CircuitBuilder.h"
#include "PassiveComponent.h"
#include "ActiveComponent.h"
#include "Wire.h"
#include <algorithm>
#include <iostream>

enum currentTypes { DC, AC };
currentTypes currentType;
std::vector<NonWire*> circuitDraft;
std::unordered_map<std::string, NonWire*> nonWireMap;
unsigned long int wireCnt = 0;

CircuitBuilder::CircuitBuilder(ActiveComponent* firstComp)
{

	//get the current type
	currentType = currentTypes::AC;
	if (firstComp->frequency == NULL)
		currentType = currentTypes::DC;

	//add newComp to locator and adjaceny graph
	addNewCompToLocatorAndCircuitDraft(firstComp);

}

NonWire* CircuitBuilder::locate(std::string id)
{
	return nonWireMap[id];
}

void CircuitBuilder::connectWireAndElement(Wire* wire, NonWire* element, connectionLocation cl)
{
	//connect wire to element
	wire->connections.push_back(element);

	//connect element to wire...

	//determine if should connect to prev or next
	if (cl == connectionLocation::next) {
		element->next = wire;
	}
	else {
		element->prev = wire;
	}
}

void CircuitBuilder::connectWireAndElement(Wire* wire, NonWire* element, Wire*& connectionPort)
{
	//connect wire to element
	wire->connections.push_back(element);

	//connect element to wire...
	connectionPort = wire;
}

void CircuitBuilder::connectToSingle(NonWire* newComp, std::string idOfConnectee, Wire* connectionPort)
{
	//get wire to be connected to, known as connectee
	NonWire* connectee = locate(idOfConnectee);

	Wire* wire;

	//create the new wire
	wire = connectionPort;
	if (connectionPort == nullptr)
	{
		wire = getNewWire();

		//add the wire to the conectee
		connectWireAndElement(wire, connectee, connectionLocation::next);
	}

	//add the wire to the newComponent
	connectWireAndElement(wire, newComp, connectionLocation::prev);

	//add newComp to locator and adjaceny graph
	addNewCompToLocatorAndCircuitDraft(newComp);

}

void CircuitBuilder::connectToAll(NonWire* newComp, std::vector<std::string> idOfAllConnectees)
{
	//create the new wire to be connected to all the connectees
	Wire* wire = getNewWire();

	//now connect the wire to all the connectees and the newComp
	for (std::string connecteeName : idOfAllConnectees)
	{
		//get the individual connectee from the container of all conectees
		NonWire* connectee = locate(connecteeName);

		//now connect it to the wire
		connectWireAndElement(wire, connectee, connectionLocation::next);
	}
	
	//now connect the newComp to the wire connected to all the connectees
	connectWireAndElement(wire, newComp, connectionLocation::prev);

	//add newComp to locator and adjaceny graph
	addNewCompToLocatorAndCircuitDraft(newComp);
}

void CircuitBuilder::addNewCompToLocatorAndCircuitDraft(NonWire* newComp)
{
	nonWireMap[newComp->id] = newComp;
	circuitDraft.push_back(newComp);
}


std::vector<NonWire*> CircuitBuilder::getCircuit()
{
	return circuitDraft;
}

void CircuitBuilder::printCircuit()
{
	for (NonWire* anElement : circuitDraft)
	{
		std::cout << anElement->id << "\n    ";
		
		Wire* elementPrev = anElement->prev;

		std::cout << "Prev: ";

		std::cout << getConnectionIDs(anElement, elementPrev) << "\n    ";

		std::cout << "Next: ";

		Wire* elementNext = anElement->next;
		std::cout << getConnectionIDs(anElement, elementNext);

		std::cout << std::endl;
	}
}

std::string CircuitBuilder::getConnectionIDs(NonWire* element, Wire* adjacentWire)
{

	bool noConnection = adjacentWire == nullptr;
	if (noConnection)
		return "";

	std::string connectionIDs = "";
	auto allConnections = adjacentWire->connections;

	for (auto aConnection : allConnections)
	{
		if (aConnection != element) {
			connectionIDs += aConnection->id;
			connectionIDs += " , ";
		}
	}

	return connectionIDs;
}

void CircuitBuilder::remove(std::string idOfCompToBeRemoved) 
{
	NonWire* compToBeRemoved = locate(idOfCompToBeRemoved);
	
	//if element is already disconnected, no further action is needed
	if (compToBeRemoved == nullptr || (compToBeRemoved->next == nullptr && compToBeRemoved->prev == nullptr))
		return;

	//remove element from external connections
	Wire** nextWire = &(compToBeRemoved->next);
	if(*nextWire != nullptr) removeExternalConnections(nextWire, compToBeRemoved);

	Wire** prevWire = &(compToBeRemoved->prev);
	if (*prevWire != nullptr) removeExternalConnections(prevWire, compToBeRemoved);

	//set prev and next connection to null
	compToBeRemoved->next = nullptr;
	compToBeRemoved->prev = nullptr;

	//remove from map and graph
	auto a = std::remove(circuitDraft.begin(), circuitDraft.end(), compToBeRemoved);
	circuitDraft.erase(a, circuitDraft.end());

	nonWireMap.erase(nonWireMap.find(idOfCompToBeRemoved));

	//now delete element
}

Wire* CircuitBuilder::getNewWire()
{
	wireCnt++;
	std::string newWireID = "Wire ";
	newWireID += std::to_string(wireCnt);

	Wire* newWire = new Wire(newWireID);

	return newWire;
}

void CircuitBuilder::removeExternalConnections(Wire** wire, NonWire* compToBeRemoved)
{
	((*wire)->connections).remove(compToBeRemoved);
}
