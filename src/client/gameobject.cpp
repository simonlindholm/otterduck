#include <algorithm>
#include "gameobject.h"

bool GameObject::use(GameState& gs, Player& pl) {
	return obj->use(gs, pl);
}
void GameObject::draw(const GameState& gs, UI& ui) const {
	return obj->draw(gs, ui);
}
void GameObject::step(GameState& gs, unsigned int delay) {
	return obj->step(gs, delay);
}
bool GameObject::within(MPosition p) const {
	return obj->within(p);
}

GameObject::GameObject(const GameObject& other) {
	RawGameObject* o = other.obj->clone();
	if (o) obj = std::shared_ptr<RawGameObject>(o);
	else obj = other.obj;
}

GameObject::GameObject(GameObject&& other)
	: obj(std::move(other.obj))
{}

void GameObject::swap(GameObject& other) {
	obj.swap(other.obj);
}

GameObject::GameObject(RawGameObject* raw)
	: obj(raw)
{}

GameObject& GameObject::operator=(const GameObject& other) {
	GameObject copy(other);
	swap(copy);
	return *this;
}

RawGameObject::RawGameObject(Rect rect)
	: rect(rect)
{}

bool RawGameObject::within(MPosition p) const {
	return (rect.p.x <= p.x && p.x < rect.p.x + rect.w &&
			rect.p.y <= p.y && p.y < rect.p.y + rect.h);
}
