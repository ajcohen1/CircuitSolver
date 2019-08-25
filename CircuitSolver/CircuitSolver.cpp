#include "CircuitSolver.h"
#include <iostream>

#include "Inductor.h"
#include "Capacitor.h"
#include "fort.hpp"
#include <Windows.h>
#include <stdio.h>

CircuitSolver::CircuitSolver(std::unordered_map<std::string, double> nodeVoltages, std::vector<NonWire*> simplifiedCircuit) : nodeVoltages(nodeVoltages), simplifiedCircuit(simplifiedCircuit){}

void CircuitSolver::printCircuitSolution()
{
	solveCircuit();

	fort::table circuitSolutionTable;
	circuitSolutionTable << fort::header << "ID" << "Voltage Across (V)" << "Current Across (A)" << fort::endr;

	for (auto anElementID_itsVoltage_itsCurrent : circuitSolution)
	{
		const int elementIDValue = 0, elementVoltageValue = 1, elementCurrentValue = 2;

		std::string elementID = std::get<elementIDValue>(anElementID_itsVoltage_itsCurrent);
		double elementVoltage = std::get<elementVoltageValue>(anElementID_itsVoltage_itsCurrent);
		double elementCurrent = std::get<elementCurrentValue>(anElementID_itsVoltage_itsCurrent);
		
		circuitSolutionTable << elementID << elementVoltage << elementCurrent << fort::endr;
	}

	std::cout << circuitSolutionTable.to_string() << std::endl;
}

void CircuitSolver::solveCircuit()
{
	for (NonWire* anElement : simplifiedCircuit) 
		tranferToAppropriateSolver(anElement);
}

void CircuitSolver::tranferToAppropriateSolver(NonWire* element)
{

	std::string elementUnit = element->unit;

	bool isActiveComp = elementUnit.compare("V") == 0 || elementUnit.compare("A") == 0;

	if (isActiveComp)
	{
		ActiveComponent* activeComp = static_cast<ActiveComponent*>(element);
		solveActiveComp(activeComp);
	}
	else  //if it isnt an active comp, it must be a passive comp
	{
		PassiveComponent* passiveComp = static_cast<PassiveComponent*>(element);
		solvePassiveComp(passiveComp);
	}
}

void CircuitSolver::solvePassiveComp(PassiveComponent* passiveComp)
{
	std::string nextWireID = passiveComp->next->id;
	std::string prevWireID = passiveComp->prev->id;

	double passiveCompVoltage = getElementVoltage(nextWireID, prevWireID);

	//this is just a tester, need to make it usable for all passive comps
	double testImpedance = passiveComp->magnitude * pow(10, passiveComp->multiplier);
	double passiveCompCurrent = passiveCompVoltage / testImpedance;

	addElementID_Voltage_Current_toSolution(passiveComp->id, passiveCompVoltage, passiveCompCurrent);
}

void CircuitSolver::solveActiveComp(ActiveComponent* activeComp)
{
	std::string activeCompUnit = activeComp->unit;

	bool isVoltageSource = activeCompUnit.compare("V") == 0;

	if (isVoltageSource)
	{
		VoltageSource* voltageSource = static_cast<VoltageSource*>(activeComp);
		solveVoltageSource(voltageSource);
	}
	else //if it isnt a voltage source, it must be current source
	{
		CurrentSource* currentSource = static_cast<CurrentSource*>(activeComp);
		solveCurrentSource(currentSource);
	}
}

double CircuitSolver::getElementVoltage(std::string nextWireID, std::string prevWireID)
{
	double nextNodeVoltage = nodeVoltages[nextWireID];
	double prevNodeVoltage = nodeVoltages[prevWireID];

	double deltaVoltage = prevNodeVoltage - nextNodeVoltage;

	return deltaVoltage;
}

void CircuitSolver::addElementID_Voltage_Current_toSolution(std::string elementID, double voltage, double current)
{
	auto elementSolution = std::make_tuple(elementID, voltage, current);
	circuitSolution.push_back(elementSolution);
}

void CircuitSolver::solveVoltageSource(VoltageSource* voltageSource)
{
	double voltageSourceVoltage = voltageSource->magnitude * pow(10, voltageSource->multiplier);
	double voltageSourceCurrent = getCurrentThroughVoltageSource(voltageSource->prev, voltageSource);

	addElementID_Voltage_Current_toSolution(voltageSource->id, voltageSourceVoltage, voltageSourceCurrent);
}

double CircuitSolver::getCurrentThroughVoltageSource(Wire* prevWire, VoltageSource* voltageSource)
{	
	std::list<NonWire*> prevWireConnections = prevWire->connections;

	double voltageSourceCurrent = 0;

	for (NonWire* aConnection : prevWireConnections)
	{

		if (voltageSource == aConnection) continue;
		bool isCurrentSource = aConnection->unit.compare("A") == 0;
		bool isVoltageSource = aConnection->unit.compare("V") == 0;
		bool isPassiveComp = !isCurrentSource && !isVoltageSource;

		if (isVoltageSource)
		{
			VoltageSource* adjacentVoltageSource = static_cast<VoltageSource*>(aConnection);
			voltageSourceCurrent += getCurrentThroughVoltageSource(voltageSource->prev, adjacentVoltageSource);
		}
		else if (isCurrentSource)
		{
			voltageSourceCurrent += aConnection->magnitude * pow(10, aConnection->multiplier);
		}
		else if (isPassiveComp)
		{
			double resistance = aConnection->magnitude * pow(10, aConnection->multiplier);
			double voltageAcrossPassiveComp = getElementVoltage(aConnection->next->id, aConnection->prev->id);
			voltageSourceCurrent += (aConnection->prev == prevWire) ? -(voltageAcrossPassiveComp / resistance) : (voltageAcrossPassiveComp / resistance);
		}
	}

	return voltageSourceCurrent;
}

void CircuitSolver::solveCurrentSource(CurrentSource* currentSource)
{
	std::string nextWireID = currentSource->next->id;
	std::string prevWireID = currentSource->prev->id;

	double currentSourceVoltage = getElementVoltage(nextWireID, prevWireID);
	double currentSourceCurrent = currentSource->magnitude * pow(10, currentSource->multiplier);

	addElementID_Voltage_Current_toSolution(currentSource->id, currentSourceVoltage, currentSourceCurrent);
}


int main()
{
	CircuitBuilder* cb = new CircuitBuilder();
	cb->createNewCircuit();


	CircuitSimplifier* cs = new CircuitSimplifier(cb);
	cs->simplify();

	SimplifiedCircuitInfoExtracter* scif = new SimplifiedCircuitInfoExtracter(cs->getSimplifiedCircuit());
	NodeEquationMatrixFillerAndSolver* nfs = new NodeEquationMatrixFillerAndSolver(scif);
	nfs->fillMatrix();

	CircuitSolver* cSolver = new CircuitSolver(nfs->getNodalMatrixSolution(), nfs->getSimplifiedCircuit());
	cSolver->printCircuitSolution();

}


