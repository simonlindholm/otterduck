#pragma once
#include <list>
#include <unordered_map>
#include <vector>
#include <cassert>
#include <memory>
#include "gameobject.h"

class GameObject;

// A container of GameObjects that can mark a point in history and later
// return back to it, both in amortized O(1).
class GOList {
	private:
		typedef unsigned char Mask;
		typedef std::shared_ptr<GameObject> T;
		typedef GameObject* Ptr;
		typedef std::list<std::pair<T, Mask>> Li;
		typedef Li::iterator Lit;
		typedef Li::const_iterator LCit;

		class BaseLayer;
		class ChangeLayer;
		class Layer {
			friend class BaseLayer;
			friend class ChangeLayer;
			private:
				std::shared_ptr<std::unordered_map<Ptr, Lit>> map;
				Mask lbit;
				int insCount;
				Li& li;
			public:
				virtual void markDirty(Lit it) = 0;
				virtual void erase(Lit it);
				virtual std::shared_ptr<Layer> getPrevLayer() const = 0;
				virtual Lit subLookupGO(Ptr ptr) const = 0;
				virtual void pushToPrev() = 0;

				Lit push_back(T el);
				Lit lookupGO(Ptr ptr) const;
				void initMap();
				Layer(Li& li) : map(), lbit(1), insCount(0), li(li) {}
				virtual ~Layer() {}
		};
		class BaseLayer : public Layer {
			public:
				void markDirty(Lit it) {}
				std::shared_ptr<Layer> getPrevLayer() const { assert(0); }
				Lit subLookupGO(Ptr ptr) const { return li.end(); }
				void pushToPrev() { assert(0); }

				BaseLayer(Li& li) : Layer(li) {}
		};
		class ChangeLayer : public Layer {
			private:
				std::shared_ptr<Layer> prev;
				std::vector<std::pair<Lit, T>> changed;
			public:
				void markDirty(Lit it);
				void erase(Lit it);
				std::shared_ptr<Layer> getPrevLayer() const { return prev; }
				Lit subLookupGO(Ptr ptr) const { return prev->lookupGO(ptr); }
				void pushToPrev();

				ChangeLayer(Li& li, std::shared_ptr<Layer> prev);
				~ChangeLayer();
		};

		class ConstIter : public std::iterator<std::forward_iterator_tag, GameObject> {
			friend class GOList;
			friend class Iter;
			private:
				LCit it, end;
				void advNonNull();
				ConstIter(const Li& li) : it(li.begin()), end(li.end()) { advNonNull(); }
				ConstIter(const Li& li, int e) : it(li.end()), end(li.end()) {}

			public:
				ConstIter() {}
				void operator++() { ++it; advNonNull(); }
				ConstIter operator++(int) { ConstIter copy = *this; ++it; advNonNull(); return copy; }
				bool operator==(const ConstIter& other) const { return it == other.it; }
				bool operator!=(const ConstIter& other) const { return !(*this == other); }
				const GameObject& operator*() const { return *it->first; }
				const GameObject* operator->() const { return &**this; }
		};

		class Iter : public std::iterator<std::forward_iterator_tag, GameObject> {
			friend class GOList;
			private:
				Lit it, end;
				void advNonNull();
				Iter(Li& li) : it(li.begin()), end(li.end()) { advNonNull(); }
				Iter(Li& li, int e) : it(li.end()), end(li.end()) {}
				Lit realIt() const { return it; }

			public:
				Iter() {}
				operator ConstIter() { ConstIter ret; ret.it = it; ret.end = end; return ret; }
				void operator++() { ++it; advNonNull(); }
				Iter operator++(int) { Iter copy = *this; ++it; advNonNull(); return copy; }
				bool operator==(const Iter& other) const { return it == other.it; }
				bool operator!=(const Iter& other) const { return !(*this == other); }
				GameObject& operator*() { return *it->first; }
				const GameObject& operator*() const { return *it->first; }
				GameObject* operator->() { return &**this; }
				const GameObject* operator->() const { return &**this; }
		};

		Li li;
		std::shared_ptr<Layer> layer;

	public:
		typedef Iter iterator;
		typedef ConstIter const_iterator;

		GOList() : layer(new BaseLayer(li)) {}
		void swap(GOList& other);

		iterator begin() { return Iter(li); }
		const_iterator begin() const { return ConstIter(li); }
		iterator end() { return Iter(li, 0); }
		const_iterator end() const { return ConstIter(li, 0); }

		void erase(iterator it);
		iterator push_back(T el);
		void markDirty(Ptr ptr);
		void markDirty(iterator it);

		void pushState();
		void popState();
		void saveState();
};
