#include "CircuitBuilder.h"
#include "PassiveComponent.h"
#include "ActiveComponent.h"
#include "Wire.h"
#include <iostream>

enum currentTypes { DC, AC };
currentTypes currentType;
std::vector<NonWire*> circuitDraft;
std::unordered_map<std::string, NonWire*> nonWireMap;

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

void CircuitBuilder::connectToSingle(NonWire* newComp, std::string idOfConnectee)
{
	//get wire to be connected to, known as connectee
	NonWire* connectee = locate(idOfConnectee);

	//create the new wire
	Wire* wire = new Wire("test");

	//add the wire to the conectee
	connectWireAndElement(wire, connectee, connectionLocation::next);

	//add the wire to the newComponent
	connectWireAndElement(wire, newComp, connectionLocation::prev);

	//add newComp to locator and adjaceny graph
	addNewCompToLocatorAndCircuitDraft(newComp);

}

void CircuitBuilder::connectToAll(NonWire* newComp, std::vector<std::string> idOfAllConnectees)
{
	//create the new wire to be connected to all the connectees
	Wire* wire = new Wire("test");

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


int main()
{
	ActiveComponent* vs = new ActiveComponent("V1", 5.0, 1.0);
	PassiveComponent* r = new PassiveComponent("R1", 1.0, 2);

	CircuitBuilder* cb = new CircuitBuilder(vs);
	cb->connectToSingle(r, "V1");

	//print the circuit
	auto circuitGraph = cb->getCircuit();

	std::cout << (circuitGraph.size());
}