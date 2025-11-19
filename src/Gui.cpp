#include <Gui.hpp>
#include <curses.h>
#undef hline
#undef vline
#include <limits.h>
#include <main.hpp>
#include <math.h>

Gui::Gui(unsigned barCol) : barCol(barCol) {
   this->msg.reserve(LINES - 1);
   this->inventoryWin = newwin(
      15,
      COLS / 2,
      LINES / 2 - 15 / 2,
      COLS / 2);
   assert(NULL != this->inventoryWin);
   wbkgdset(
      this->inventoryWin,
      ' ' | A_DIM | COLOR_PAIR(alloc_pair(COLOR_WHITE, COLOR_WHITE)));
}

Gui::~Gui() {
   ok(delwin(this->inventoryWin));
}

void Gui::render() {
   this->renderMouseLook();
   for (auto y = this->msg.size(); y >= 1; y--) {
      auto &msg = this->msg[y - 1];
      if (::engine.gameStatus != Engine::DEFEAT) this->msg.pop_back();
      if (UINT_MAX != msg.col) attron(msg.col);
      mvprintw(
         y,
         0,
         msg.fmt,
         msg.ownName,
         msg.targName,
         msg.floatArg);
      if (UINT_MAX != msg.col) attroff(msg.col);
   }
   this->renderBar(
      0,
      LINES - 1,
      "HP",
      ::engine.player->destructible->hp,
      ::engine.player->destructible->maxHp);
}

void Gui::renderBar(
   int x,
   int y,
   const char *name,
   float value,
   float maxValue)
{
   if (UINT_MAX != this->barCol) attron(this->barCol);
   mvprintw(
      y,
      x,
      "%s : %d/%d",
      name,
      (int) value,
      (int) maxValue);
   if (UINT_MAX != this->barCol) attroff(this->barCol);
}

void Gui::renderMouseLook() {
   if (!(::engine).map->isInFov(::engine.mouse.cx, ::engine.mouse.cy))
      return;
   if (-1 != ::engine.pick.maxSqRange) return;
   int x = 0;
   attron(A_DIM);
   for (auto actor : ::engine.actors)
      if (actor->x == ::engine.mouse.cx &&
          actor->y == ::engine.mouse.cy)
      {
         mvprintw(0, x, "%s; ", actor->name);
         x += strlen(actor->name) + strlen("; ");
      }
   attroff(A_DIM);
   ::engine.mouse = { .cx = INT_MIN, .cy = INT_MIN };
}
