#pragma once

#include <Actor.hpp>
#include <map.hpp>
#include <vector>

struct Engine {
   std::vector<Actor *> actors;
   Actor *player;
   Map *map;
   int fovRadius;
   bool computeFovAtUpdate;

   Engine();
   ~Engine();
   bool update();
   void render(); 
};
