#pragma once

struct Actor {
   int x, y; // position on map
   int ch; // ascii code
   const char *name; // the actor's name
   int col; // color pair attr

   Actor(int x, int y, int ch, const char *name, int col);
   void update();
   bool moveOrAttack(int x, int y);
   void render() const;
};
