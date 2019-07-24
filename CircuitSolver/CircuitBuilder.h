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

	CircuitBuilder(ActiveComponent* firstComp);
	void connectToSingle(NonWire* newComp, std::string idOfConnectee);
	void connectToAll(NonWire* newComp, std::vector<std::string> idOfAllConnectees);
	std::vector<NonWire*> getCircuit();
	//void connectSingleElementToFirstCompGround(std::string);
	//void connectAllElementsToFirstCompGround(std::vector<std::string> idOfAllElements);
	//void remove(std::string compId);
	//void combineWires(Wire* wire1, Wire* wire2);

private:
	void addNewCompToLocatorAndCircuitDraft(NonWire* newComp);
	void connectWireAndElement(Wire* wire, NonWire* element, connectionLocation cl);
	NonWire* locate(std::string id);
};

