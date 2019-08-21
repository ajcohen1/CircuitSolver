#pragma once
#include <string>
#include "NonWire.h"
#include <vector>
#include <unordered_map>
#include "ActiveComponent.h"
#include "CurrentSource.h"
#include "PassiveComponent.h"
#include "Resistor.h"
#include "Inductor.h"
#include "Capacitor.h"
#include <fort.hpp>

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

	//for ui
	void createNewCircuit();

	//for making a brand new circuit
	void sendWelcomeMessageAndNewCircuitDisclaimer();
	void determineCircuitCurrentType();
	void setUpElementIDAndValueTable();
	void determineFrequency();

	//for adding all new comps
	void loadAllNonWire();
	bool allCompsAdded();

	void loadSingleNonWire();
	int getNewCompType();
	int forceFirstCompToBeAnActiveComp();
 
	void loadPassiveComp(int compType);
	void loadActiveComp(int compType);
	void loadDCActiveComp(ActiveComponent* activeComp, int compType, std::string id, double magnitude, double multipler);
	void loadACActiveComp(ActiveComponent* activeComp, int compType, std::string id, double magnitude, double multipler);



	std::tuple<std::string, double, double> getID_Magnitude_andMultiplier();
	double getPhasor();

	void connectAllNonWire();
	void printAllLoadedElements();
	void printConnectionInstructions();
	void connectANonWire(NonWire* &aNonWire);

	void printElementPairConnections(NonWire* nonWire1, NonWire* nonWire2);
	connectionLocation getConnectionPort(std::string primaryConnecteeID, std::string secondardConnecteeID);

private:
	void removeExternalConnections(Wire** wire, NonWire* compToBeRemoved);
	std::string getConnectionIDs(NonWire* element, Wire* adjacentWire);
	void connectWireAndElement(Wire* wire, NonWire* element, connectionLocation cl);
	NonWire* locate(std::string id);
};

