#include "CircuitBuilder.h"
#include "PassiveComponent.h"
#include "ActiveComponent.h"
#include "Wire.h"
#include <fort.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>
#include "VoltageSource.h"

enum currentTypes {DC, AC };
currentTypes currentType;
std::vector<NonWire*> circuitDraft;
std::unordered_map<std::string, NonWire*> nonWireMap;
fort::table elementTable;
double frequency = 0.00;
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
	if (cl == connectionLocation::next)
		element->next = wire;
	else element->prev = wire;
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


///////////////////////FOR UI/////////////////////////////////

void CircuitBuilder::createNewCircuit()
{
	sendWelcomeMessageAndNewCircuitDisclaimer();
	loadAllNonWire();
	connectAllNonWire();
}

void CircuitBuilder::sendWelcomeMessageAndNewCircuitDisclaimer()
{
	std::string welcomeMessage = "Hello, you are using the CircuitSolver made by Abraham Cohen";
	std::string newCircuitDisclaimer = "If you desire to load a past circuit, restart the programe and please use the \"loadPastCircuit(std::string pastCircuitFileName) method";

	std::cout << welcomeMessage << std::endl;
	std::cout << newCircuitDisclaimer << std::endl;
}

void CircuitBuilder::determineCircuitCurrentType()
{
	std::string chooseCurrentType = "\nThis version of the CircuitSolver can only support a single current type. \n";
	std::cout << chooseCurrentType << std::endl;

	int chosenCurrentType;
	std::string currentTypeNumbers = "\nEnter 1 for DC \nEnter 2 for AC";
	std::cout << currentTypeNumbers << std::endl;

	std::cin >> chosenCurrentType;

	currentType = currentTypes::DC;
	if (chosenCurrentType == 2)
	{
		currentType = currentTypes::AC;
		determineFrequency();
	}
}

void CircuitBuilder::setUpElementIDAndValueTable()
{
	elementTable << fort::header << "ID" << "Value";

	if (currentType == currentTypes::AC)
		elementTable << "Impedance " << "Phase Shift";

	elementTable << fort::endr;
}

void CircuitBuilder::determineFrequency()
{
	std::string chooseFrequency = "This version of the CircuitSolver can only support a single frequency. \nEnter desired frequency (units used are Hertz)";
	std::cout << chooseFrequency << std::endl;
	std::cin >> frequency;
}

void CircuitBuilder::loadAllNonWire()
{
	std::string firstMessage = "You will be loading all the board components that your circuit may use. \nThe first componentn must be either a voltage source or a current source.";
	std::cout << firstMessage << std::endl;

	determineCircuitCurrentType();
	setUpElementIDAndValueTable();

	bool addingNewComps = true;

	while (addingNewComps)
	{	
		loadSingleNonWire();
		printAllLoadedElements();
		if (allCompsAdded()) return;
	}
}

bool CircuitBuilder::allCompsAdded()
{
	std::string shouldWeContinueAddingComponents = "If adding another component, enter 0 to continue. \nIf finished, enter 1";
	std::cout << shouldWeContinueAddingComponents << std::endl;

	std::string ifShouldContinue = "0";
	std::cin >> ifShouldContinue;

	bool doneAddingComps = ifShouldContinue.compare("1") == 0;

	return doneAddingComps;
}

void CircuitBuilder::loadSingleNonWire()
{
	
	int compType = getNewCompType(); 
	const int voltageSource = 0, currentSource = 1;

	if (circuitDraft.size() == 0 && (compType > 1))
		compType = forceFirstCompToBeAnActiveComp();

	bool isActiveComp = compType == 0 || compType == 1;

	if (isActiveComp)
		loadActiveComp(compType);
	else loadPassiveComp(compType);
}

int CircuitBuilder::getNewCompType()
{
	std::string informUserOfOperation = "\nDetermine the type of element you wish to add to the circuit. \n\n";
	std::string numberForVoltageSource = "Enter 0 for a VoltageSource. \n";
	std::string numberForCurrentSource = "Enter 1 for a CurrentSource. \n";
	std::string numberForResistor = "Enter 2 for a Resistor. \n";
	std::string numberForInductor = "Enter 3 for an Inductor. \n";
	std::string numberForCapacitor = "Enter 4 for a Capacitor.";

	int compType = -1;

	std::cout << informUserOfOperation << numberForVoltageSource << numberForCurrentSource << numberForResistor << numberForInductor << numberForCapacitor << std::endl;
	std::cin >> compType;

	return compType;

}

