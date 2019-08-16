#include "NonWire.h"
#include "Wire.h"

NonWire::NonWire(std::string id, double magnitude, double multiplier, std::string unit) : BoardComponent(id) {
	this->magnitude = magnitude;
	this->multiplier = multiplier;
	this->unit = unit;
	next = nullptr;
	prev = nullptr;
}

NonWire::~NonWire() {

	//remove reference from prev
	next->connections.remove(this);
	prev->connections.remove(this);
}
