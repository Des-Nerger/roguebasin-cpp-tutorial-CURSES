#include <Actor.hpp>
#include <Engine.hpp>
#include <assert.h>
#include <curses.h>
#include <locale.h>
#include <main.hpp>
#include <map.hpp>

Engine::Engine() : gameStatus(Engine::IDLE), fovRadius(10) {
   assert(NULL != setlocale(LC_CTYPE, ""));
   assert(initscr() == stdscr);
   ok(start_color());

   // ok(cbreak()); // why no effect?
   ok(noecho());
   ok(keypad(stdscr, true));
   assert(1 == curs_set(0));

   this->player = new Actor(
      COLS / 2,
      LINES / 2,
      '@',
      "player",
      COLOR_PAIR(alloc_pair(COLOR_WHITE, COLOR_BLACK)) );
   this->player->destructible = new PlayerDestructible(
      30,
      2,
      "your cadaver");
   this->player->attacker = new Attacker(5);
   this->player->ai = new PlayerAi();
   
   this->actors.push_back(this->player);
   this->map = new Map(COLS, LINES);
   this->map->computeFov();
   this->msg.reserve(LINES);
}

Engine::~Engine() {
   ok(flushinp());
   ok(endwin());
   while (!this->actors.empty()) {
      delete this->actors.back();
      this->actors.pop_back();
   }
   delete this->map;
}

bool Engine::update() {
   this->gameStatus = Engine::IDLE;
   if (!this->player->update()) return false;
   if (this->gameStatus == Engine::NEW_TURN)
      for (auto actor : this->actors)
         if (actor != this->player)
            (void) actor->update();
   return true;
}

void Engine::render() {
   ok(erase());
   // draw the map
   this->map->render();
   for (auto actor : this->actors)
      if (map->isInFov(actor->x, actor->y))
         actor->render();
   for (auto y = this->msg.size() - 1; !this->msg.empty(); y--) {
      auto &msg = this->msg.back();
      this->msg.pop_back();
      mvprintw(
         y,
         0,
         msg.fmt,
         msg.ownName,
         msg.targName,
         msg.floatArg);
   }
   mvprintw(
      LINES - 1,
      0,
      "HP : %d/%d",
      (int) this->player->destructible->hp,
      (int) this->player->destructible->maxHp);
}

void Engine::sendNonBlockingToBack(Actor *actor) {
   assert(!actor->blocks);
   unsigned i = 0, j = this->actors.size() - 1;
   for (; i < this->actors.size(); i++)
      if (this->actors[i]->blocks) break;
   for (; j >= 0; j--)
      if (actor == this->actors[j]) break;
   if (i >= j) return;
   std::swap(this->actors[i], this->actors[j]);
}
