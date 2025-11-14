#include <Actor.hpp>

Container::Container(int size) : size(size) {
}

Container::~Container() {
}

bool Container::add(Actor *actor) {
   if (this->size > 0 && this->inventory.size() >= this->size)
      // inventory full
      return false;
   this->inventory.push_back(actor);
   return true;
}

void Container::remove(Actor *actor) {
   unsigned i = 0;
   for (; i < this->inventory.size(); i++)
      if (actor == this->inventory[i]) break;
   if (i != this->inventory.size() - 1)
      std::swap(this->inventory[i], this->inventory.back());
   this->inventory.pop_back();
}
