#pragma once
#include "CircuitBuilder.h"
#include <list>
#include <Eigen/Dense>

class CircuitBoard
{
public:
	Eigen::MatrixXd circuitMatrix;
	std::list<BoardComponent*> cg;
	CircuitBuilder* cb;
	CircuitBoard(CircuitBuilder* cb);
	void printBoard();
	void fillMatrix();
	std::string fillRow(BoardComponent* node);
	std::string findSecondNumerator(BoardComponent* connection, BoardComponent* originalConnectee);


};