int CircuitBuilder::forceFirstCompToBeAnActiveComp()
{
	std::string errorMessageToUser = "First componenet must be either a current source or a voltagesource. Please appropriately choose the first component";
	std::cout << errorMessageToUser << std::endl;

	return getNewCompType();
}

void CircuitBuilder::loadPassiveComp(int compType)
{
	unsigned const int resistor = 2, inductor = 3, capacitor = 4, id = 0, magitnude = 1, multiplier = 2;

	auto ID_Magnitude_andMultiplier = getID_Magnitude_andMultiplier();

	PassiveComponent* passiveComp = nullptr;

	switch (compType)
	{
	case resistor:
		passiveComp = new Resistor(std::get<id>(ID_Magnitude_andMultiplier), std::get<magitnude>(ID_Magnitude_andMultiplier), std::get<multiplier>(ID_Magnitude_andMultiplier));
		break;
	case capacitor:
		passiveComp = new Capacitor(std::get<id>(ID_Magnitude_andMultiplier), std::get<magitnude>(ID_Magnitude_andMultiplier), std::get<multiplier>(ID_Magnitude_andMultiplier));
		break;
	case inductor:	
		passiveComp = new Inductor(std::get<id>(ID_Magnitude_andMultiplier), std::get<magitnude>(ID_Magnitude_andMultiplier), std::get<multiplier>(ID_Magnitude_andMultiplier));
		break;
	}

	if(currentType == currentTypes::AC) 
		passiveComp->setImpedance(frequency);

	elementTable << std::get<id>(ID_Magnitude_andMultiplier) << std::get<magitnude>(ID_Magnitude_andMultiplier) * std::get<multiplier>(ID_Magnitude_andMultiplier) << fort::endr;
	addNewCompToLocatorAndCircuitDraft(passiveComp);
}

void CircuitBuilder::loadActiveComp(int compType)
{
	unsigned const int id = 0, magnitude = 1, multiplier = 2;
	auto ID_Magnitude_Multipler = getID_Magnitude_andMultiplier();

	ActiveComponent* activeComp = nullptr;

	if (currentType == currentTypes::DC)
		loadDCActiveComp(activeComp, compType, std::get<id>(ID_Magnitude_Multipler), std::get<magnitude>(ID_Magnitude_Multipler), std::get<multiplier>(ID_Magnitude_Multipler));
	else loadACActiveComp(activeComp, compType, std::get<id>(ID_Magnitude_Multipler), std::get<magnitude>(ID_Magnitude_Multipler), std::get<multiplier>(ID_Magnitude_Multipler));

}

void CircuitBuilder::loadDCActiveComp(ActiveComponent* activeComp, int compType, std::string id, double magnitude, double multiplier)
{
	bool isVoltageSource = compType == 0;

	activeComp = new CurrentSource(id, magnitude, multiplier);
	if (isVoltageSource)
		activeComp = new VoltageSource(id, magnitude, multiplier);	

	elementTable << id << magnitude * pow(10, multiplier) << fort::endr;
	addNewCompToLocatorAndCircuitDraft(activeComp);
}

void CircuitBuilder::loadACActiveComp(ActiveComponent* activeComp, int compType, std::string id, double magnitude, double multiplier)
{
	double phasor = getPhasor();

	bool isVoltageSource = compType == 0;
	activeComp = new CurrentSource(id, magnitude, multiplier, frequency, phasor);

	if (isVoltageSource)
		activeComp = new VoltageSource(id, magnitude, multiplier, frequency, phasor);

	elementTable << id << magnitude * pow(10, multiplier) << frequency << phasor << fort::endr;
	addNewCompToLocatorAndCircuitDraft(activeComp);

}

