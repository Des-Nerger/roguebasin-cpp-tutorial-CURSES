#pragma once

struct Pickable {
   bool pick(Actor *owner, Actor *wearer);
   void drop(Actor *owner, Actor *wearer);
   virtual bool use(Actor *owner, Actor *wearer);
   virtual ~Pickable() {}
};

struct Healer : Pickable {
   float amount; // how many hp

   Healer(float amount);
   bool use(Actor *owner, Actor *wearer) override;
};

struct LightningBolt : Pickable {
   float range, damage;
   LightningBolt(float range, float damage);
   bool use(Actor *owner, Actor *wearer) override;
};

struct Fireball : LightningBolt {
   Fireball(float range, float damage);
   bool use(Actor *owner, Actor *wearer) override;
};

struct Confuser : Pickable {
   int nbTurns;
   float range;
   Confuser(int nbTurns, float range);
   bool use(Actor *owner, Actor *wearer) override;
};
