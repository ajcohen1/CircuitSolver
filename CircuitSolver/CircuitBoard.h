#ifndef CIRCUITBOARD_H
#define CIRCUITBOARD_H
class CircuitBoard
{
public:
	CircuitBoard(BoardComponent* firstComponent);
	void printCircuit();

private:
	void addElement(BoardComponent* newComponent);
	void addNode();

};

#endif

