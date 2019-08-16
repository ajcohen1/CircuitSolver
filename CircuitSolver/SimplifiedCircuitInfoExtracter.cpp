#include "SimplifiedCircuitInfoExtracter.h"
#include <iostream>
#include "Resistor.h"
#include "Capacitor.h"
#include "Inductor.h"

std::vector<NonWire*> simplifiedCircuit;
Wire* refNode = nullptr;
std::unordered_map<std::string, Wire*> visitedWires;

SimplifiedCircuitInfoExtracter::SimplifiedCircuitInfoExtracter(std::vector<NonWire*> simplifiedCircuit) : simplifiedCircuit(simplifiedCircuit){}

std::tuple<unsigned int, Wire*, std::vector<VoltageSource*>, std::vector<CurrentSource*>, std::vector<Wire*>, std::vector<Wire*>> SimplifiedCircuitInfoExtracter::extract()
{
	findRefNode();
	auto superNodes = findSuperNodes();
	auto currentSources = findCurrentSources();
	auto freeNodes = findFreeNodes();
	auto stdNodes = findStandaradNodes();
	unsigned int wireCnt = findWireCount();

	auto simplifiedCircuitInfo = std::make_tuple(wireCnt, refNode, superNodes, currentSources, freeNodes, stdNodes);
	return simplifiedCircuitInfo;
}

std::vector<NonWire*> SimplifiedCircuitInfoExtracter::getSimplifiedCircuit()
{
	return simplifiedCircuit;
}

Wire* SimplifiedCircuitInfoExtracter::findRefNode()
{
	unsigned long int maxConnectionCount = 0;

	for (NonWire* aCircuitPart : simplifiedCircuit)
	{
		Wire* prevWire = aCircuitPart->prev;
		Wire* nextWire = aCircuitPart->next;

		if (prevWire != nullptr && prevWire->connections.size() > maxConnectionCount)
		{
			maxConnectionCount = prevWire->connections.size();
			refNode = prevWire;
		}
		else if (nextWire != nullptr && nextWire->connections.size() > maxConnectionCount)
		{
			maxConnectionCount = nextWire->connections.size();
			refNode = nextWire;
		}
		else continue;
	}

	if (refNode != nullptr) visitedWires[refNode->id] = refNode;

	return refNode;

}


std::vector<VoltageSource*> SimplifiedCircuitInfoExtracter::findSuperNodes()
{
	std::vector<VoltageSource*> allVoltageSources;

	for (NonWire* aCircuitPart : simplifiedCircuit)
	{
		bool prevWireIsRefNode = aCircuitPart->prev == refNode;
		bool nextWireIsRefNode = aCircuitPart->next == refNode;

		if ((aCircuitPart->unit).compare("V") == 0 && !prevWireIsRefNode && !nextWireIsRefNode)
		{
			allVoltageSources.push_back(static_cast<VoltageSource*>(aCircuitPart));
			visitedWires[aCircuitPart->next->id] = aCircuitPart->next;
			visitedWires[aCircuitPart->prev->id] = aCircuitPart->prev;
		}
	}

	return allVoltageSources;
}

std::vector<CurrentSource*> SimplifiedCircuitInfoExtracter::findCurrentSources()
{
	std::vector<CurrentSource*> allCurrentSources;

	for (NonWire* aCircuitPart : simplifiedCircuit)
	{
		if ((aCircuitPart->unit).compare("A") == 0)
		{
			allCurrentSources.push_back(static_cast<CurrentSource*>(aCircuitPart));
			visitedWires[aCircuitPart->next->id] = aCircuitPart->next;
			visitedWires[aCircuitPart->prev->id] = aCircuitPart->prev;
		}
	}

	return allCurrentSources;
}

std::vector<Wire*> SimplifiedCircuitInfoExtracter::findFreeNodes()
{
	std::vector<Wire*> allFreeNodes;
	std::list<NonWire*> refNodeConnections = refNode->connections;

	//search through all the nonwire elements in the refNode/refWire
	for (NonWire* adjacentElement : refNodeConnections)
	{
		//if there is a voltage source, find the wire that isn't the ref node
		if ((adjacentElement->unit).compare("V") == 0)
		{
			Wire* freeNode = (adjacentElement->prev == refNode) ? adjacentElement->next : adjacentElement->prev;
			allFreeNodes.push_back(freeNode);
			visitedWires[freeNode->id] = freeNode;
		}
	}

	return allFreeNodes;
}


std::vector<Wire*> SimplifiedCircuitInfoExtracter::findStandaradNodes()
{
	std::vector<Wire*> standardNodes;

	for (NonWire* aCircuitPart : simplifiedCircuit)
	{
		Wire* prevWire = aCircuitPart->prev;
		if (visitedWires.count(prevWire->id) == 0)
		{
			standardNodes.push_back(prevWire);
			visitedWires[prevWire->id] = prevWire;
		}
			
		Wire* nextWire = aCircuitPart->next;
		if (visitedWires.count(nextWire->id) == 0)
		{
			standardNodes.push_back(nextWire);
			visitedWires[nextWire->id] = nextWire;
		}
	}

	return standardNodes;
}

unsigned int SimplifiedCircuitInfoExtracter::findWireCount()
{
	return visitedWires.size();
}
