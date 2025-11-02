#pragma once

struct Actor {
   int x, y; // position on map
   int ch; // ascii code
   int col; // color pair attr

   Actor(int x, int y, int ch, int col);
   void render() const;
};
