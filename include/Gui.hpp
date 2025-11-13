#pragma once

#include <limits.h>
#include <math.h>
#include <vector>

struct Gui {
   Gui(unsigned barCol);
   ~Gui();
   void render();
   void renderBar(
      int x,
      int y,
      const char *name,
      float value,
      float maxValue);
   void renderMouseLook();

   struct Msg {
      unsigned col = UINT_MAX; // color pair attr
      const char *fmt = NULL; // format string
      const char *ownName = NULL;
      const char *targName = NULL;
      float floatArg = NAN;
   };
   std::vector<Gui::Msg> msg; // current messages
   unsigned barCol = UINT_MAX; // color pair attr
};
