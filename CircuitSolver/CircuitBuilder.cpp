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
	printCircuitInfoTable();

	std::cout << "\nContinue to Next step to see its solution.\nThat is, the current and voltage going through each component.\n" << std::endl;
	pressOneToContinue();
}

void CircuitBuilder::pressOneToContinue()
{
	std::cout << "\nPress 1 and ENTER to continue." << std::endl;

	unsigned int continueNum = 0;

	std::cin >> continueNum;

	clearScreen();
	if (continueNum == 1) return;
}

void CircuitBuilder::sendWelcomeMessageAndNewCircuitDisclaimer()
{
	std::string welcomeMessage = "Hello, you are using the CircuitSolver made by Abraham Cohen.\n\n";
	std::string newCircuitDisclaimer = "If you desire to load a past circuit, restart the programe and please use the \n\"loadPastCircuit(std::string pastCircuitFileName)\" method";

	std::cout << welcomeMessage << std::endl;
	std::cout << newCircuitDisclaimer << std::endl;

	pressOneToContinue();
}

void CircuitBuilder::determineCircuitCurrentType()
{
	std::string chooseCurrentType = "\nSelect current type of the circuit type of your first current/voltage source.\n";
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

	clearScreen();
}

void CircuitBuilder::setUpElementIDAndValueTable()
{
	elementTable << fort::header << "Type" <<"ID" << "Value";

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
	std::string firstMessage = "You will be loading all the board components that your circuit may use. \n\nThe first component must be either a voltage source or a current source.";
	std::cout << firstMessage << std::endl;

	determineCircuitCurrentType();
	setUpElementIDAndValueTable();

	bool addingNewComps = true;

	while (addingNewComps)
	{	
		clearScreen();
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

	clearScreen();
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

	clearScreen();
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
		elementTable << "Resistor";
		break;
	case capacitor:
		passiveComp = new Capacitor(std::get<id>(ID_Magnitude_andMultiplier), std::get<magitnude>(ID_Magnitude_andMultiplier), std::get<multiplier>(ID_Magnitude_andMultiplier));
		elementTable << "Capacitor";
		break;
	case inductor:	
		passiveComp = new Inductor(std::get<id>(ID_Magnitude_andMultiplier), std::get<magitnude>(ID_Magnitude_andMultiplier), std::get<multiplier>(ID_Magnitude_andMultiplier));
		elementTable << "Inductor";
		break;
	}

	if(currentType == currentTypes::AC) 
		passiveComp->setImpedance(frequency);

	elementTable << std::get<id>(ID_Magnitude_andMultiplier) << std::get<magitnude>(ID_Magnitude_andMultiplier) * pow(10, std::get<multiplier>(ID_Magnitude_andMultiplier)) << fort::endr;
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

	if (isVoltageSource) {
		activeComp = new VoltageSource(id, magnitude, multiplier);
		elementTable << "Voltage Source";
	}
	else
	{
		activeComp = new CurrentSource(id, magnitude, multiplier);
		elementTable << "Current Source";
	}

	elementTable << id << magnitude * pow(10, multiplier) << fort::endr;
	addNewCompToLocatorAndCircuitDraft(activeComp);
}

void CircuitBuilder::loadACActiveComp(ActiveComponent* activeComp, int compType, std::string id, double magnitude, double multiplier)
{
	double phasor = getPhasor();

	bool isVoltageSource = compType == 0;

	if (isVoltageSource) {
		activeComp = new VoltageSource(id, magnitude, multiplier, frequency, phasor);
		elementTable << "Voltage Source";
	}
	else
	{
		activeComp = new CurrentSource(id, magnitude, multiplier, frequency, phasor);
		elementTable << "CurrentSource";
	}


	elementTable << id << magnitude * pow(10, multiplier) << frequency << phasor << fort::endr;
	addNewCompToLocatorAndCircuitDraft(activeComp);

}

