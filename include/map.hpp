#pragma once

struct Tile {
   bool canWalk; // can we walk through this tile?
   Tile() : canWalk(true) {}
};

struct Map {
   int width, height;

   Map(int width, int height);
   ~Map();
   bool isWall(int x, int y) const;
   void render() const;
   Tile *tiles;

   void setWall(int x, int y);
};
