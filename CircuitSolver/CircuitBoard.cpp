#include "CircuitBoard.h"
#include "CircuitBuilder.h"
#include <list>
#include <iostream>
#include <Eigen/Dense>
#include "PassiveComponent.h"
#include "PowerSupply.h"
#include "Wire.h"

std::list<BoardComponent*> cg;
CircuitBuilder* cb;
Eigen::MatrixXd circuitMatrix;

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

	// int numEqns = allNodes.size();
	// circuitMatrix(numEqns, numEqns);

	//fill all rows of matrix
	for (BoardComponent* node : allNodes) {
		eqnsTstString += fillRow(node);
		eqnsTstString += "\n";
	}

}

std::string CircuitBoard::fillRow(BoardComponent* node) {

	auto allConnections = node->connections;
	std::string row = "";

	std::string numerator1;
	std::string numerator2;
	for (BoardComponent* connection : allConnections) {
		numerator1 = "(" + node->id + "/" + connection->id + ") ";
		numerator2 = findSecondNumerator(connection, node);

		row += numerator1 + " - " + numerator2 + " + ";
	}

	return row;
}

std::string CircuitBoard::findSecondNumerator(BoardComponent* connection, BoardComponent* originalConnectee) {

	std::string numerator2 = "(";
	auto allSecondaryConnections = connection->connections;

	for (BoardComponent* aSecondaryConnection : allSecondaryConnections) {
		if (aSecondaryConnection != originalConnectee) {
			numerator2 += aSecondaryConnection->id + "/" + connection->id;

		}
	}
	
	numerator2 += ")";
	return numerator2;
}


int main() {
		std::string id = "";
		BoardComponent* e1 = new PowerSupply("V1", PowerSupply::allCompTypes::DC_Voltage, 2.0, 1.0);
		BoardComponent* e2 = new Wire("N1");
		BoardComponent* e3 = new PassiveComponent("R1", PassiveComponent::allCompTypes::RESISTOR, 5.0, 3.0);
		BoardComponent* e4 = new Wire("N2");
		CircuitBuilder* cb = new CircuitBuilder(e1);
		cb->connectToSingle("V1", e2);
		cb->connectToSingle("N1", e3);
		std::string eArr[] = {"V1", "R1"};
		cb->connectToAll(e4, eArr, 2);
		CircuitBoard* cBoard = new CircuitBoard(cb);
		cBoard->fillMatrix();
		std::cout << eqnsTstString << std::endl;
		cBoard->printBoard();
		
		return 1;
}

