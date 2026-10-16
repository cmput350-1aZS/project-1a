#ifndef GAMECONTEXT_H
#define GAMECONTEXT_H

#include "DrawContext.h"
#include "EngineView.h"
#include "GameObject.h"
#include <memory>

namespace CMPUT350 {

class NotificationManager;

class GameContext {
public:
    EngineView *mEngineView;
    DrawContext *ScreenContext;  // Transformed per object: draw in the object's local coordinates
    DrawContext *GUIContext;     // Never transformed: draw in screen coordinates (score, text, UI)
    NotificationManager *mNotificationManager;
    int currentFrame;
    std::weak_ptr<GameObject> CurrObject;
};

}  // namespace CMPUT350

#endif  // GAMECONTEXT_H