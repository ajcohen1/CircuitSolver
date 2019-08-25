#include "NodeEquationMatrixFillerAndSolver.h"
#include <iostream>
#include "Resistor.h"
const unsigned int defualt_size = 1;
Eigen::MatrixXd nodeVoltageMatrix(defualt_size, defualt_size);
Eigen::VectorXd linearComboResultant(defualt_size);
Eigen::VectorXd nodeVoltageVec(defualt_size);
SimplifiedCircuitInfoExtracter* scie;
std::unordered_map<std::string, unsigned int> nodeToMatrixPositionMap;
std::unordered_map<std::string, Wire*> freeNodeMap;
unsigned int row = 0;

NodeEquationMatrixFillerAndSolver::NodeEquationMatrixFillerAndSolver(SimplifiedCircuitInfoExtracter* scie) : scie(scie) {}

void NodeEquationMatrixFillerAndSolver::fillMatrix()
{
	auto circuitInfo = scie->extract();
	const int wireCnt = 0, refNode = 1, superNodes = 2, currentSources = 3, freeNodes = 4, stdNodes = 5;

	setupNodeVoltageMatrixAndLinComboResultant(std::get<wireCnt>(circuitInfo));

	fillRefNodeEqn(std::get<refNode>(circuitInfo));
	row++;
	fillSuperNodeAndDeltaVoltageEqns(std::get<superNodes>(circuitInfo));
	fillFreeNodeEqns(std::get<freeNodes>(circuitInfo), std::get<refNode>(circuitInfo));
//	fillCurrentSourceEqns(std::get<currentSources>(circuitInfo));
	fillStandardNodeEqns(std::get<stdNodes>(circuitInfo));

	//std::cout << nodeVoltageMatrix << "\n" << std::endl;
	//std::cout << linearComboResultant << "\n" << std::endl;
}

std::unordered_map < std::string, double> NodeEquationMatrixFillerAndSolver::getNodalMatrixSolution()
{
	std::unordered_map < std::string, double> nodeMatrixSolution;

	solveNodeEqnSystem();

	for (auto const& aNodeNameAndMatrixPosition : nodeToMatrixPositionMap)
	{
		int nodeSlnVecIndex = aNodeNameAndMatrixPosition.second;

		double strangeZero = pow(2.77542, -16);
		double nodeVoltage = nodeVoltageVec(nodeSlnVecIndex);

		bool isStrangeZero = (abs(strangeZero - nodeVoltage) < 0.01);
		nodeVoltage = (isStrangeZero) ? 0.00 : nodeVoltage;

		std::string nodeName = aNodeNameAndMatrixPosition.first;

	//	std::cout << nodeName << " : " << nodeVoltage << std::endl;
		
		nodeMatrixSolution[nodeName] = nodeVoltage;
	}

	return nodeMatrixSolution;
}

std::vector<NonWire*> NodeEquationMatrixFillerAndSolver::getSimplifiedCircuit()
{
	return scie->getSimplifiedCircuit();
}



void NodeEquationMatrixFillerAndSolver::setupNodeVoltageMatrixAndLinComboResultant(unsigned int wireCnt)
{
	nodeVoltageMatrix = Eigen::MatrixXd::Zero(wireCnt, wireCnt);
	linearComboResultant = Eigen::VectorXd::Zero(wireCnt);
	nodeVoltageVec = Eigen::VectorXd::Zero(wireCnt);
}

void NodeEquationMatrixFillerAndSolver::fillRefNodeEqn(Wire* refNode)
{

	this->refNode = refNode;

	//fill linearComboResultant
	const double refNodeVoltage = 0;
	linearComboResultant(row) = refNodeVoltage;

	//add to matrix at proper column num
	//note, since this is a ref node, it will have no coefficients
	const double coefficient = 1;
	unsigned int column = getNodeColPositionOnMatrix(refNode->id);
	nodeVoltageMatrix(row, column) = coefficient;
}


//A branch is considered to be the connection between a primary node, the inbetween element, and the node across that (called the secondary node)
	// EX: PrimaryNode ---- NonWire ---- SecondaryNode
void NodeEquationMatrixFillerAndSolver::fillAllBranchEqns(Wire* primaryNode, std::string elementToBeIgnored)
{
	std::list<NonWire*> nodeConnections = primaryNode->connections;
	for (NonWire* aNonWireConnection : nodeConnections)
	{
		bool isVoltageSource = aNonWireConnection->unit.compare("V") == 0;
		if (isVoltageSource) 
			continue;  //this is handled by supernode code

		Wire* secondaryNode = getSecondaryNode(primaryNode, aNonWireConnection);
		fillSingleBranchEqn(primaryNode, aNonWireConnection, secondaryNode);
	}

}

