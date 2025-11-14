#include <Actor.hpp>
#include <Destructible.hpp>
#include <curses.h>
#undef hline
#undef vline
#include <main.hpp>

Destructible::Destructible(
   float maxHp,
   float defense,
   const char *corpseName
) : maxHp(maxHp), hp(maxHp), defense(defense), corpseName(corpseName)
   {}

float Destructible::takeDamage(Actor *owner, float damage) {
   damage -= this->defense;
   if (damage > 0) {
      this->hp -= damage;
      if (this->hp <= 0) this->die(owner);
   } else damage = 0;
   return damage;
}

void Destructible::die(Actor *owner) {
   // transform the actor into a corpse!
   owner->ch = '%';
   owner->col = A_DIM |
      COLOR_PAIR(alloc_pair(COLOR_RED, COLOR_BLACK));
   owner->name = this->corpseName;
   owner->blocks = false;
   ::engine.sendNonBlockingToBack(owner);
}

MonsterDestructible::MonsterDestructible(
   float maxHp,
   float defense,
   const char *corpseName) : Destructible(maxHp, defense, corpseName)
{}

float Destructible::heal(float amount) {
   this->hp += amount;
   auto overAmount = this->hp - this->maxHp;
   if (overAmount > 0) {
      amount -= overAmount;
      this->hp = this->maxHp;
   }
   return amount;
}

void MonsterDestructible::die(Actor *owner) {
   // tranform it into a nasty corpse! it doesn't block, can't be
   // attacked and doesn't move
   ::engine.gui->msg.push_back({
      .col = A_DIM,
      .fmt = "%s is dead",
      .ownName = owner->name});
   Destructible::die(owner);
}

PlayerDestructible::PlayerDestructible(
   float maxHp,
   float defense,
   const char *corpseName) : Destructible(maxHp, defense, corpseName) {}

void PlayerDestructible::die(Actor *owner) {
   ::engine.gui->msg.push_back({
      .col = COLOR_PAIR(alloc_pair(COLOR_RED, COLOR_BLACK)),
      .fmt = "You died!"});
   Destructible::die(owner);
   ::engine.gameStatus = Engine::DEFEAT;
}
