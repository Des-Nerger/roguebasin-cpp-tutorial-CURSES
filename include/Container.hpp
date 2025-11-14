#pragma once

#include <vector>

struct Container {
   unsigned size; // maximum number of actors. 0=unlimited
   std::vector<Actor *> inventory;

   Container(int size);
   ~Container();
   bool add(Actor *actor);
   void remove(Actor * actor);
};