Wire* NodeEquationMatrixFillerAndSolver::getSecondaryNode(Wire* primaryNode, NonWire* adjacentElement)
{
	return (adjacentElement->next == primaryNode) ? adjacentElement->prev : adjacentElement->next;
}

void NodeEquationMatrixFillerAndSolver::fillSingleBranchEqn(Wire* primaryNode, NonWire* adjacentElement, Wire* secondaryNode)
{
	std::string primaryNodeName = primaryNode->id;
	std::string secondaryNodeName = secondaryNode->id;
	transferBranchToAppropriateEqnMaker(primaryNodeName, adjacentElement, secondaryNodeName);
}

void NodeEquationMatrixFillerAndSolver::transferBranchToAppropriateEqnMaker(std::string primaryNodeName, NonWire* adjacentElememnt, std::string secondaryNodeName)
{
	std::string adjacentElemementUnit = adjacentElememnt->unit;
	
	bool adjacentElementIsPassiveComp = adjacentElemementUnit.compare("\u03A9") == 0 || adjacentElemementUnit.compare("H")==0 || adjacentElemementUnit.compare("F") == 0;
	bool adjacentElementIsCurrentSource = adjacentElemementUnit.compare("A") == 0;

	if (adjacentElementIsPassiveComp)
	{
		PassiveComponent* passiveComp = static_cast<PassiveComponent*>(adjacentElememnt);
		fillPassiveCompBranchEqn(primaryNodeName, passiveComp, secondaryNodeName);
	}
	else if (adjacentElementIsCurrentSource)
	{
		CurrentSource* currentSource = static_cast<CurrentSource*>(adjacentElememnt);
		fillCurrentSourceBranchEqn(primaryNodeName, currentSource);
	}
	else return;
}

void NodeEquationMatrixFillerAndSolver::fillPassiveCompBranchEqn(std::string primaryNodeName, PassiveComponent* passiveComp, std::string secondaryNodeName)
{
	double primaryNodeCoefficient = 1 / (getNonWireValue(passiveComp));
	addCoefficientToNodeMatrix(primaryNodeName, primaryNodeCoefficient);

	double secondaryNodeCoefficient = -primaryNodeCoefficient;
	addCoefficientToNodeMatrix(secondaryNodeName, secondaryNodeCoefficient);
}

void NodeEquationMatrixFillerAndSolver::fillCurrentSourceBranchEqn(std::string primaryNodeName, CurrentSource* currentSource)
{
	const double coefficient = 1;
	addCoefficientToNodeMatrix(primaryNodeName, coefficient);

	//add currentSource val to linCombo vector
	double currentCoefficient = getNonWireValue(currentSource) * getCurrentSourceSign(primaryNodeName, currentSource->next->id);
	linearComboResultant(row) += (-1) * currentCoefficient;
}


int NodeEquationMatrixFillerAndSolver::getCurrentSourceSign(std::string primaryNodeName, std::string currentSourceNextWireID)
{
	if (primaryNodeName.compare(currentSourceNextWireID) == 0)
		return -1;
	return 1;
}


double NodeEquationMatrixFillerAndSolver::getNonWireValue(NonWire* nonwire) {
	return nonwire->magnitude* pow(10, nonwire->multiplier);
}

void NodeEquationMatrixFillerAndSolver::addCoefficientToNodeMatrix(std::string nodeName, double coefficient)
{
	unsigned int colNumber = getNodeColPositionOnMatrix(nodeName);
	nodeVoltageMatrix(row, colNumber) = coefficient;
}

void NodeEquationMatrixFillerAndSolver::fillSuperNodeAndDeltaVoltageEqns(std::vector<VoltageSource*> allSuperNodes)
{
	/*With every super node, there are 2 equations. 
	1: The equation relating the two nodes and the voltage source, where the difference of the two node voltages must equal the voltage source
	This will be called the deltaVoltageEqn.

	2: The super node equation, where the current going in BOTH nodes will equal the current going out BOTH nodes
	This will be called the superNodeEqn*/

	for (VoltageSource* aFloatingVoltageSource : allSuperNodes)
	{
		Wire* prevWire = aFloatingVoltageSource->prev;
		Wire* nextWire = aFloatingVoltageSource->next;

		fillDeltaVoltageEqn(aFloatingVoltageSource, prevWire->id, nextWire->id);
		row++;
		fillSuperNodeEqn(prevWire, nextWire, aFloatingVoltageSource);
		row++;
	}
}

