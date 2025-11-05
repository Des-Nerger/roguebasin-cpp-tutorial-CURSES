#pragma once

#include <Actor.hpp>
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
   struct {
      const char *fmt;
      const char *name;
   } currentMessage;

   Engine();
   ~Engine();
   bool update();
   void render(); 
};
