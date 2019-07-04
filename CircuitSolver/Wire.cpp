#include "Wire.h"

Wire::Wire(std::string id) : BoardComponent(id, allCompTypes::WIRE, 0, 0) {
}