void CircuitBuilder::mergeWires(Wire* primaryWire, Wire* wireToBeMerged)
{
	std::list<NonWire*>* primaryWireConnections = &(primaryWire->connections);
	std::list<NonWire*> wireToBeMergedConnections = wireToBeMerged->connections;

	transferWireConnectionsToPrimaryWire(primaryWire, wireToBeMerged);
	connectSecondaryWireElementsToMergedWire(*primaryWireConnections, wireToBeMergedConnections);
}

void CircuitBuilder::transferWireConnectionsToPrimaryWire(Wire* primaryWire, Wire* wireToBeMerged)
{
	std::list<NonWire*> wireToBeMergedConnections = wireToBeMerged->connections;

	for (NonWire* aConnection : wireToBeMergedConnections)
	{
		Wire** portToMergingWire = (aConnection->next == wireToBeMerged) ? &(aConnection->next) : &(aConnection->prev);

		*portToMergingWire = primaryWire;
	}
}

void CircuitBuilder::connectSecondaryWireElementsToMergedWire(std::list<NonWire*>& primaryWire, std::list<NonWire*> wireToBeMerged)
{
	std::list<NonWire*>::iterator mergingWireIterator = wireToBeMerged.begin();

	for (auto mergingConnection = wireToBeMerged.begin(); mergingConnection != wireToBeMerged.end();)
	{
		bool connectionShared = std::find(primaryWire.begin(), primaryWire.end(), *mergingConnection) != primaryWire.end();

		if (connectionShared)
		{
			++mergingConnection;
		}
		else
		{
			primaryWire.push_back(*mergingConnection);
			mergingConnection = wireToBeMerged.erase(mergingConnection);
		}
	}
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
	printConnectionInstructions();
	std::unordered_map <std::string, NonWire*> visitedElements;

	std::vector<NonWire*> nextElements;
	nextElements.push_back(circuitDraft.front());

	connectANonWireAndTheNext(nextElements, visitedElements);
}

void CircuitBuilder::connectANonWireAndTheNext(std::vector<NonWire*>& nextElements, std::unordered_map<std::string, NonWire*> visitedElements)
{
	std::vector<NonWire*> connectingElements;

	if (visitedElements.size() == circuitDraft.size()) return;

	for (NonWire* anElement : nextElements)
	{
		bool compVisited = visitedElements.count(anElement->id) > 0;
		if (compVisited)
			continue;
		visitedElements[anElement->id] = anElement;

		connectingElements = connectAndGetConnectingElements(anElement ->id);
	}

	return connectANonWireAndTheNext(connectingElements, visitedElements);
}

std::vector<NonWire*> CircuitBuilder::connectAndGetConnectingElements(std::string currentElementID)
{
	int continueNum = 1;
	bool stillConnectingElements = true;

	std::vector<NonWire*> connectingElements;
	std::string nextConnectedElementID;

	while (stillConnectingElements)
	{
		printCircuitInfoTable();
		printConnectionsAtNextConnectionPort(locate(currentElementID));
		std::cout << "Type in the ID of a single element you would like to connect "
			<< currentElementID << " to: "<< std::endl;

		std::cin >> nextConnectedElementID;
		
		connectingElements.push_back(locate(nextConnectedElementID));
		connectElementToConnectee(locate(currentElementID), locate(nextConnectedElementID));

		std::cout << "Enter 1 to continue connecting elements to " << currentElementID << std::endl;
		std::cout << "Enter 0 if finished\n" << std::endl;

		std::cin >> continueNum;
		stillConnectingElements = continueNum == 1;

		clearScreen();
	}

	return connectingElements;
}

void CircuitBuilder::connectElementToConnectee(NonWire* currentElement, NonWire* nextConnectedElement)
{
	//four cases
	if (currentElement->next == nullptr && nextConnectedElement->prev == nullptr) {
		Wire* wire = getNewWire();
		connectWireAndElement(wire, currentElement, connectionLocation::next);
		connectWireAndElement(wire, nextConnectedElement, connectionLocation::prev);
	}
	else if (currentElement->next == nullptr)
	{
		Wire* wire = nextConnectedElement->prev;
		connectWireAndElement(wire, currentElement, connectionLocation::next);
	}
	else if (nextConnectedElement->prev == nullptr)
	{
		Wire* wire = currentElement->next;
		connectWireAndElement(wire, nextConnectedElement, connectionLocation::prev);
	}
	else
	{
		mergeWires(currentElement->next, nextConnectedElement->prev);
	}

}


