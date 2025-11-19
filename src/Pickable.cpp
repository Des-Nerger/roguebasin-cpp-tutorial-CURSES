#include <Actor.hpp>
#include <main.hpp>

bool Pickable::pick(Actor *owner, Actor *wearer) {
   if (wearer->container && wearer->container->add(owner)) {
      ::engine.removeNonBlocking(owner);
      return true;
   }
   return false;
}

void Pickable::drop(Actor *owner, Actor *wearer) {
   if (wearer->container) {
      wearer->container->remove(owner);
      ::engine.actors.push_back(owner);
      ::engine.sendNonBlockingToBack(owner);
      owner->x = wearer->x;
      owner->y = wearer->y;
      ::engine.gui->msg.push_back({
         .col = A_DIM,
         .fmt = "%s drops a %s.",
         .ownName = wearer->name,
         .targName = owner->name });
   }
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

LightningBolt::LightningBolt(float range, float damage)
   : range(range), damage(damage) {}

bool LightningBolt::use(Actor *owner, Actor *wearer) {
   auto closestMonster = ::engine.getClosestMonster(
      wearer->x,
      wearer->y,
      this->range);
   if (!closestMonster) {
      ::engine.gui->msg.push_back({
         .col = A_DIM,
         .fmt = "No enemy is close enough to strike." });
      return false;
   }
   // hit closest monster for <damage> hit points
   auto col = A_BOLD | COLOR_PAIR(alloc_pair(COLOR_BLUE, COLOR_BLACK));
   ::engine.gui->msg.push_back({
      .col = col,
      .fmt = "A lightning bolt strikes the %s with a loud thunder!",
      .ownName = closestMonster->name });
   ::engine.gui->msg.push_back({
      .col = col,
      .fmt = "%s%sThe damage is %g points.",
      .floatArg = this->damage});
   closestMonster->destructible->takeDamage(
      closestMonster,
      this->damage );
   return this->Pickable::use(owner, wearer);
}

Fireball::Fireball(float range, float damage)
   : LightningBolt(range, damage) {}

bool Fireball::use(Actor *owner, Actor *wearer) {
   auto col = COLOR_PAIR(alloc_pair(COLOR_CYAN, COLOR_BLACK));
   ::engine.gui->msg.push_back({
      .col = col,
      .fmt = "Mouse-click a target tile for the fireball."
   });
   if (!(::engine).pickATile(
          COLOR_PAIR(alloc_pair(COLOR_BLACK, COLOR_RED)),
       this->range )
   ) return false;
   // burn everything in <range> (including player)
   col = COLOR_PAIR(alloc_pair(COLOR_RED, COLOR_BLACK));
   ::engine.gui->msg.push_back({
      .col = col,
      .fmt = "%s%sThe fireball explodes, burning "
             "everything within %g tiles!",
      .floatArg = this->range });
   auto sqRange = (this->range == FLT_MAX) ?
      (this->range)
   :
      (this->range * this->range);
   for (auto actor : ::engine.actors)
      if (actor->destructible &&
          !actor->destructible->isDead() &&
          actor->squaredDistance(
            ::engine.mouse.cx,
            ::engine.mouse.cy) <= sqRange)
      {
         auto damage = this->damage / ((actor == ::engine.player)?2:1);
         ::engine.gui->msg.push_back({
            .col = col,
            .fmt = "The %s%s gets burned for %g hit points.",
            .ownName = actor->name,
            .floatArg = damage });
         actor->destructible->takeDamage(actor, damage);
      }
   ::engine.mouse = { .cx = INT_MIN, .cy = INT_MIN };
   return this->Pickable::use(owner, wearer);
}

Confuser::Confuser(int nbTurns, float range)
   : nbTurns(nbTurns), range(range) {}

bool Confuser::use(Actor *owner, Actor *wearer) {
   auto col = COLOR_PAIR(alloc_pair(COLOR_CYAN, COLOR_BLACK));
   ::engine.gui->msg.push_back({
      .col = col,
      .fmt = "Mouse-click an enemy to confuse it."
   });
   if (!(::engine).pickATile(
          COLOR_PAIR(alloc_pair(COLOR_BLACK, COLOR_MAGENTA)),
       this->range )
   ) return false;
   auto actor = ::engine.getActor(
      ::engine.mouse.cx,
      ::engine.mouse.cy );
   if (!actor) return false;
   actor->ai = new ConfusedMonsterAi(this->nbTurns, actor->ai);
   actor->col |= A_REVERSE;
   col = COLOR_PAIR(alloc_pair(COLOR_GREEN, COLOR_BLACK));
   ::engine.gui->msg.push_back({
      .col = col,
      .fmt = "The eyes of the %s look vacant,",
      .ownName = actor->name });
   ::engine.gui->msg.push_back({
      .col = col,
      .fmt = "as he starts to stumble around!",
      .ownName = actor->name });
   return this->Pickable::use(owner, wearer);
}
