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
	for (auto anElementID_itsVoltage_itsCurrent : circuitSolution)
	{
		const int elementIDValue = 0, elementVoltageValue = 1, elementCurrentValue = 2;

		std::string elementID = std::get<elementIDValue>(anElementID_itsVoltage_itsCurrent);
		double elementVoltage = std::get<elementVoltageValue>(anElementID_itsVoltage_itsCurrent);
		double elementCurrent = std::get<elementCurrentValue>(anElementID_itsVoltage_itsCurrent);

		std::cout << elementID << ": "<< "\n" <<
			"   Voltage: " << elementVoltage << "V, Current: " << elementCurrent << " A."
			<< std::endl;
	}
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
		bool isCurrentSource = aConnection->unit.compare("A");
		bool isVoltageSource = aConnection->unit.compare("V");
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
			voltageSourceCurrent += voltageAcrossPassiveComp / resistance;
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
	VoltageSource* vs = new VoltageSource("V1", 5.0, 1.0);
	VoltageSource* v2 = new VoltageSource("V2", 5.0, 1.0);
	VoltageSource* v3 = new VoltageSource("V3", 5.0, 1.0);
	CurrentSource* cs1 = new CurrentSource("CS1", 5, 1);
	Resistor* r = new Resistor("R1", 1.0, 2);
	Resistor* r2 = new Resistor("R2", 1.0, 2);
	Inductor* i = new Inductor("I1", 1, 2);
	Capacitor* c = new Capacitor("C1", 1, 2);

	//VS

	CircuitBuilder* cb = new CircuitBuilder(vs);

	//LR VS CIRCUIT 1 SERIES
	//cb->connectToSingle(i, "V1", nullptr);
	//cb->connectToSingle(r, "I1", nullptr);
	//cb->addWireBetween("R1", "V1");

	//LR VS Circuit PARRALEL
	//cb->connectToSingle(i, "V1", nullptr);
	//cb->connectToSingle(r, "V1", vs->next);
	//std::string comps[] = { "R1", "V1", "I1" };
	//cb->addWireBetweenAll(comps);

	//LRC VS Circuit PARRALEL with CI series
	//cb->connectToSingle(c, "V1", nullptr);
	//cb->connectToSingle(r, "V1", vs->next);
	//cb->connectToSingle(i, "C1", c->next);
	//std::string comps[] = { "R1", "V1", "I1" };
	//cb->addWireBetweenAll(comps);

	//Series VS circuit (only vs)
	//cb->connectToSingle(v2, "V1", nullptr);
	//cb->connectToSingle(v3, "V2", nullptr);
	//cb->connectWireAndElement(v2->next, vs, (vs->prev));

	//Series Combo circuit
	//cb->connectToSingle(cs1, "V1", nullptr);
	//cb->connectToSingle(r, "CS1", nullptr);
	//cb->connectToSingle(v2, "R1", nullptr);
	//cb->connectToSingle(i, "V2", nullptr);
	//cb->connectToSingle(r2, "I1", nullptr);
	//Wire* wire = cb->getNewWire();
	//cb->connectWireAndElement(wire, vs, vs->prev);
	//cb->connectWireAndElement(wire, r2, r2->next);

	//CircuitSimplifier* cs = new CircuitSimplifier(cb);
	//cs->simplify();
	//SimplifiedCircuitInfoExtracter* scif = new SimplifiedCircuitInfoExtracter(cs->getSimplifiedCircuit());
	//NodeEquationMatrixFillerAndSolver* nfs = new NodeEquationMatrixFillerAndSolver(scif);
	//nfs->fillMatrix();

	//CircuitSolver* cSolver = new CircuitSolver(nfs->getNodalMatrixSolution(), nfs->getSimplifiedCircuit());
	//cSolver->printCircuitSolution();

	 cb->createNewCircuit();

}


