#include <Actor.hpp>
#include <Attacker.hpp>
#include <curses.h>
#undef hline
#undef vline
#include <main.hpp>

Attacker::Attacker(float power) : power(power) {}

void Attacker::attack(Actor *owner, Actor *target) {
   ::engine.gui->msg.push_back({});
   auto &msg = ::engine.gui->msg.back();
   msg.ownName = owner->name;
   msg.targName = target->name;
   if (target->destructible && !target->destructible->isDead()) {
      if (this->power - target->destructible->defense > 0) {
         msg.col = (owner == ::engine.player)?
            COLOR_PAIR(alloc_pair(COLOR_RED, COLOR_BLACK))
         :
            A_DIM;
         msg.fmt = "%s attacks %s for %g hit points.";
         msg.floatArg =
            this->power - target->destructible->defense;
      } else {
         msg.col = A_DIM;
         msg.fmt = "%s attacks %s but it has no effect!";
      }
      target->destructible->takeDamage(target, power);
   } else {
      msg.col = A_DIM;
      msg.fmt = "%s attacks %s in vain.";
   }
}
