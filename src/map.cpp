#include <Actor.hpp>
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
   this->map = new TCODMap(width, height);
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
   delete this->map;
}

bool Map::isWall(int x, int y) const {
   return !this->map->isWalkable(x, y);
}

bool Map::canWalk(int x, int y) const {
   if (this->isWall(x, y)) return false;
   for (auto actor : ::engine.actors)
      if (actor->blocks && x == actor->x && y == actor->y)
         // there is a blocking actor here. cannot walk
         return false;
   return true;
}

bool Map::isExplored(int x, int y) const {
   return this->tiles[x + y * this->width].explored;
}

bool Map::isInFov(int x, int y) const {
   if (x < 0 ||
       x >= this->map->getWidth() ||
       y < 0 ||
       y >= this->map->getHeight()) return false;
   if (this->map->isInFov(x, y)) {
      this->tiles[x + y * width].explored = true;
      return true;
   }
   return false;
}

void Map::computeFov() {
   this->map->computeFov(
      ::engine.player->x,
      ::engine.player->y,
      ::engine.fovRadius);
}

void Map::dig(int x1, int y1, int x2, int y2) {
   if (x2 < x1) std::swap(x1, x2);
   if (y2 < y1) std::swap(y1, y2);
   for (auto x = x1; x <= x2; x++)
      for (auto y = y1; y <= y2; y++)
         this->map->setProperties(x, y, true, true);
}

void Map::createRoom(bool first, int x1, int y1, int x2, int y2) {
   this->dig(x1, y1, x2, y2);
   if (first) {
      // put the player in the first room
      ::engine.player->x = (x1 + x2) / 2;
      ::engine.player->y = (y1 + y2) / 2;
   } else {
      auto rng = TCODRandom::getInstance();
      for (auto nbMonsters = rng->getInt(0, MAX_ROOM_MONSTERS);
           nbMonsters > 0;
           nbMonsters -= 1)
      {
         auto x = rng->getInt(x1, x2);
         auto y = rng->getInt(y1, y2);
         if (this->canWalk(x, y))
            this->addMonster(x, y);
      }
      for (auto nbItems = rng->getInt(0, MAX_ROOM_ITEMS);
           nbItems > 0;
           nbItems--)
      {
         auto x = rng->getInt(x1, x2);
         auto y = rng->getInt(y1, y2);
         if (this->canWalk(x, y)) this->addItem(x, y);
      }
   }
}

void Map::setWall(int x, int y) {
   this->map->setProperties(x, y, false, false);
}

void Map::render() const {
   for (auto x = 0; x < this->width; x += 1)
      for (auto y = 0; y < this->height; y += 1) {
         unsigned attr = 0;
         if (this->isInFov(x, y)) {
            if (::engine.player->squaredDistance(x, y) <=
                   ::engine.pick.maxSqRange
            ) attr |= ::engine.pick.col;
         } else if (this->isExplored(x, y))
            attr |= A_DIM;
         else continue;
         if (0 != attr) attron(attr);
         mvaddch(y, x, this->isWall(x, y)? '#' : '.');
         if (0 != attr) attroff(attr);
      }
}

void Map::addMonster(int x, int y) {
   auto rng = TCODRandom::getInstance();
   if (rng->getInt(0, 100) < 80) {
      // create an orc
      auto orc = new Actor(
         x,
         y,
         'o',
         "orc",
         COLOR_PAIR(alloc_pair(COLOR_CYAN, COLOR_BLACK)) );
      orc->destructible = new MonsterDestructible(10, 0, "dead orc");
      orc->attacker = new Attacker(3);
      orc->ai = new MonsterAi();
      ::engine.actors.push_back(orc);
   } else {
      // create a troll
      auto troll = new Actor(
         x,
         y,
         'T',
         "troll",
         COLOR_PAIR(alloc_pair(COLOR_GREEN, COLOR_BLACK)));
      troll->destructible = new MonsterDestructible
         (16, 1, "troll carcass");
      troll->attacker = new Attacker(4);
      troll->ai = new MonsterAi();
      ::engine.actors.push_back(troll);
   }
}

void Map::addItem(int x, int y) {
   auto rng = TCODRandom::getInstance();
   auto dice = rng->getInt(0, 100);
   if (dice < 70) {
      // create a health potion
      auto healthPotion = new Actor(
         x,
         y,
         '!',
         "health potion",
         A_BOLD |
            COLOR_PAIR(alloc_pair(COLOR_MAGENTA, COLOR_BLACK)) );
      healthPotion->blocks = false;
      healthPotion->pickable = new Healer(4);
      ::engine.actors.push_back(healthPotion);
      ::engine.sendNonBlockingToBack(healthPotion);
   } else if (dice < 70 + 10) {
      // create a scroll of lightning bolt
      auto lightnBoltScroll = new Actor(
         x,
         y,
         '#',
         "scroll of lightning bolt",
         A_BOLD |
            COLOR_PAIR(alloc_pair(COLOR_YELLOW, COLOR_BLACK)) );
      lightnBoltScroll->blocks = false;
      lightnBoltScroll->pickable = new LightningBolt(5, 20);
      ::engine.actors.push_back(lightnBoltScroll);
      ::engine.sendNonBlockingToBack(lightnBoltScroll);
   } else if (dice < 70 + 10 + 10) {
      // create a scroll of fireball
      auto fireballScroll = new Actor(
         x,
         y,
         '#',
         "scroll of fireball",
         A_BOLD |
            COLOR_PAIR(alloc_pair(COLOR_YELLOW, COLOR_BLACK)) );
      fireballScroll->blocks = false;
      fireballScroll->pickable = new Fireball(3, 12);
      ::engine.actors.push_back(fireballScroll);
      ::engine.sendNonBlockingToBack(fireballScroll);
   } else {
      // create a scroll of confusion
      auto confusScroll = new Actor(
         x,
         y,
         '#',
         "scroll of confusion",
         A_BOLD |
            COLOR_PAIR(alloc_pair(COLOR_YELLOW, COLOR_BLACK)) );
      confusScroll->blocks = false;
      confusScroll->pickable = new Confuser(10, 8);
      ::engine.actors.push_back(confusScroll);
      ::engine.sendNonBlockingToBack(confusScroll);
   }
}
