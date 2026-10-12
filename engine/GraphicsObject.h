#ifndef GRAPHICS_OBJECT_H
#define GRAPHICS_OBJECT_H

#include "GameObject.h"
#include "MathUtil.h"

namespace CMPUT350 {

class GameContext;

class GraphicsObject : public GameObject {
public:
    virtual void RenderBackground(GameContext *context);
    virtual void RenderForeground(GameContext *context);
    // Screen position of the object's local origin (0, 0); also the point rotations pivot around
    virtual Point2D GetLocation() const = 0;
    // Rotation about GetLocation(), in radians
    virtual float GetRotation() const = 0;

};

}  // namespace CMPUT350

#endif
