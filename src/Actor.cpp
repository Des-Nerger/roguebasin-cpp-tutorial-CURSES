#include <Actor.hpp>
#include <curses.h>
#undef hline
#undef vline
#include <main.hpp>

Actor::Actor(int x, int y, int ch, const char *name, int col) :
   x(x),
   y(y),
   ch(ch),
   name(name)
{
   this->col = col;
}

void Actor::update() {
   if (::engine.map->isInFov(this->x, this->y))
      if (NULL == ::engine.currentMessage.fmt) {
         ::engine.currentMessage.fmt = "The %s growls!\n";
         ::engine.currentMessage.name = this->name;
      }
}

bool Actor::moveOrAttack(int x, int y) {
   if (::engine.map->isWall(x, y)) return false;
   for (auto actor : ::engine.actors)
      if (x == actor->x && y == actor->y) {
         ::engine.currentMessage.fmt =
            "The %s laughs at your puny efforts to attack him!\n";
         ::engine.currentMessage.name = actor->name;
         return false;
      }
   this->x = x;
   this->y = y;
   return true;
}

void Actor::render() const {
   attron(this->col);
   mvaddch(this->y, this->x, this->ch);
   attroff(this->col);
}
