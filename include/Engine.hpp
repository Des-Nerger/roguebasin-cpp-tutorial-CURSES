#pragma once

#include <Actor.hpp>
#include <Gui.hpp>
#include <float.h>
#include <limits.h>
#include <map.hpp>
#include <vector>

struct Engine {
   std::vector<Actor *> actors;
   Actor *player;
   Map *map;
   enum GameStatus {
      IDLE, // no new turn. Redraw the same screen.
      NEW_TURN, // update the monsters position
      VICTORY, // the player won
      DEFEAT // the player was killed
   } gameStatus;
   int fovRadius;
   int lastKey;
   Gui *gui;
   struct {
      int cx = INT_MIN, cy = INT_MIN;
   } mouse;
   struct {
      long long maxSqRange = -1;
      unsigned col; // color pair attr
   } pick;

   Engine();
   ~Engine();
   bool update();
   void render(); 
   void sendNonBlockingToBack(Actor *actor);
   void removeNonBlocking(Actor *actor);
   void assertBlockingOrder(const char *fnName);
   Actor *getClosestMonster(int x, int y, float range) const;
   bool pickATile(unsigned col, float maxRange = FLT_MAX);
   Actor *getActor(int x, int y) const;
};
