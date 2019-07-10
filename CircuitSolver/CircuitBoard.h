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
	bool vsAdjacent;
	CircuitBoard(CircuitBuilder* cb);
	void printBoard();
	void fillMatrix();
	std::string getNodeEquation(BoardComponent* node);
	std::string getPositiveExpression(BoardComponent* connection, BoardComponent* primaryNode);
	std::string getNegativeExpression(BoardComponent* connection, BoardComponent* primaryNode);


};

