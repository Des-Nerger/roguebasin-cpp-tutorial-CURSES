#pragma once

struct Tile {
   bool canWalk; // can we walk through this tile?
   Tile() : canWalk(false) {}
};

struct Map {
   int width, height;

   Map(int width, int height);
   ~Map();
   bool isWall(int x, int y) const;
   void render() const;
   Tile *tiles;

   void dig(int x1, int y1, int x2, int y2);
   void createRoom(bool first, int x1, int y1, int x2, int y2);
   void setWall(int x, int y);
};

enum : int {
   ROOM_MIN_SIZE = 6,
   ROOM_MAX_SIZE = 12,
};
