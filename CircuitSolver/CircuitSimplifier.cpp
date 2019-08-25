#include "CircuitSimplifier.h"
#include "Wire.h"
#include <tuple>


CircuitBuilder* scb = nullptr;
std::list<NonWire*> simplifiedCircuit;

CircuitSimplifier::CircuitSimplifier(CircuitBuilder* cb): scb(cb), simplifiedCircuit(cb->getCircuit()) {}

void CircuitSimplifier::simplify()
{
	removeOpenCircuits();
	
	if (scb->currentType == CircuitBuilder::currentTypes::DC)
		removeShortCircuits();

	combineSeriesVoltageSources();
}

///////////////////////////////////////////////////
//////   REMOVE OPEN CIRCUITS //////////////////////
/////////////////////////////////////////////////////

void CircuitSimplifier::removeOpenCircuits()
{
	std::vector<NonWire*> allOpenCircuits = getAllOpenCircuits();

	for (NonWire* anOpenCircuit : allOpenCircuits) {
	scb->remove(anOpenCircuit->id);
	}
	
	//update the circuit
	simplifiedCircuit = scb->getCircuit();
}

std::vector<NonWire*> CircuitSimplifier::getAllOpenCircuits()
{
	std::vector<NonWire*> initialOpenCircuits = getInitialOpenCircuits();
	std::vector<NonWire*> propogatedOpenCircuits = getPropogatedOpenCircuits(initialOpenCircuits);

	std::vector<NonWire*> allOpenCircuits = initialOpenCircuits;
	allOpenCircuits.insert(allOpenCircuits.end(), propogatedOpenCircuits.begin(), propogatedOpenCircuits.end());

	return allOpenCircuits;
}

std::vector<NonWire*> CircuitSimplifier::getInitialOpenCircuits()
{

	std::vector<NonWire*> initialOpenCircuits;

	for (NonWire* aCircuitPart : simplifiedCircuit)
	{

		bool circuitIsDC = scb->currentType == CircuitBuilder::currentTypes::DC;
		bool emptyConnectionPort = (aCircuitPart->next == nullptr) || (aCircuitPart->prev == nullptr);

		if (circuitIsDC)
		{

			bool isCapacitor = (aCircuitPart->unit).compare("F") == 0;
			if (isCapacitor || emptyConnectionPort)
				initialOpenCircuits.push_back(aCircuitPart);
		}
		else
		{
			if (emptyConnectionPort)
				initialOpenCircuits.push_back(aCircuitPart);
		}
	}

	return initialOpenCircuits;
}

std::vector<NonWire*> CircuitSimplifier::getPropogatedOpenCircuits(std::vector<NonWire*> initialOpenCircuits)
{

	std::vector<NonWire*> propogatedCircuitParts;

	std::unordered_map<std::string, NonWire*> visitedPropogatedParts;

	for (NonWire* anOpenCircuit : initialOpenCircuits)
	{
		visitedPropogatedParts[anOpenCircuit->id] = anOpenCircuit;

		propogateOpenCircuitAffects(visitedPropogatedParts, anOpenCircuit, initialOpenCircuits);
		
	}

	return propogatedCircuitParts;
}

void CircuitSimplifier::propogateOpenCircuitAffects(std::unordered_map<std::string, NonWire*>& vistitedPropogatedParts, NonWire*& openCircuitPart, std::vector<NonWire*>& propogatedCircuitParts)
{
	Wire* connectedWire = (openCircuitPart->next == nullptr) ? openCircuitPart->prev : openCircuitPart->next;
	std::list<NonWire*> wireConnections = connectedWire->connections;

	if (wireConnections.size() > 2 || vistitedPropogatedParts.count(openCircuitPart->id) != 0)
		return;

	NonWire* propgatedPart = (wireConnections.front() == openCircuitPart) ? wireConnections.back() : wireConnections.front();
	vistitedPropogatedParts[propgatedPart->id] = propgatedPart;
	propogatedCircuitParts.push_back(propgatedPart);

	propogateOpenCircuitAffects(vistitedPropogatedParts, propgatedPart, propogatedCircuitParts);
}

