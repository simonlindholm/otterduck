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

GameObject::GameObject(const GameObject& other) {
	RawGameObject* o = other.obj->clone();
	if (o) obj = std::shared_ptr<RawGameObject>(o);
	else obj = other.obj;
}
void GameObject::swap(GameObject& other) {
	obj.swap(other.obj);
}

GameObject& GameObject::operator=(const GameObject& other) {
	GameObject copy(other);
	swap(copy);
	return *this;
}

RawGameObject::RawGameObject(Rect rect)
	: rect(rect)
{}

void RawGameObject::swap(RawGameObject& other) {
	std::swap(rect, other.rect);
}
