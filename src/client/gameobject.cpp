#include <algorithm>
#include "gameobject.h"

RawGameObject::RawGameObject(Rect rect)
	: rect(rect)
{}

bool RawGameObject::within(MPosition p) const {
	return (rect.p.x <= p.x && p.x < rect.p.x + rect.w &&
			rect.p.y <= p.y && p.y < rect.p.y + rect.h);
}
