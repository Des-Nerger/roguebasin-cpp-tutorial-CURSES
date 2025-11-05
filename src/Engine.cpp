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
   this->actors.push_back(this->player);
   this->map = new Map(COLS, LINES);
   this->map->computeFov();
}

Engine::~Engine() {
   while (!this->actors.empty()) {
      delete this->actors.back();
      this->actors.pop_back();
   }
   delete this->map;

   ok(flushinp());
   ok(endwin());
}

bool Engine::update() {
   this->gameStatus = Engine::IDLE;
   int dx = 0, dy = 0;
   auto ch = getch();
   switch (ch) {
   case 'q': return false;
   case KEY_UP:
      dy -= 1;
      break;
   case KEY_DOWN:
      dy += 1;
      break;
   case KEY_LEFT:
      dx -= 1;
      break;
   case KEY_RIGHT:
      dx += 1;
      break;
   }
   if (dx != 0 || dy != 0) {
      this->gameStatus = Engine::NEW_TURN;
      if (this->player->moveOrAttack(
          this->player->x + dx,
          this->player->y + dy)) this->map->computeFov();
   }
   if (this->gameStatus == Engine::NEW_TURN)
      for (auto actor : this->actors)
         if (actor != this->player)
            actor->update();
   return true;
}

void Engine::render() {
   ok(erase());
   // draw the map
   this->map->render();
   for (auto actor : this->actors)
      if (map->isInFov(actor->x, actor->y))
         actor->render();
   if (NULL != this->currentMessage.fmt) {
      mvprintw(
         0,
         0,
         this->currentMessage.fmt,
         this->currentMessage.name);
      this->currentMessage.fmt = NULL;
   }
}
