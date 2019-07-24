#include "NonWire.h"

NonWire::NonWire(std::string id, double magnitude, double multiplier) : BoardComponent(id) {
	this->magnitude = magnitude;
	this->multiplier = multiplier;
}

NonWire::~NonWire() {

	//remove reference from prev
	next->connections.remove(this);
	prev->connections.remove(this);
}
