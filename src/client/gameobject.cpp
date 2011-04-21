#include <algorithm>
#include "gameobject.h"

GameObject::GameObject(Rect rect)
	: rect(rect)
{}

bool GameObject::within(MPosition p) const {
	return (rect.p.x <= p.x && p.x < rect.p.x + rect.w &&
			rect.p.y <= p.y && p.y < rect.p.y + rect.h);
}