void CircuitBuilder::printAllLoadedElements()
{
	std::cout << "These are all the available circuit elements you have loaded:" << std::endl;

	std::cout << elementTable.to_string() << std::endl;
}

void CircuitBuilder::printConnectionInstructions()
{

	clearScreen();
	std::cout << "\nYou will now connect the elements of the circuit." << std::endl;
	pressOneToContinue();

	std::cout << "Every component you have loaded has a PREV connection port and a NEXT connection port.\n\n\n" << 
	"During the connection process, you will be connecting the NEXT connection port of a single item to the \nPREV connection port of a single or many items.\n\n\n" <<
	"After this step, since every circuit is a loop, every component will have connections in its PREV and NEXT connection ports and the circuit will be properly constructed" << std::endl;

	pressOneToContinue();

	clearScreen();
}

void CircuitBuilder::printConnectionsAtNextConnectionPort(NonWire* nonWire1)
{
	std::cout << "\nCurrent Connections of " << nonWire1->id << " at NEXT" << std::endl;

	fort::table elementConnectionTable;
	elementConnectionTable << fort::header << "ID" << "Next Connection Port" << fort::endr;

	elementConnectionTable << nonWire1->id << getConnectionIDs(nonWire1, nonWire1->next) << fort::endr;
	
	std::cout << elementConnectionTable.to_string() << std::endl;
}

void CircuitBuilder::printCircuitInfoTable()
{
	clearScreen();

	std::cout << "This is the current Circuit you have created. Restart the program if any errors have been detected." << std::endl;

	fort::table circuitInfoTable;

	circuitInfoTable << fort::header << "Type" << "ID" << "Value" << "PREV Connections" << "NEXT Connections" << fort::endr;

	for (NonWire* aCircuitElement : circuitDraft)
	{
		circuitInfoTable <<
			getElementType(aCircuitElement)
			<< aCircuitElement->id
			<< aCircuitElement->magnitude * pow(10, aCircuitElement->multiplier)
			<< getConnectionIDs(aCircuitElement, aCircuitElement->prev)
			<< getConnectionIDs(aCircuitElement, aCircuitElement->next)
			<< fort::endr;
	}

	std::cout << circuitInfoTable.to_string() << std::endl;
}

std::string CircuitBuilder::getElementType(NonWire* anElement)
{
	std::string type;
	std::string unit = anElement->unit;

	if (unit.compare("V") == 0) return "Voltage Source";
	if (unit.compare("A") == 0) return "Current Source";
	if (unit.compare("\u03A9") == 0) return "Resistor";
	if (unit.compare("F") == 0) return "Capacitor";
	if (unit.compare("H") == 0) return "Inductor";

	return "ERROR";
}


void CircuitBuilder::clearScreen()
{
	HANDLE                     hStdOut;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD                      count;
	DWORD                      cellCount;
	COORD                      homeCoords = { 0, 0 };

	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdOut == INVALID_HANDLE_VALUE) return;

	/* Get the number of cells in the current buffer */
	if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) return;
	cellCount = csbi.dwSize.X * csbi.dwSize.Y;

	/* Fill the entire buffer with spaces */
	if (!FillConsoleOutputCharacter(
		hStdOut,
		(TCHAR) ' ',
		cellCount,
		homeCoords,
		&count
	)) return;

	/* Fill the entire buffer with the current colors and attributes */
	if (!FillConsoleOutputAttribute(
		hStdOut,
		csbi.wAttributes,
		cellCount,
		homeCoords,
		&count
	)) return;

	/* Move the cursor home */
	SetConsoleCursorPosition(hStdOut, homeCoords);
}
