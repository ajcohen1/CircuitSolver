#include "BoardComponent.h"
#include "BoardComponent.cpp"
#include "potentialElements.h"
#include "CircuitBoard.h"
#include "Node.h"
#include <string>
#include <iostream>
#include "potentialElements.h"
#include "ActiveComponent.h"
#include "Main.h"

using namespace std;

class Main {

	int main() {
	potentialElements::allElements elementName = potentialElements::allElements::capacitor;
	BoardComponent* num1 = new ActiveComponent(nullptr, elementName, 1.0, 1);
	CircuitBoard* board = new CircuitBoard(num1);
	board->printCircuit();
}


};