#pragma once

struct Ai {
   virtual bool update(Actor *owner) = 0;
   virtual ~Ai() {}
};

struct PlayerAi : Ai {
   bool moveOrAttack(Actor *owner, int targetx, int targety);
   bool update(Actor *owner) override;
   bool handleActionKey(Actor *owner, int ascii);
   Actor *choseFromInventory(Actor *owner);
};

struct MonsterAi : Ai {
   int moveCount;
   void moveOrAttack(Actor *owner, int targetx, int targety);
   bool update(Actor *owner) override;
   enum : int { TRACKING_TURNS = 3 };
};

struct ConfusedMonsterAi : Ai {
   ConfusedMonsterAi(int nbTurns, Ai *oldAi);
   bool update(Actor *owner) override;

   int nbTurns;
   Ai *oldAi;
};
