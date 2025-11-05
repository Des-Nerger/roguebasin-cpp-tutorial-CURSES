#include <curses.h>
#undef hline
#undef vline
#include <libtcod.hpp>
#include <main.hpp>
#include <map.hpp>
#include <utility>

struct BspListener : ITCODBspCallback {
   Map &map; // a map to dig
   int roomNum; // room number
   int lastx, lasty; // center of the last room

   BspListener(Map &map) : map(map), roomNum(0) {}
   bool visitNode(TCODBsp *node, void *userData) {
      if (node->isLeaf()) {
         // dig a room
         auto rng = TCODRandom::getInstance();
         auto w = rng->getInt(ROOM_MIN_SIZE, node->w - 2);
         auto h = rng->getInt(ROOM_MIN_SIZE, node->h - 2);
         auto x = rng->getInt(node->x + 1, node->x + node->w - w - 1);
         auto y = rng->getInt(node->y + 1, node->y + node->h - h - 1);
         map.createRoom
            (0 == this->roomNum, x, y, x + w - 1, y + h - 1);
         if (0 != this->roomNum) {
            // dig a corridor from last room
            map.dig(lastx, lasty, x + w / 2, lasty);
            map.dig(x + w / 2, lasty, x + w / 2, y + h / 2);
         }
         lastx = x + w / 2;
         lasty = y + h / 2;
         this->roomNum += 1;
      }
      return true;
   }
};

Map::Map(int width, int height) : width(width), height(height) {
   this->tiles = new Tile[width * height];
   TCODBsp bsp(0, 0, width, height);
   bsp.splitRecursive(
      NULL,
      8,
      ROOM_MAX_SIZE,
      ROOM_MAX_SIZE,
      1.5f,
      1.5f);
   BspListener listener(*this);
   bsp.traverseInvertedLevelOrder(&listener, NULL);
}

Map::~Map() {
   delete[] this->tiles;
}

bool Map::isWall(int x, int y) const {
   return !this->tiles[x + y * this->width].canWalk;
}

void Map::dig(int x1, int y1, int x2, int y2) {
   if (x2 < x1) std::swap(x1, x2);
   if (y2 < y1) std::swap(y1, y2);
   for (auto x = x1; x <= x2; x++)
      for (auto y = y1; y <= y2; y++)
         this->tiles[x + y * this->width].canWalk = true;
}

void Map::createRoom(bool first, int x1, int y1, int x2, int y2) {
   this->dig(x1, y1, x2, y2);
   if (first) {
      // put the player in the first room
      ::engine.player->x = (x1 + x2) / 2;
      ::engine.player->y = (y1 + y2) / 2;
   } else {
      auto rng = TCODRandom::getInstance();
      if (0 == rng->getInt(0, 3))
         ::engine.actors.push_back(
            new Actor(
               (x1 + x2) / 2,
               (y1 + y2) / 2,
               '@',
               COLOR_YELLOW));
   }
}

void Map::setWall(int x, int y) {
   this->tiles[x + y * this->width].canWalk = false;
}

void Map::render() const {
   for (auto x = 0; x < this->width; x += 1)
      for (auto y = 0; y < this->height; y += 1)
         mvaddch(y, x, this->isWall(x, y)? '#' : '.');
}
