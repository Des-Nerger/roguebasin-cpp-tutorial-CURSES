#include <curses.h>
#include <map.hpp>

Map::Map(int width, int height) : width(width), height(height) {
   this->tiles = new Tile[width * height];
   this->setWall(46, 6);
   this->setWall(50, 6);
}

Map::~Map() {
   delete[] this->tiles;
}

bool Map::isWall(int x, int y) const {
   return !this->tiles[x + y * this->width].canWalk;
}

void Map::setWall(int x, int y) {
   this->tiles[x + y * this->width].canWalk = false;
}

void Map::render() const {
   for (auto x = 0; x < this->width; x += 1)
      for (auto y = 0; y < this->height; y += 1)
         mvaddch(y, x, this->isWall(x, y)? '#' : '.');
}
