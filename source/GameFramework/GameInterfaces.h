#pragma once

#include <stdio.h>
#include "Constants.h"

class GameManager;

class IGameObject
{
    static int NEXT_GAME_OBJECT_INDEX;

protected:
    GameManager *mgr;
    IGameObject *parent;
    bool needsUpdate;

    IGameObject(GameManager *_mgr)
    {
        mgr = _mgr;
        parent = null;
        needsUpdate = false;
        sprintf(debugId, "GameObject %d", NEXT_GAME_OBJECT_INDEX++);
    }

public:
    virtual ~IGameObject() {}

    virtual bool Active() = 0;
    virtual int32 GetZIndex() = 0;

    virtual void SetParent(IGameObject *parent)
    {
        this->parent = parent;
    }

    virtual IGameObject *GetParent()
    {
        return this->parent;
    }

    virtual bool GetNeedsUpdate()
    {
        return needsUpdate;
    }

    virtual void SetNeedsUpdate(bool needsUpdate)
    {
        this->needsUpdate = needsUpdate;
    }

    virtual void PreUpdate(uint32 gameTime, uint32 timeSinceLastUpdate) = 0;
    virtual void Update(uint32 gameTime, uint32 timeSinceLastUpdate) = 0;
    virtual void PostUpdate(uint32 gameTime, uint32 timeSinceLastUpdate) = 0;

    virtual void PreRender(uint32 gameTime, uint32 timeSinceLastRender) = 0;
    virtual void Render(uint32 gameTime, uint32 timeSinceLastRender) = 0;
    virtual void PostRender(uint32 gameTime, uint32 timeSinceLastRender) = 0;

    virtual bool HitTest(int16 x, int16 y) = 0;

    virtual void TouchStart(uint16 x, uint16 y) = 0;
    virtual void TouchMove(uint16 x, uint16 y) = 0;
    virtual void TouchEnd(uint16 x, uint16 y) = 0;

    char debugId[50];

    friend class GameManager;
};

// This class implelements "do nothing" versions of all the abstract functions of IGameObject.  It is intended
// for classes that want to receive game events, but do not actually need to be game objects.
class EventTarget : public IGameObject
{
protected:
    EventTarget(GameManager *mgr) : IGameObject(mgr)
    {
    }

public:
    virtual ~EventTarget() {}

    virtual bool Active() { return false; }
    virtual int32 GetZIndex() { return 0; }

    virtual void PreUpdate(uint32 gameTime, uint32 timeSinceLastUpdate) { }
    virtual void Update(uint32 gameTime, uint32 timeSinceLastUpdate) { }
    virtual void PostUpdate(uint32 gameTime, uint32 timeSinceLastUpdate) { }

    virtual void PreRender(uint32 gameTime, uint32 timeSinceLastRender) { }
    virtual void Render(uint32 gameTime, uint32 timeSinceLastRender) { }
    virtual void PostRender(uint32 gameTime, uint32 timeSinceLastRender) { }

    virtual bool HitTest(int16 x, int16 y) { return false; }

    virtual void TouchStart(uint16 x, uint16 y) { }
    virtual void TouchMove(uint16 x, uint16 y) { }
    virtual void TouchEnd(uint16 x, uint16 y) { }
    
    friend class GameManager;
};

