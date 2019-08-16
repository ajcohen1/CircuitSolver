#pragma once
#include <string>
#include "NonWire.h"
#include <vector>
#include <unordered_map>
#include "ActiveComponent.h"
class CircuitBuilder
{
public:
	enum currentTypes {DC, AC};
	enum connectionLocation { prev, next };
	currentTypes currentType;
	std::vector<NonWire*> circuitDraft;
	std::unordered_map<std::string, NonWire*> nonWireMap;
	unsigned long int wireCnt = 0;

	CircuitBuilder(ActiveComponent* firstComp);
	void connectToSingle(NonWire* newComp, std::string idOfConnectee, Wire* connectionPort);
	void connectToAll(NonWire* newComp, std::vector<std::string> idOfAllConnectees);
	std::vector<NonWire*> getCircuit();
	void printCircuit();
	void remove(std::string idOfCompToBeRemoved);
	void addNewCompToLocatorAndCircuitDraft(NonWire* newComp);
	void connectWireAndElement(Wire* wire, NonWire* element, Wire*& connectionPort);
	Wire* getNewWire();

private:
	void removeExternalConnections(Wire** wire, NonWire* compToBeRemoved);
	std::string getConnectionIDs(NonWire* element, Wire* adjacentWire);
	void connectWireAndElement(Wire* wire, NonWire* element, connectionLocation cl);
	NonWire* locate(std::string id);
};