///////////////////////////////////////////////////
//////   REMOVE SHORT CIRCUITS //////////////////////
/////////////////////////////////////////////////////

void CircuitSimplifier::removeShortCircuits()
{
	std::vector<NonWire*> allInductors = getInductors();
	
	for (NonWire* anInductor : allInductors)
		removeInductorAndMergeWires(anInductor);

	//update the circuit
	simplifiedCircuit = scb->getCircuit();
}

std::vector<NonWire*> CircuitSimplifier::getInductors()
{
	std::vector<NonWire*> allInductors;

	bool isInductor;

	for (NonWire* aCircuitPart : simplifiedCircuit)
	{
		isInductor = (aCircuitPart->unit).compare("H") == 0;

		if (isInductor)
			allInductors.push_back(aCircuitPart);
	}

	return allInductors;
}

void CircuitSimplifier::removeInductorAndMergeWires(NonWire* anInductor)
{
	Wire* primaryWire = anInductor->next;
	Wire* wireToBeMerged = anInductor->prev;

	scb->remove(anInductor->id);
	scb->mergeWires(primaryWire, wireToBeMerged);
}

///////////////////////////////////////////////////
//////   COMBINE SERIES VOLTAGE SOURCES ///////////
/////////////////////////////////////////////////////

void CircuitSimplifier::combineSeriesVoltageSources()
{
	std::vector<NonWire*> allVoltageSources = getAllVoltageSources();
	std::unordered_map<std::string, NonWire*> vistitedVoltageSources;

	for (auto aVoltageSource = allVoltageSources.begin(); aVoltageSource != allVoltageSources.end(); )
	{

		//skip voltage source if already visited
		if (vistitedVoltageSources.count((*aVoltageSource)->id) > 0)
		{
			++aVoltageSource;
			continue;
		}

		//if not visited, we are visiting it now. Add it to visited group
		vistitedVoltageSources[(*aVoltageSource)->id] = *aVoltageSource;

		//DEFns: 
		//VoltageSourceGroups - a grouo of voltage sources that are connected in series with each other
		//Edge: the most adjacent wire that is connecgted to either side of the group
		auto voltageSourceGroupsAndEdges = getVoltageSourceGroupsAndEdges(vistitedVoltageSources, *aVoltageSource);

		std::vector<NonWire*> adjacentVoltageGroup = std::get<0>(voltageSourceGroupsAndEdges);
		Wire* prevMostAdjacent = std::get<1>(voltageSourceGroupsAndEdges);
		Wire* nextMostAdjacent = std::get<2>(voltageSourceGroupsAndEdges);

		//if no additional voltage sources are connected to this voltage source in series, then skip to next voltage source
		if (adjacentVoltageGroup.size() < 2)
		{
			++aVoltageSource;
			continue;
		}	

		removeVoltageGroups(adjacentVoltageGroup);

		VoltageSource* combinedVoltageSource = getCombinedVoltageSource(voltageSourceGroupsAndEdges);
		addCombinedVoltageSourceToCircuit(combinedVoltageSource, prevMostAdjacent, nextMostAdjacent);
	}

	//update the circuit
	simplifiedCircuit = scb->getCircuit();
}

std::vector<NonWire*> CircuitSimplifier::getAllVoltageSources()
{
	std::vector<NonWire*> allVoltageSources;

	for (NonWire* aCircuitPart : simplifiedCircuit)
	{
		bool isAVoltageSource = (aCircuitPart->unit).compare("V") == 0;

		if (isAVoltageSource)
			allVoltageSources.push_back(aCircuitPart);
	}

	return allVoltageSources;
}

