#include "BoardComponent.h"
#include "potentialElements.h"
#include "CircuitBoard.h"
#include "Node.h"
#include "ActiveComponent.h"
#include "Main.h"


	int main() {
	potentialElements::allElements elementName = potentialElements::allElements::capacitor;
	BoardComponent* num1 = new ActiveComponent(nullptr, elementName, 1.0, 1);
	CircuitBoard* board = new CircuitBoard(num1);

	elementName = potentialElements::allElements::capacitor;
	BoardComponent* num2 = new ActiveComponent(board->firstComponentPtr, elementName, 1.0, 1);
	board->addElement(num2);

	board->printCircuit();
	std::cout << "yah done good, bucko";
	return 0;
}

