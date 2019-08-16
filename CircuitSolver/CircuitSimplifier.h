#pragma once
#include "CircuitBuilder.h"
#include "VoltageSource.h"
#include "CurrentSource.h"
#include "Wire.h"
class CircuitSimplifier
{
public:
	CircuitBuilder* scb;
	CircuitSimplifier(CircuitBuilder* cb);
	void simplify();
	std::vector<NonWire*> getSimplifiedCircuit();


private:
	std::vector<NonWire*> simplifiedCircuit;
	void removeOpenCircuits();
	std::vector<NonWire*> getAllOpenCircuits();
	std::vector<NonWire*> getInitialOpenCircuits();
	std::vector<NonWire*> getPropogatedOpenCircuits(std::vector<NonWire*> initialShortCircuits);
	void propogateOpenCircuitAffects(std::unordered_map<std::string, NonWire*>& vistitedPropogatedParts, NonWire* &openCircuitPart, std::vector<NonWire*> &propogatedCircuitParts);
	void removeShortCircuits();
	std::vector<NonWire*> getInductors();
	void removeInductorAndMergeWires(NonWire* anInductor);
	void transferWireConnections(Wire* primaryWire, Wire* wireToBeMerged);
	void mergeWireConnections(std::list<NonWire*> &primaryWire, std::list<NonWire*> wireToBeMerged);
	void combineSeriesVoltageSources();
	std::vector<NonWire*> getAllVoltageSources();
	std::tuple<std::vector<NonWire*>, Wire*, Wire*> getVoltageSourceGroupsAndEdges(std::unordered_map<std::string, NonWire*>& visitedVoltageSources, NonWire* &aVoltageSource);
	Wire* traverseDirectionForVoltageSources(std::unordered_map<std::string, NonWire*> &vistitedVoltageSources, std::vector<NonWire*>& voltageGroups, NonWire* aVoltageSource, Wire* wireToBeTraversed);
	VoltageSource* getCombinedVoltageSource(std::tuple<std::vector<NonWire*>, Wire*, Wire*> voltageSourceGroupsAndEdges);
	void addCombinedVoltageSourceToCircuit(NonWire* combinedVoltageSource, Wire*& prevWire, Wire*& nextWire);
	void removeVoltageGroups(std::vector<NonWire*> voltageGroups);
};