std::tuple<std::string, double, double> CircuitBuilder::getID_Magnitude_andMultiplier()
{
	std::string chooseID = "\nEnter deisred ID for component";
	std::cout << chooseID << std::endl;
	std::string id;
	std::cin >> id;

	std::string chooseMagnitude = "\nEnter desired magnitude for component (should not be greater than 10 or less than 1. Multiplier chosen in next step)";
	std::cout << chooseMagnitude << std::endl;
	double magnitude;
	std::cin >> magnitude;

	std::string chooseMultiplier = "\nEnter in desired multiplier (multiple of 10)";
	std::cout << chooseMultiplier << std::endl;
	double multipler;
	std::cin >> multipler;

	return std::make_tuple(id, magnitude, multipler);
}

double CircuitBuilder::getPhasor()
{
	double phasor;

	std::string choosePhasor = "\nEnter desired phasor.";
	std::cout << choosePhasor << std::endl;
	std::cin >> phasor;

	return phasor;
}

void CircuitBuilder::connectAllNonWire()
{

	std::cout << "\n\n\n\n---------------------STEP 2---------------------\n\n\n\n";

	printAllLoadedElements();
	printConnectionInstructions();

	for (NonWire* aNonWire : circuitDraft)
	{
		connectANonWire(aNonWire);
	}

	std::cout << "\n FINISHED" << std::endl;

}

void CircuitBuilder::printAllLoadedElements()
{
	std::cout << "\n\n\n\n\nThese are all the available circuit elements you have loaded:" << std::endl;

	std::cout << elementTable.to_string() << std::endl;
}

void CircuitBuilder::printConnectionInstructions()
{
	std::cout << "\nYou will now connect the elements of the circuit.\n" 
		<< "\nEach element has a PREV and a NEXT connection. "
		<< "\nAt PREV or NEXT, an element can have 1 to infinite connections.\n\n\n\n" << std::endl;
}

void CircuitBuilder::connectANonWire(NonWire*& aNonWire)
{
	std::cout << "You will be connecting items to " << aNonWire->id << std::endl;
	std::string compToBeConnectedID;

	std::cout << "Enter ID of component you would like to connect to " << aNonWire->id << std::endl;
	std::cin >> compToBeConnectedID;

	NonWire* compToBeConnected = locate(compToBeConnectedID);

	Wire* wire = getNewWire();

	CircuitBuilder::connectionLocation connectionPortOfSecondaryCompOnPrimaryComp = getConnectionPort(aNonWire->id, compToBeConnectedID);
	connectWireAndElement(wire, aNonWire, connectionPortOfSecondaryCompOnPrimaryComp);

	CircuitBuilder::connectionLocation connectionPortOfPrimaryCompOnSecondaryComp = getConnectionPort(compToBeConnectedID, aNonWire->id);
	connectWireAndElement(wire, compToBeConnected, connectionPortOfPrimaryCompOnSecondaryComp);
}

void CircuitBuilder::printElementPairConnections(NonWire* nonWire1, NonWire* nonWire2)
{
	std::cout << "\nCurrent Connections of " << nonWire1->id << " and " << nonWire2->id << std::endl;

	fort::table elementConnectionTable;
	elementConnectionTable << fort::header << "ID" << "Previous Connection Port" << "Next Connection Port" << fort::endr;

	elementConnectionTable << nonWire1->id << getConnectionIDs(nonWire1, nonWire1->prev) << getConnectionIDs(nonWire1, nonWire1->next) << fort::endr;
	elementConnectionTable << nonWire2->id << getConnectionIDs(nonWire2, nonWire2->prev) << getConnectionIDs(nonWire2, nonWire2->next) << fort::endr;
	
	std::cout << elementConnectionTable.to_string() << std::endl;
}

CircuitBuilder::connectionLocation CircuitBuilder::getConnectionPort(std::string primaryConnecteeID, std::string secondardConnecteeID)
{
	printElementPairConnections(locate(secondardConnecteeID), locate(primaryConnecteeID));

	std::cout << "Which connection port on " << primaryConnecteeID << " would you like to connect " << secondardConnecteeID << " to?" << std::endl;

	std::cout << "Enter 1 to connect " << secondardConnecteeID << " to " << primaryConnecteeID << " NEXT connection port" << std::endl;
	std::cout << "Enter 2 to connect " << secondardConnecteeID << " to " << primaryConnecteeID << " PREVIOUS connection port" << std::endl;

	int connectionPortNum;

	std::cin >> connectionPortNum;

	if (connectionPortNum == 1)
		return CircuitBuilder::connectionLocation::next;
	return CircuitBuilder::connectionLocation::prev;
}



