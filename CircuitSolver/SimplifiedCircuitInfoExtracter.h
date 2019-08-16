#pragma once

#include "CircuitSimplifier.h"
#include "Wire.h"
class SimplifiedCircuitInfoExtracter
{
private: 
	
	template <typename T>
	bool wireContainsBoardComp(const T& element, std::list<T>& listOfElements)
	{
		// Find the iterator if element in list
		auto it = std::find(listOfElements.begin(), listOfElements.end(), element);
		//return if iterator points to end or not. It points to end then it means element
		// does not exists in list
		return it != listOfElements.end();
	}

	std::vector<NonWire*> simplifiedCircuit;
	Wire* refNode = nullptr;
	std::unordered_map<std::string, Wire*> visitedWires;


	Wire* findRefNode();
	std::vector<VoltageSource*> findSuperNodes();
	std::vector<CurrentSource*> findCurrentSources();
	std::vector<Wire*> findFreeNodes();
	std::vector<Wire*> findStandaradNodes();
	unsigned int findWireCount();



public: 

	SimplifiedCircuitInfoExtracter(std::vector<NonWire*> simplifiedCircuit);
	std::tuple<unsigned int, Wire*, std::vector<VoltageSource*>, std::vector<CurrentSource*>, std::vector<Wire*>, std::vector<Wire*>> extract();
	std::vector<NonWire*> getSimplifiedCircuit();
};

