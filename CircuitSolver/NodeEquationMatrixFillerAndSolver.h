#pragma once

#include "SimplifiedCircuitInfoExtracter.h"
#include "Eigen/dense"
#include "Resistor.h"
class NodeEquationMatrixFillerAndSolver
{

public:
	NodeEquationMatrixFillerAndSolver(SimplifiedCircuitInfoExtracter* scie);
	void fillMatrix();
	std::unordered_map < std::string, double> getNodalMatrixSolution();
	unsigned int row = 0;
	Eigen::VectorXd linearComboResultant;
	Eigen::MatrixXd nodeVoltageMatrix;
	Eigen::VectorXd nodeVoltageVec;
	std::vector<NonWire*> getSimplifiedCircuit();
	Wire* refNode = nullptr;

private:
	const unsigned int defualt_size = 1;
	Eigen::MatrixXd nodeVoltageCoefficients;
	SimplifiedCircuitInfoExtracter* scie;
	std::unordered_map<std::string, unsigned int> nodeToMatrixPositionMap;
	std::unordered_map<std::string, Wire*> freeNodeMap;

	//resize matrix to the number of equations
	void setupNodeVoltageMatrixAndLinComboResultant(unsigned int wireCnt);
	
	//for all functions

	//A branch is considered to be the connection between a primary node, the inbetween element, and the node across that (called the secondary node)
	// EX: PrimaryNode ---- NonWire ---- SecondaryNode
	void fillAllBranchEqns(Wire* primaryNode, std::string elementToBeIgnored);
	Wire* getSecondaryNode(Wire* primaryNode, NonWire* adjacentElement);
	void fillSingleBranchEqn(Wire* primaryNode, NonWire* adjacentElement, Wire* secondaryNode);

	void transferBranchToAppropriateEqnMaker(std::string primaryNodeName, NonWire* adjacentElememnt, std::string secondaryNodeName);
	void fillPassiveCompBranchEqn(std::string primaryNodeName, PassiveComponent* passiveComp, std::string secondaryNodeName);
	void fillCurrentSourceBranchEqn(std::string primaryNodeName, CurrentSource* currentSource);

	double getNonWireValue(NonWire* nonWire);
	void addCoefficientToNodeMatrix(std::string nodeName, double coefficient);
	unsigned int getNodeColPositionOnMatrix(std::string nodeName);
	
	//for the super node equations
	void fillSuperNodeAndDeltaVoltageEqns(std::vector<VoltageSource*> allSuperNodes);
	void fillDeltaVoltageEqn(VoltageSource* floatingVoltageSource, std::string prevWireID, std::string nextWireID);
	void fillSuperNodeEqn(Wire* prevWireToFloatingSource, Wire* nextWireToFloatingSource, VoltageSource* floatingVoltageSource);

	//for the free node eqns
	void fillFreeNodeEqns(std::vector<Wire*> allFreeNodes, Wire* refNode);
	VoltageSource* getVoltageSourceConnectedToRefNode(Wire* aFreeNode, Wire* refNode);
	
	void fillCurrentSourceEqns(std::vector<CurrentSource*> allCurrentSources);
	int getCurrentSourceSign(std::string primaryNodeName, std::string currentSourceNextWireID);
	void fillRefNodeEqn(Wire* refNode);
	void fillStandardNodeEqns(std::vector<Wire*> allStandardNodes);


	//solves the Ax = b system
	void solveNodeEqnSystem();
};


