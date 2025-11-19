#pragma once

struct Actor;

#include <Ai.hpp>
#include <Attacker.hpp>
#include <Container.hpp>
#include <Destructible.hpp>
#include <Pickable.hpp>

struct Actor {
   int x, y; // position on map
   int ch; // ascii code
   unsigned col; // color pair attr
   const char *name; // the actor's name
   bool blocks; // can we walk on this actor?
   Attacker *attacker; // something that deals damage
   Destructible *destructible; // something that can be damaged
   Ai *ai; // something self-updating
   Pickable *pickable; // something that can be picked and used
   Container *container; // something that can contain actors

   Actor(int x, int y, int ch, const char *name, int col);
   ~Actor();
   bool update();
   void render() const;
   long long squaredDistance(int cx, int cy) const;
};
