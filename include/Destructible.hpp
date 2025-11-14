#pragma once

struct Destructible {
   float maxHp; // maximum health points
   float hp; // current health points
   float defense; // hit points deflected
   const char *corpseName; // the actor's name once dead/destroyed

   Destructible(float maxHp, float defense, const char *corpseName);
   inline bool isDead() { return this->hp <= 0.0; }
   float takeDamage(Actor *owner, float damage);
   float heal(float amount);
   virtual void die(Actor *owner);
   virtual ~Destructible() {}
};

struct MonsterDestructible : Destructible {
   MonsterDestructible(
      float maxHp,
      float defense,
      const char *corpseName);
   void die(Actor *owner) override;
};

struct PlayerDestructible : Destructible {
   PlayerDestructible(
      float maxHp,
      float defense,
      const char *corpseName);
   void die(Actor *owner) override;
};
