#include <Actor.hpp>
#include <Attacker.hpp>
#include <main.hpp>

Attacker::Attacker(float power) : power(power) {}

void Attacker::attack(Actor *owner, Actor *target) {
   ::engine.msg.push_back({});
   auto &msg = ::engine.msg.back();
   msg.ownName = owner->name;
   msg.targName = target->name;
   if (target->destructible && !target->destructible->isDead()) {
      if (this->power - target->destructible->defense > 0) {
         msg.fmt = "%s attacks %s for %g hit points.";
         msg.floatArg =
            this->power - target->destructible->defense;
      } else msg.fmt = "%s attacks %s but it has no effect!";
      target->destructible->takeDamage(target, power);
   } else msg.fmt = "%s attacks %s in vain.";
}
