#pragma once

#include <Actor.hpp>
#include <Gui.hpp>
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

   Engine();
   ~Engine();
   bool update();
   void render(); 
   void sendNonBlockingToBack(Actor *actor);
};
