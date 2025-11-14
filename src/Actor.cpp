#include <Actor.hpp>
#include <curses.h>
#undef hline
#undef vline
#include <main.hpp>

Actor::Actor(int x, int y, int ch, const char *name, int col) :
   x(x),
   y(y),
   ch(ch),
   name(name),
   blocks(true),
   attacker(NULL),
   destructible(NULL),
   ai(NULL),
   pickable(NULL),
   container(NULL)
{
   this->col = col;
}

Actor::~Actor() {
   if (this->attacker) delete this->attacker;
   if (this->destructible) delete this->destructible;
   if (this->ai) delete this->ai;
   if (this->pickable) delete this->pickable;
   if (this->container) delete this->container;
}

bool Actor::update() {
   if (this->ai) return this->ai->update(this);
   return true;
}

void Actor::render() const {
   attron(this->col);
   mvaddch(this->y, this->x, this->ch);
   attroff(this->col);
}
