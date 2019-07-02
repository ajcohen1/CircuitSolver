#include "CircuitBoard.h"
#include "CircuitBuilder.h"
#include "ActiveComponent.h"
#include "potentialElements.h"
#include <list>
#include <iostream>

std::list<BoardComponent*> cg;

CircuitBoard::CircuitBoard(CircuitBuilder* cb) {
	cg = cb->getCircuitGraph();
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

int main() {
	std::string id = "";
	BoardComponent* element_1 = new ActiveComponent(potentialElements::allElements::voltageSupply, "V1", 1.0, 1);
	BoardComponent* element_2 = new ActiveComponent(potentialElements::allElements::resistor, "R1", 1.0, 1);
	BoardComponent* element_3 = new ActiveComponent(potentialElements::allElements::capacitor, "C1", 1.0, 1);

	std::string elementArr [] = { "V1", "R1"};

	CircuitBuilder* cb = new CircuitBuilder(element_1);
	cb->connectToSingle("V1", element_2);

	cb->connectToAll(element_3, elementArr, 2);

	CircuitBoard* cBoard = new CircuitBoard(cb);
	cBoard->printBoard();

	return 1;
}

