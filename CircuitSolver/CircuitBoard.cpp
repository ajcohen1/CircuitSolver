#include "CircuitBoard.h"
#include "CircuitBuilder.h"
#include <list>
#include <iostream>
#include <Eigen/Dense>
#include "BoardComponent.h"
#include "PassiveComponent.h"
#include "PowerSupply.h"
#include "Wire.h"

std::list<BoardComponent*> cg;
CircuitBuilder* cb;
Eigen::MatrixXd circuitMatrix;
bool vsAdjacent = false;

std::string eqnsTstString = "";

CircuitBoard::CircuitBoard(CircuitBuilder* cb) {
	cg = cb->getCircuitGraph();
	this->cb = cb;
}

void CircuitBoard::printBoard() {

	for (BoardComponent* vertex : cg) {
		std::cout << vertex->id << " : ";

		for (BoardComponent* connection : vertex->connections) {
			std::cout << connection->id << ", ";
		}
		std::cout << "\n";
	}

}


//this will fill the matrix with the node-voltage equations
void CircuitBoard::fillMatrix()
{
	//get all the nodes all the circuit
	std::vector<BoardComponent*> allNodes = cb->getNodes();

	//initialize the matrix with correct dimesnions

	//ARE NOT FILLING MATRIX YET

	// int numEqns = allNodes.size()-1;
	// circuitMatrix(numEqns, numEqns);

	//fill all rows of matrix

	//works only if reference node is last node to be added to circuit
	for (auto node = allNodes.begin(), lastNode = --allNodes.end(); node != lastNode; ++node) {
		eqnsTstString += getNodeEquation(*node);
		eqnsTstString += "\n";
	}
}

std::string CircuitBoard::getNodeEquation(BoardComponent* primaryNode) {

	auto surroudingBridges = primaryNode->connections;
	std::string equation = "";

	std::string positiveExpression;
	std::string negativeExpression;
	for (BoardComponent* bridge : surroudingBridges) {

		positiveExpression = getPositiveExpression(bridge, primaryNode);

		equation += positiveExpression;

		negativeExpression = getNegativeExpression(bridge, primaryNode);
		if (negativeExpression != "") {
			equation += " - " + negativeExpression;
		}
		

		if (bridge != *(--surroudingBridges.end()))
			equation += " + ";
	}

	equation += " = 0";

	return equation;
}


std::string CircuitBoard::getPositiveExpression(BoardComponent* bridge, BoardComponent* primaryNode)
{
	std::string expression = "";

	BoardComponent* refNode = *(--primaryNode->connections.end());

	//if bridge is a current source
	bool bridgeCurrentSource = (bridge->compType == BoardComponent::AC_Current) || (bridge->compType == BoardComponent::DC_Current);
	if (bridgeCurrentSource) {
		expression = bridge->id;
		return expression;
	}
	//end 

	//if bridge is a voltage source
	bool bridgeVoltageSource = (bridge->compType == BoardComponent::AC_Voltage) || (bridge->compType == BoardComponent::DC_Voltage);
	//TODO

	//end

	std::string denominator = bridge->id;
	std::string numerator = primaryNode->id;

	expression = "(1/" + denominator + ")*" + numerator;

	return expression;
}


std::string CircuitBoard::getNegativeExpression(BoardComponent* bridge, BoardComponent* primaryNode) {

	std::string expression = "";
	BoardComponent* refNode = *(--cb->getNodes().end());

	//if bridge is a power source
	bool bridgeCurrentSource = (bridge->compType == BoardComponent::AC_Current) || (bridge->compType == BoardComponent::DC_Current) || (bridge->compType == BoardComponent::AC_Voltage) || (bridge->compType == BoardComponent::DC_Voltage);
	if (bridgeCurrentSource) {
		return "";
	}

	auto allSecondaryConnections = bridge->connections;
	
	std::string numerator = "";
	std::string denominator = "";

	//note: this loop should only ever loop twice, as each active component should only have 2 connections
	for (BoardComponent* aSecondaryConnection : allSecondaryConnections) {
		
		//to ensure we arent traveling back to original node
		if (aSecondaryConnection != primaryNode) {

			if (aSecondaryConnection == refNode) {
				numerator = "0";
			}
			else numerator = aSecondaryConnection->id;
			denominator = bridge->id;
		}

	}

	expression = "(1/" + denominator + ")*" + numerator;

	return expression;
}


int main() {
		std::string id = "";
		BoardComponent* e1 = new PowerSupply("VS1", PowerSupply::allCompTypes::DC_Voltage, 2.0, 1.0);
		BoardComponent* e2 = new Wire("N1");
		BoardComponent* e3 = new PassiveComponent("R1", PassiveComponent::allCompTypes::RESISTOR, 5.0, 1.0);
		BoardComponent* e4 = new Wire("N2");
		BoardComponent* e5 = new PassiveComponent("R2", PassiveComponent::allCompTypes::RESISTOR, 5.0, 1.0);
		BoardComponent* e6 = new PassiveComponent("R3", PassiveComponent::allCompTypes::RESISTOR, 10.0, 1.0);
		BoardComponent* e7 = new Wire("N3");
		BoardComponent* e8 = new PassiveComponent("R4", PassiveComponent::allCompTypes::RESISTOR, 10.0, 1.0);
		BoardComponent* e9 = new Wire("N4");

		CircuitBuilder* cb = new CircuitBuilder(e1);
		cb->connectToSingle("VS1", e2);
		cb->connectToSingle("N1", e3);
		cb->connectToSingle("R1", e4);
		cb->connectToSingle("N2", e5);
		cb->connectToSingle("N2", e6);
		cb->connectToSingle("R2", e7);
		cb->connectToSingle("N3", e8);

		std::string eArr[] = { "VS1", "R3", "R4" };

		cb->connectToAll(e9, eArr, 3);
		CircuitBoard* cBoard = new CircuitBoard(cb);
		cBoard->fillMatrix();
		std::cout << eqnsTstString << std::endl;
		cBoard->printBoard();
		
		return 1;
}

