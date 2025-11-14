#pragma once

struct Ai {
   virtual bool moveOrAttack(
      Actor *owner,
      int targetx,
      int targety) = 0;
   virtual bool update(Actor *owner) = 0;
   virtual ~Ai() {}
};

struct PlayerAi : Ai {
   bool moveOrAttack(Actor *owner, int targetx, int targety) override;
   bool update(Actor *owner) override;
   bool handleActionKey(Actor *owner, int ascii);
   Actor *choseFromInventory(Actor *owner);
};

struct MonsterAi : Ai {
   int moveCount;
   bool moveOrAttack(Actor *owner, int targetx, int targety) override;
   bool update(Actor *owner) override;
   enum : int { TRACKING_TURNS = 3 };
};
