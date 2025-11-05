#pragma once

#include <libtcod.hpp>

struct Tile {
   bool explored; // has the player already seen this tile?
   Tile() : explored(false) {}
};

struct Map {
   int width, height;

   Map(int width, int height);
   ~Map();
   bool isWall(int x, int y) const;
   bool canWalk(int x, int y) const;
   bool isInFov(int x, int y) const;
   bool isExplored(int x, int y) const;
   void computeFov();
   void render() const;
   Tile *tiles;
   TCODMap *map;

   void dig(int x1, int y1, int x2, int y2);
   void createRoom(bool first, int x1, int y1, int x2, int y2);
   void setWall(int x, int y);
   void addMonster(int x, int y);
};

enum : int {
   MAX_ROOM_MONSTERS = 3,
   ROOM_MIN_SIZE = 6,
   ROOM_MAX_SIZE = 12,
};
