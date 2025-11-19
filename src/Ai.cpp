#include <Actor.hpp>
#include <Ai.hpp>
#include <curses.h>
#undef hline
#undef vline
#include <main.hpp>
#include <math.h>

Actor *PlayerAi::choseFromInventory(Actor *owner) {   
   ok(wclear(::engine.gui->inventoryWin));
   auto shortcut = 'a';
   auto y = 1;
   auto x = 2;
   for (auto actor : owner->container->inventory) {
      mvwprintw(
         ::engine.gui->inventoryWin,
         y,
         x,
         "(%c) %s",
         shortcut,
         actor->name);
      y++;
      if (y == 14) {
         y = 1;
         x += COLS / 4;
      }
      shortcut++;
   }
   ok(wrefresh(::engine.gui->inventoryWin));

   // wait for a key press
   ::engine.lastKey = getch();
   unsigned actorIndex = ::engine.lastKey - 'a';
   if (actorIndex >= 0 &&
       actorIndex < owner->container->inventory.size())
      return owner->container->inventory[actorIndex];
   return NULL;
}

bool PlayerAi::handleActionKey(Actor *owner, int ascii) {
   switch (ascii) {
   case 'g':
      ::engine.gameStatus = Engine::NEW_TURN;
      for (auto actor : ::engine.actors)
         if (actor->pickable &&
             actor->x == owner->x &&
             actor->y == owner->y)
         {
            if (actor->pickable->pick(actor, owner)) {
               ::engine.gui->msg.push_back({
                  .col = A_DIM,
                  .fmt = "You pick up the %s.",
                  .ownName = actor->name });
               return true;
            } else {
               ::engine.gui->msg.push_back({
                  .col =
                     COLOR_PAIR(alloc_pair(COLOR_RED, COLOR_BLACK)),
                  .fmt = "Your inventory is full." });
               return true;
            }
         }
      ::engine.gui->msg.push_back({
         .col = A_DIM,
         .fmt = "There's nothing here that you can pick up." });
      break;
   case 'i': // display inventory
      {
         auto actor = this->choseFromInventory(owner);
         if (actor) {
            actor->pickable->use(actor, owner);
            if (::engine.gameStatus != Engine::DEFEAT)
               ::engine.gameStatus = Engine::NEW_TURN;
         }
      }
      break;
   case 'd': // drop item
      {
         auto actor = this->choseFromInventory(owner);
         if (actor) {
            actor->pickable->drop(actor, owner);
            if (::engine.gameStatus != Engine::DEFEAT)
               ::engine.gameStatus = Engine::NEW_TURN;
         }
      }
      break;
   }
   return true;
}

bool PlayerAi::update(Actor *owner) {
   int dx = 0, dy = 0;
   auto ch = getch();
   defer( ::engine.lastKey = ch );
   if ('q' == ch || 'q' == ::engine.lastKey) return false;
   if (owner->destructible && owner->destructible->isDead())
      return true; // should be `false` ?
   switch (ch) {
   case KEY_MOUSE:
      MEVENT event;
      if (OK != getmouse(&event)) return true;
      ::engine.mouse = { .cx = event.x, .cy = event.y };
      return true;
   case KEY_UP:
      dy -= 1;
      break;
   case KEY_DOWN:
      dy += 1;
      break;
   case KEY_LEFT:
      dx -= 1;
      break;
   case KEY_RIGHT:
      dx += 1;
      break;
   default:
      return this->handleActionKey(owner, ch);
   }
   if (dx != 0 || dy != 0) {
      ::engine.gameStatus = Engine::NEW_TURN;
      if (this->moveOrAttack(
          owner,
          owner->x + dx,
          owner->y + dy)) ::engine.map->computeFov();
   }
   return true;
}

bool PlayerAi::moveOrAttack(Actor *owner, int targetx, int targety) {
   if (::engine.map->isWall(targetx, targety)) return false;
   // look for living actors to attack
   for (auto actor : ::engine.actors)
      if (actor->destructible &&
          !actor->destructible->isDead() &&
          actor->x == targetx &&
          actor->y == targety)
      {
         owner->attacker->attack(owner, actor);
         return false;
      }

   // look for corpses
   for (auto actor : ::engine.actors)
      if ((actor->pickable || (actor->destructible &&
          actor->destructible->isDead())) &&
          actor->x == targetx &&
          actor->y == targety)
         ::engine.gui->msg.push_back({
            .col = A_DIM,
            .fmt = "There's a %s here",
            .ownName = actor->name});

   owner->x = targetx;
   owner->y = targety;
   return true;
}

bool MonsterAi::update(Actor *owner) {
   if (owner->destructible && owner->destructible->isDead())
      return true;
   if (::engine.map->isInFov(owner->x, owner->y))
      // we can see the player. move towards him
      this->moveCount = MonsterAi::TRACKING_TURNS;
   else this->moveCount -= 1;
   if (this->moveCount > 0)
      this->moveOrAttack
         (owner, ::engine.player->x, ::engine.player->y);
   return true;
}

void MonsterAi::moveOrAttack(Actor *owner, int targetx, int targety) {
   auto dx = targetx - owner->x;
   auto stepx = (dx > 0) ? 1 : -1;
   auto dy = targety - owner->y;
   auto stepy = (dy > 0) ? 1 : -1;
   auto distance = sqrtf(dx*dx + dy*dy);
   if (distance >= 2) {
      dx = (int) round(dx / distance);
      dy = (int) round(dy / distance);
      if (::engine.map->canWalk(owner->x + dx, owner->y + dy)) {
         owner->x += dx;
         owner->y += dy;
         return;
      } else if (abs(dx) > abs(dy)) {
         if (::engine.map->canWalk(owner->x + stepx, owner->y)) {
            owner->x += stepx;
            return;
         } else if (::engine.map->canWalk(owner->x, owner->y + stepy))
         {
            owner->y += stepy;
            return;
         }
      } else {
         if (::engine.map->canWalk(owner->x, owner->y + stepy)) {
            owner->y += stepy;
            return;
         } else if (::engine.map->canWalk(owner->x + stepx, owner->y))
         {
            owner->x += stepx;
            return;
         }
      }  
   } else if (owner->attacker &&
              (owner->x + dx) == (::engine).player->x &&
              (owner->y + dy) == (::engine).player->y)
      owner->attacker->attack(owner, ::engine.player);
}

ConfusedMonsterAi::ConfusedMonsterAi(int nbTurns, Ai *oldAi)
   : nbTurns(nbTurns), oldAi(oldAi) {}

bool ConfusedMonsterAi::update(Actor *owner) {
   auto rng = TCODRandom::getInstance();
   auto dx = rng->getInt(-1, 1);
   auto dy = rng->getInt(-1, 1);
   if (dx != 0 || dy != 0) {
      auto destx = owner->x + dx;
      auto desty = owner->y + dy;
      if (::engine.map->canWalk(destx, desty)) {
         owner->x = destx;
         owner->y = desty;
      } else {
         auto actor = ::engine.getActor(destx, desty);
         if (actor) owner->attacker->attack(owner, actor);
      }
   }
   nbTurns--;
   if (nbTurns == 0) {
      owner->ai = oldAi;
      owner->col &= ~A_REVERSE;
      delete this;
   }
   return true;
}
