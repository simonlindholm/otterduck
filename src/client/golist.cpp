#include <algorithm>
#include <cassert>
#include "golist.h"
#include "gameobject.h"

void GOList::swap(GOList& other) {
	li.swap(other.li);
	std::swap(layer, other.layer);
}

void GOList::erase(iterator it) {
	Lit rit = it.realIt();
	assert(rit != li.end());
	layer->erase(rit);
}

GOList::iterator GOList::push_back(T el) {
	iterator it(li);
	it.it = layer->push_back(el);
	return it;
}

void GOList::markDirty(Ptr ptr) {
	Lit it = layer->subLookupGO(ptr);
	if (it != li.end())
		layer->markDirty(it);
}

void GOList::markDirty(iterator it) {
	Lit rit = it.realIt();
	assert(rit != li.end());
	layer->markDirty(rit);
}

void GOList::pushState() {
	layer.reset(new ChangeLayer(li, layer));
}

void GOList::popState() {
	layer = layer->getPrevLayer();
}

void GOList::saveState() {
	layer->pushToPrev();
}


GOList::ChangeLayer::ChangeLayer(Li& li, std::shared_ptr<Layer> prev)
	: Layer(li), prev(prev)
{
	assert(prev);
	Layer* pprev = prev.get();
	lbit = pprev->lbit << 1;
	prev->initMap();
}

GOList::ChangeLayer::~ChangeLayer() {
	// Remove all the newly added elements.
	while (insCount --> 0) {
		li.pop_back();
	}

	for (auto it = changed.begin(); it != changed.end(); ++it) {
		std::pair<Lit, T> ch = *it;
		if (ch.first->first)
			ch.first->first->setFrom(*ch.second.get());
		else
			ch.first->first = ch.second;
		ch.first->second &= ~lbit;
	}
}

void GOList::ChangeLayer::erase(Lit it) {
	if (!(it->second & (lbit-1))) {
		// The element was added in this layer, so just remove it the usual way.
		this->Layer::erase(it);
	}
	else {
		// The element was added in some previous layer, so delete it by
		// setting it to NULL and remembering the previous value (if it is
		// not already).
		assert(it->first);
		if (!(it->second & lbit)) {
			changed.push_back(make_pair(it, it->first));
			it->second |= lbit;
		}
		it->first.reset();
	}
}

void GOList::ChangeLayer::markDirty(Lit it) {
	if (!(it->second & lbit)) {
		T nval = T(it->first->clone());
		changed.push_back(make_pair(it, nval));
		it->second |= lbit;
	}
}

void GOList::ChangeLayer::pushToPrev() {
	if (map) map.reset();
	Mask pbit = prev->lbit;

	// Move over knowledge of the inserted elements to the previous layer.
	Lit it = li.end();
	for (int i = 0; i < insCount; ++i) {
		--it;
		it->second = pbit;
		prev->map->insert(make_pair(it->first.get(), it));
	}
	prev->insCount += insCount;
	insCount = 0;

	// Pretend that all the changes happened on the previous layer, if they
	// did not already.
	for (auto it = changed.begin(); it != changed.end(); ++it) {
		std::pair<Lit, T> ch = *it;
		ch.first->second &= ~lbit;

		if (!ch.first->first) {
			// Deleting elements should be done with erase().
			ch.first->first = ch.second;
			prev->erase(ch.first);
		}
		else if (!(ch.first->second & pbit)) {
			// Modification of a previously existing element that was not
			// changed in the previous layer, which implies that that layer
			// is not the BaseLayer, can be done simply by adding knowledge
			// of that modification to the 'changed' list.
			// This could be done in a less hacky and implementation-dependent
			// way at the expense of another el->clone() or a variant of
			// markDirty that takes a cloned element as a parameter, but meh.
			ch.first->second |= pbit;
			ChangeLayer* chprev = (ChangeLayer*)prev.get();
			chprev->changed.push_back(ch);
		}
	}
	changed.clear();
}


GOList::Lit GOList::Layer::push_back(T el) {
	// Remember it as an element that was changed in this layer, and also
	// add it in a marked state since we therefore know about it.
	li.push_back(make_pair(el, lbit));
	Lit it = li.end(); --it;
	if (map)
		map->insert(make_pair(el.get(), it));
	++insCount;
	return it;
}

void GOList::Layer::erase(Lit it) {
	assert(it->first);

	if (map)
		map->erase(it->first.get());
	li.erase(it);
	--insCount;
}

void GOList::Layer::initMap() {
	if (!map) {
		map.reset(new std::unordered_map<Ptr, Lit>());
		Lit it = li.end();
		for (int i = 0; i < insCount; ++i) {
			--it;
			map->insert(make_pair(it->first.get(), it));
		}
	}
}

GOList::Lit GOList::Layer::lookupGO(Ptr ptr) const {
	auto mit = map->find(ptr);
	return ((mit != map->end()) ? mit->second : this->subLookupGO(ptr));
}


void GOList::Iter::advNonNull() {
	while (it != end && !it->first) {
		++it;
	}
}

void GOList::ConstIter::advNonNull() {
	while (it != end && !it->first) {
		++it;
	}
}
