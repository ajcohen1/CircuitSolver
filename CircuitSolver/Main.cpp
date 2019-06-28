#include "BoardComponent.h"
#include "potentialElements.h"
#include "CircuitBoard.h"
#include "Node.h"
#include "ActiveComponent.h"
#include <iostream>



	int main() {
		potentialElements::allElements elementName = potentialElements::allElements::voltageSupply;
	BoardComponent* num1 = new ActiveComponent(nullptr, elementName,1.0, 1);
	CircuitBoard* board = new CircuitBoard(num1, "V1");

	BoardComponent* memAddress = board->getElementLocation("V1");

	std::cout << memAddress << std::endl;
	return 0;
}

