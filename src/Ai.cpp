#include <Actor.hpp>
#include <Ai.hpp>
#include <curses.h>
#undef hline
#undef vline
#include <main.hpp>
#include <math.h>

bool PlayerAi::update(Actor *owner) {
   if (owner->destructible && owner->destructible->isDead())
      return true; // should be `false` ?
   int dx = 0, dy = 0;
   auto ch = getch();
   switch (ch) {
   case 'q': return false;
   case KEY_MOUSE:
      MEVENT event;
      ok(getmouse(&event));
      ::engine.mouse = { .cx = event.x, .cy = event.y };
      break;
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
      if (actor->destructible &&
          actor->destructible->isDead() &&
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
      (void) this->moveOrAttack
         (owner, ::engine.player->x, ::engine.player->y);
   return true;
}

bool MonsterAi::moveOrAttack(Actor *owner, int targetx, int targety) {
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
         return true;
      } else if (abs(dx) > abs(dy)) {
         if (::engine.map->canWalk(owner->x + stepx, owner->y)) {
            owner->x += stepx;
            return true;
         } else if (::engine.map->canWalk(owner->x, owner->y + stepy))
         {
            owner->y += stepy;
            return true;
         }
      } else {
         if (::engine.map->canWalk(owner->x, owner->y + stepy)) {
            owner->y += stepy;
            return true;
         } else if (::engine.map->canWalk(owner->x + stepx, owner->y))
         {
            owner->x += stepx;
            return true;
         }
      }  
   } else if (owner->attacker &&
              (owner->x + dx) == (::engine).player->x &&
              (owner->y + dy) == (::engine).player->y)
      owner->attacker->attack(owner, ::engine.player);
   return false;
}
