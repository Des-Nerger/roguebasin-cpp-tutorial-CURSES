#pragma once

struct Pickable {
   bool pick(Actor *owner, Actor *wearer);
   virtual bool use(Actor *owner, Actor *wearer);
   virtual ~Pickable() {}
};

struct Healer : Pickable {
   float amount; // how many hp

   Healer(float amount);
   bool use(Actor *owner, Actor *wearer) override;
};
