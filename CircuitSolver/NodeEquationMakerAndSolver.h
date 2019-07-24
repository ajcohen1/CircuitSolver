#pragma once
#include "CircuitSimplifierAndInfoExtracter.h"
#include <Eigen/Dense>
class NodeEquationMakerAndSolver
{
public:
	NodeEquationMakerAndSolver(CircuitSimplifierAndInfoExtracter csie);
	void makeEquations();
	std::unordered_map<Wire*, double> solve();

private:
	CircuitSimplifierAndInfoExtracter csie;
	Eigen::MatrixXd nodeEqns;
	Eigen::VectorXd nodeEqnsRval;
	std::vector<Wire*> nodeVec;

	void setUpSuperNodeEqns(std::vector<VoltageSource*> allSuperNodes);
	void setUpCurrentSourceEqns(std::vector<CurrentSource*> allCurrentSources);
	void setUpFreeNodeeqns(std::vector<Wire*> allFreeNodes);
	void setUpRefNode(Wire* refNode);
	void setUpStandardNodeEqns(std::vector<Wire*> allStandardNodes);
};

