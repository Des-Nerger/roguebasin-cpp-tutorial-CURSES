#include <Actor.hpp>
#include <main.hpp>

bool Pickable::pick(Actor *owner, Actor *wearer) {
   if (wearer->container && wearer->container->add(owner)) {
      ::engine.removeNonBlocking(owner);
      return true;
   }
   return false;
}

bool Pickable::use(Actor *owner, Actor *wearer) {
   if (wearer->container) {
      wearer->container->remove(owner);
      delete owner;
      return true;
   }
   return false;
}

Healer::Healer(float amount) : amount(amount) {
}

bool Healer::use(Actor *owner, Actor *wearer) {
   if (wearer->destructible) {
      auto amountHealed = wearer->destructible->heal(this->amount);
      if (amountHealed > 0) return this->Pickable::use(owner, wearer);
   }
   return false;
}