std::tuple<std::vector<NonWire*>, Wire*, Wire*> CircuitSimplifier::getVoltageSourceGroupsAndEdges(std::unordered_map<std::string, NonWire*>& visitedVoltageSources, NonWire*& aVoltageSource)
{
	std::vector<NonWire*> voltageGroup;
	voltageGroup.push_back(aVoltageSource);

	NonWire* startingElement = aVoltageSource;

	Wire* prevEdge;
	Wire* nextEdge;

	if(aVoltageSource->prev != nullptr) prevEdge = traverseDirectionForVoltageSources(visitedVoltageSources, voltageGroup, aVoltageSource, aVoltageSource->prev);
	if(aVoltageSource->next != nullptr) nextEdge = traverseDirectionForVoltageSources(visitedVoltageSources, voltageGroup, aVoltageSource, aVoltageSource->next);

	auto voltageSourceGroupsAndEdges = std::make_tuple(voltageGroup, prevEdge, nextEdge);

	return voltageSourceGroupsAndEdges;
}

Wire* CircuitSimplifier::traverseDirectionForVoltageSources(std::unordered_map<std::string, NonWire*> &visitedVoltageSources, std::vector<NonWire*>& voltageGroups, NonWire* aVoltageSource, Wire* wireToBeTraversed)
{
	Wire* edge = nullptr;
	std::list<NonWire*> wireConnections = wireToBeTraversed->connections;
	
	bool partsAreInSeries = wireConnections.size() == 2;

	while (partsAreInSeries)
	{
		NonWire* adjacentElement = (wireConnections.front() == aVoltageSource) ? wireConnections.back() : wireConnections.front();

		bool isVS = (adjacentElement->unit).compare("V") == 0;
		bool voltageSourceVisited = visitedVoltageSources.count(adjacentElement->id) != 0;

		if (!isVS || voltageSourceVisited)
			break;

		aVoltageSource= adjacentElement;
		wireToBeTraversed = (aVoltageSource->prev == wireToBeTraversed) ? aVoltageSource->next : aVoltageSource->prev;

		visitedVoltageSources[aVoltageSource->id] = aVoltageSource;
		voltageGroups.push_back(aVoltageSource);
	}

	return wireToBeTraversed;
}

VoltageSource* CircuitSimplifier::getCombinedVoltageSource(std::tuple<std::vector<NonWire*>, Wire*, Wire*> voltageSourceGroupsAndEdges)
{
	double volts = 0;
	std::string id = "Combined VS of (";

	for (NonWire* aVoltageSource : std::get<0>(voltageSourceGroupsAndEdges))
	{
		volts += pow(aVoltageSource->magnitude, aVoltageSource->multiplier);
		id += (aVoltageSource->id) + " " ;
	}

	id += ")";

	double multiplier = floor(log(volts));
	double magnitude = volts / multiplier;

	VoltageSource* combinedVoltageSource = new VoltageSource(id, magnitude, multiplier);
	combinedVoltageSource->prev = std::get<1>(voltageSourceGroupsAndEdges);;
	combinedVoltageSource->next = std::get<2>(voltageSourceGroupsAndEdges);;
	
	return combinedVoltageSource;
}

void CircuitSimplifier::addCombinedVoltageSourceToCircuit(NonWire* combinedVoltageSource, Wire*& prevWire, Wire*& nextWire)
{
	//add combined vs to wires
	prevWire->connections.push_back(combinedVoltageSource);
	nextWire->connections.push_back(combinedVoltageSource);

	//add to circuit and locator
	scb->addNewCompToLocatorAndCircuitDraft(combinedVoltageSource);
}

void CircuitSimplifier::removeVoltageGroups(std::vector<NonWire*> voltageGroups)
{
	for (NonWire* aVoltageSource : voltageGroups)
		scb->remove(aVoltageSource->id);
}


//getting the circuit
std::vector<NonWire*> CircuitSimplifier::getSimplifiedCircuit()
{
	return simplifiedCircuit;
}
