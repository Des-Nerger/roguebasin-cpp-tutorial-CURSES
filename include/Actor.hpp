#pragma once

struct Actor;

#include <Ai.hpp>
#include <Attacker.hpp>
#include <Destructible.hpp>

struct Actor {
   int x, y; // position on map
   int ch; // ascii code
   unsigned col; // color pair attr
   const char *name; // the actor's name
   bool blocks; // can we walk on this actor?
   Attacker *attacker; // something that deals damage
   Destructible *destructible; // something that can be damaged
   Ai *ai; // something self-updating

   Actor(int x, int y, int ch, const char *name, int col);
   bool update();
   // bool moveOrAttack(int x, int y);
   void render() const;
};
