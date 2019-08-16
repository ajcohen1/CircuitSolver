#pragma once
#include "NodeEquationMatrixFillerAndSolver.h"
class CircuitSolver
{
public:
	CircuitSolver(std::unordered_map<std::string, double> nodeVoltages, std::vector<NonWire*> simplifiedCircuit);
	void printCircuitSolution();

private:
	std::vector<NonWire*> simplifiedCircuit;
	std::unordered_map<std::string, double> nodeVoltages;
	std::vector<std::tuple<std::string, double, double>> circuitSolution;

	void solveCircuit();
	void tranferToAppropriateSolver(NonWire* element);

	void solvePassiveComp(PassiveComponent* passiveComp);
	void solveActiveComp(ActiveComponent* activeComp);

	double getElementVoltage(std::string nextWireID, std::string prevWireID);
	void addElementID_Voltage_Current_toSolution(std::string elementID, double voltage, double current);

	void solveVoltageSource(VoltageSource* voltageSource);
	double getCurrentThroughVoltageSource(Wire* prevWire, VoltageSource* voltageSource);


	void solveCurrentSource(CurrentSource* currentSource);
};


