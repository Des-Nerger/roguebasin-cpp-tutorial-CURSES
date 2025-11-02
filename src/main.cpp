#include <Engine.hpp>
#include <assert.h>
#include <curses.h>
#include <locale.h>

Engine engine;

int main() {
   do {
      engine.render();
      ok(refresh());
   } while (engine.update());
}
