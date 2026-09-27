#include "GameObject.h"

namespace CMPUT350 {

void GameObject::Initialize(GameContext *context) { 
    
    return; 
}
void GameObject::Update(GameContext *context) { 
    return; 
}
void GameObject::LateUpdate(GameContext *context) { 
    return; 
}
void GameObject::RenderUI(GameContext *contextrender) { 
    return; }
bool GameObject::HandleKeyEvent(GameContext *context, char key) { 
    return false; 
}
bool GameObject::IsAlive() const { 
    return alive; 
}
void GameObject::Kill() {
    alive = false;
}
}  // namespace CMPUT350
