#pragma once
#include "CircuitBuilder.h"
#include "VoltageSource.h"
#include "CurrentSource.h"
class CircuitSimplifierAndInfoExtracter
{
public:
	CircuitSimplifierAndInfoExtracter(CircuitBuilder cb);
	void simplify();
	std::vector<NonWire*> getSimplifiedCircuit();
	std::tuple<std::vector<VoltageSource*>, std::vector<CurrentSource*>, std::vector<Wire*>, std::vector<Wire*>, Wire*> extractCircuitInfo();

private:
	CircuitBuilder cb;
	std::vector<NonWire*> simplifiedCircuit;
	Wire* refNode;
	std::vector<VoltageSource*> allVoltageSources;
	std::vector<CurrentSource*> allCurrentSources;
	std::vector<Wire*> allFreeNodes;
	std::vector<Wire*> allStandardNodes;
	Wire* refNode;

	void removeOpenCircuits();
	void combineShortCircuits();
	void combineVSPairs();
	bool confirmCurrentFlows();

	void findRefNode();
	void findSuperNodes();
	void findCurrentSources();
	void findFreeNodes();
};