void NodeEquationMatrixFillerAndSolver::fillSuperNodeEqn(Wire* prevWireToFloatingSource, Wire* nextWireToFloatingSource, VoltageSource* floatingVoltageSource)
{
	fillAllBranchEqns(prevWireToFloatingSource, "VoltageSource");
	fillAllBranchEqns(nextWireToFloatingSource, "VoltageSource");
}

void NodeEquationMatrixFillerAndSolver::fillDeltaVoltageEqn(VoltageSource* floatingVoltageSource, std::string prevWireID, std::string nextWireID)
{
	//add the voltagesource magnituce to the linearComboResultant
	double voltageSourceValue = getNonWireValue(floatingVoltageSource);
	linearComboResultant(row) = voltageSourceValue;

	//add the nodal coefficients to the matrix
	//NOTE: the relationship between the node voltages and the voltage source is 
	//that the nextNodeVoltage - prevNodeVoltage = voltageSourceValue

	const double prevWireCoefficient = -1;
	unsigned int prevWireColNumber = getNodeColPositionOnMatrix(prevWireID);
	nodeVoltageMatrix(row, prevWireColNumber) = prevWireCoefficient;

	const double nextWireCoefficient = 1;
	unsigned int nextWireColNumber = getNodeColPositionOnMatrix(nextWireID);
	nodeVoltageMatrix(row, nextWireColNumber) = nextWireCoefficient;
	
}

void NodeEquationMatrixFillerAndSolver::fillCurrentSourceEqns(std::vector<CurrentSource*> allCurrentSources)
{
	for (CurrentSource* aCurrentSource : allCurrentSources)
	{
		Wire* prevWire = aCurrentSource->prev;
		if (freeNodeMap.count(prevWire->id) == 0 && prevWire != refNode) {
			fillAllBranchEqns(prevWire, "");
			row++;
		}

		Wire* nextWire = aCurrentSource->next;
		if (freeNodeMap.count(nextWire->id) == 0 && nextWire != refNode) {
			fillAllBranchEqns(nextWire, "");
			row++;
		}
	}
}

void NodeEquationMatrixFillerAndSolver::fillFreeNodeEqns(std::vector<Wire*> allFreeNodes, Wire* refNode)
{
	for (Wire* aFreeNode : allFreeNodes)
	{
		freeNodeMap[aFreeNode->id] = aFreeNode;

		VoltageSource* voltageSourceConnectedToRefNode = getVoltageSourceConnectedToRefNode(aFreeNode, refNode);
		linearComboResultant(row) = getNonWireValue(voltageSourceConnectedToRefNode);
		addCoefficientToNodeMatrix(aFreeNode->id, 1);
		
		row++;
	}
}

VoltageSource* NodeEquationMatrixFillerAndSolver::getVoltageSourceConnectedToRefNode(Wire* aFreeNode, Wire* refNode)
{
	auto freeNodeConnections = aFreeNode->connections;
	for (NonWire* aConnection : freeNodeConnections)
	{
		if (aConnection->unit.compare("V") == 0)
			if (aConnection->next == refNode || aConnection->prev == refNode)
			{
				VoltageSource* voltageSource = static_cast<VoltageSource*>(aConnection);
				return voltageSource;
			}
			else return nullptr;
	}
}

void NodeEquationMatrixFillerAndSolver::fillStandardNodeEqns(std::vector<Wire*> allStandardNodes)
{
	for (auto node : allStandardNodes)
	{
		fillAllBranchEqns(node, "");
		row++;
	}
}

void NodeEquationMatrixFillerAndSolver::solveNodeEqnSystem()
{
	nodeVoltageVec = nodeVoltageMatrix.householderQr().solve(linearComboResultant);
}

unsigned int NodeEquationMatrixFillerAndSolver::getNodeColPositionOnMatrix(std::string nodeName)
{
	bool nodeAbsentFromMap = nodeToMatrixPositionMap.count(nodeName) < 1;
	if (nodeAbsentFromMap)
		nodeToMatrixPositionMap[nodeName] = nodeToMatrixPositionMap.size();

	return nodeToMatrixPositionMap[nodeName];
}