#include <Gui.hpp>
#include <curses.h>
#undef hline
#undef vline
#include <limits.h>
#include <main.hpp>
#include <math.h>

Gui::Gui(unsigned barCol) : barCol(barCol) {
   this->msg.reserve(LINES - 1);
}

Gui::~Gui() {
}

void Gui::render() {
   this->renderMouseLook();
   for (auto y = this->msg.size(); !this->msg.empty(); y--) {
      auto &msg = this->msg.back();
      this->msg.pop_back();
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
