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
   struct Msg {
      const char *fmt; // format string
      const char *ownName;
      const char *targName;
      float floatArg;
   };
   std::vector<Engine::Msg> msg; // current messages
   int lastKey;

   Engine();
   ~Engine();
   bool update();
   void render(); 
   void sendNonBlockingToBack(Actor *actor);
};
