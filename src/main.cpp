#include <assert.h>
#include <curses.h>
#include <locale.h>

#define ok(result) assert(OK == (result))

int main() {
   assert(NULL != setlocale(LC_CTYPE, ""));
   assert(initscr() == stdscr);

   // ok(cbreak()); // why no effect?
   ok(noecho());
   ok(keypad(stdscr, true));
   assert(1 == curs_set(0));

   int playerx = COLS / 2, playery = LINES / 2;
   for (;;) {
      ok(erase());
      mvaddch(playery, playerx, '@');
      auto ch = getch();
      switch (ch) {
      case 'q':
         goto break_loop;
         break;
      case KEY_UP:
         playery -= 1;
         break;
      case KEY_DOWN:
         playery += 1;
         break;
      case KEY_LEFT:
         playerx -= 1;
         break;
      case KEY_RIGHT:
         playerx += 1;
         break;
      }
   }
   break_loop:

   ok(flushinp());
   ok(endwin());
}
