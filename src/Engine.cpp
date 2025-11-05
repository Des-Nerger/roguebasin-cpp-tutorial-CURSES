#include <Actor.hpp>
#include <Engine.hpp>
#include <assert.h>
#include <curses.h>
#include <locale.h>
#include <main.hpp>
#include <map.hpp>

Engine::Engine() {
   assert(NULL != setlocale(LC_CTYPE, ""));
   assert(initscr() == stdscr);
   ok(start_color());

   // ok(cbreak()); // why no effect?
   ok(noecho());
   ok(keypad(stdscr, true));
   assert(1 == curs_set(0));

   this->player = new Actor(COLS / 2, LINES / 2, '@', COLOR_WHITE);
   this->actors.push_back(this->player);
   this->map = new Map(COLS, LINES);
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
   auto ch = getch();
   switch (ch) {
   case 'q': return false;
   case KEY_UP:
      if (!this->map->isWall(this->player->x, this->player->y - 1))
         this->player->y -= 1;
      break;
   case KEY_DOWN:
      if (!this->map->isWall(this->player->x, this->player->y + 1))
         this->player->y += 1;
      break;
   case KEY_LEFT:
      if (!this->map->isWall(this->player->x - 1, this->player->y))
         this->player->x -= 1;
      break;
   case KEY_RIGHT:
      if (!this->map->isWall(this->player->x + 1, this->player->y))
         this->player->x += 1;
      break;
   }
   return true;
}

void Engine::render() {
   ok(erase());
   // draw the map
   this->map->render();
   for (auto actor : actors)
      actor->render();
}
