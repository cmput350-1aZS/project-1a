#ifndef GAMECONTEXT_H
#define GAMECONTEXT_H

#include "DrawContext.h"
#include "EngineView.h"
#include "GameObject.h"

namespace CMPUT350 {

class GameContext {
public:
    EngineView *mEngineView;
    DrawContext *ScreenContext;  // Transformed per object: draw in the object's local coordinates
    DrawContext *GUIContext;     // Never transformed: draw in screen coordinates (score, text, UI)
};

}  // namespace CMPUT350

#endif  // GAMECONTEXT_H
