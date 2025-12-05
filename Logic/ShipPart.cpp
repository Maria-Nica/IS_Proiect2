#include "ShipPart.h"

ShipPart::ShipPart(Position position, bool head) : m_position(position), m_hit(false), m_isHead(head) {}

void ShipPart::markHit() {
	m_hit = true; 
}

bool ShipPart::isHit() const 
{
	return m_hit; 
}

bool ShipPart::isHeadPart() const 
{ 
	return m_isHead;
}

Position ShipPart::getPosition() const 
{ 
	return m_position;
}