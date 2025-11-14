#include <Actor.hpp>
#include <Engine.hpp>
#include <assert.h>
#include <curses.h>
#include <locale.h>
#include <main.hpp>
#include <map.hpp>
#include <stdlib.h>

Engine::Engine() : gameStatus(Engine::IDLE), fovRadius(10) {
   assert(NULL != setlocale(LC_CTYPE, ""));
   assert(initscr() == stdscr);
   ok(start_color());

   // ok(cbreak()); // why no effect?
   ok(noecho());
   ok(keypad(stdscr, true));
   assert(1 == curs_set(0));

   assert(0 != mousemask(
      BUTTON1_PRESSED |
         BUTTON2_PRESSED |
         BUTTON3_PRESSED |
         BUTTON4_PRESSED |
         BUTTON5_PRESSED,
      NULL));
   assert(true == has_mouse());

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
   this->player->container = new Container(26);
   
   this->actors.push_back(this->player);
   this->map = new Map(COLS, LINES);
   this->map->computeFov();
   this->gui = new Gui(
      COLOR_PAIR(alloc_pair(COLOR_WHITE, COLOR_RED)));
   auto col = COLOR_PAIR(alloc_pair(COLOR_RED, COLOR_BLACK));
   this->gui->msg.push_back(Gui::Msg{
      .col = col,
      .fmt = "Welcome stranger!" } );
   this->gui->msg.push_back(Gui::Msg{
      .col = col,
      .fmt =
         "Prepare to perish in the Tombs of the Ancient Kings." } );
}

Engine::~Engine() {
   ok(flushinp());
   ok(endwin());
   while (!this->actors.empty()) {
      auto actor = this->actors.back();
      delete actor;
      this->actors.pop_back();
   }
   delete this->gui;
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
   // show the log messages and player's stats
   this->gui->render();
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
   this->assertBlockingOrder(__func__);
}

void Engine::removeNonBlocking(Actor *actor) {
   assert(!actor->blocks);
   unsigned k = UINT_MAX;
   unsigned i = 0;
   for (; i < this->actors.size(); i += 1) {
      if (this->actors[i]->blocks) break;
      else if (actor == this->actors[i]) k = i;
   }
   if (k != this->actors.size() - 1) {
      assert(i > 0);
      i -= 1;
      if (i != k)
         std::swap(this->actors[i], this->actors[k]);
      std::swap(this->actors[i], this->actors.back());
   }
   this->actors.pop_back();
   this->assertBlockingOrder(__func__);
}

void Engine::assertBlockingOrder(const char *fnName) {
   auto blocks = false;
   for (auto actor : this->actors) {
      if (!(!blocks || actor->blocks)) {
         mvprintw
            (LINES - 2, COLS / 2, "assert failed at %s\n", fnName);
         ok(refresh());
         (void) getch();
         exit(EXIT_FAILURE);
      }
      blocks = actor->blocks;
   }
}
