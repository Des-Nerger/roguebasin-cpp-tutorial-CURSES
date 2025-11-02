#include <Actor.hpp>
#include <curses.h>

Actor::Actor(int x, int y, int ch, int fg_color) :
   x(x),
   y(y),
   ch(ch)
{
   this->col = COLOR_PAIR(alloc_pair(fg_color, COLOR_BLACK));
}

void Actor::render() const {
   attron(this->col);
   mvaddch(this->y, this->x, this->ch);
   attroff(this->col);
}
