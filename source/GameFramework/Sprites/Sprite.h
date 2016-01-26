#pragma once

#include <string>
#include <vector>

#include "s3e.h"
#include "Iw2D.h"
#include "IwResManager.h"

#include "Constants.h"
#include "GameInterfaces.h"
#include "GameManager.h"
#include "ITransformation.h"
#include "AnimatableParameters.h"

class Sprite : public IGameObject, public ILocationSource, public IDirectionalSource
{
protected:
    int32 lifespanMillis;
    uint32 birthMillis;

    std::vector<ITransformation *> *transformations;
    CIw2DImage *image;
    CIwVec2 baseLocation;
    iwangle baseOrientation;
    iwangle baseTangent;
    iwangle baseNormal;
    iwfixed baseScale;
    CIwVec2 transformedLocation;
    iwangle transformedOrientation;
    iwangle transformedTangent;
    iwangle transformedNormal;
    iwfixed transformedScale;
    CIwVec2 hitBox[4];
    int32 zIndex;
    CIwMat2D currentTransformation;

    ILocationSource *baseLocationSource;

    EventCallback completionCallback;
    EventCallback touchStartCallback, touchMoveCallback, touchEndCallback;


    virtual void RenderImage(uint32 gameTime);

public:
    Sprite(GameManager *gameManager);
    ~Sprite();

    GameManager *GetManager();
    
    virtual void SetImage(CIw2DImage *newImage);
    virtual CIw2DImage *GetImage();

    virtual bool Active();

    virtual void SetLifespan(int32 lifespanMillis);
    virtual int32 GetLifespan();
    virtual int32 GetAge(uint32 gameTime);
    virtual int32 GetLifespanRemaining(uint32 gameTime);
    virtual void UpdateAge(uint32 gameTime);

    virtual CIwVec2 GetLocation(uint32 gameTime);
    virtual iwangle GetNormal(uint32 gameTime);
    virtual iwangle GetTangent(uint32 gameTime);
    virtual iwangle GetOrientation(uint32 gameTime);
    virtual iwfixed GetScale(uint32 gameTime);
    virtual int32 GetWidth();
    virtual int32 GetHeight();

    virtual CIwVec2 GetBaseLocation();
    virtual void SetBaseLocation(CIwVec2 location);
    virtual void SetBaseLocationSource(ILocationSource *locationSource);

    virtual void ClearTransformations();
    virtual void AddTransformation(ITransformation *newTransform);

    virtual bool HitTest(int16 x, int16 y);

    virtual void Stop();
    virtual void Kill();

    virtual int32 GetZIndex();
    virtual void SetZIndex(int32 newZIndex);

    virtual void PreUpdate(uint32 gameTime, uint32 timeSinceLastUpdate);
    virtual void Update(uint32 gameTime, uint32 timeSinceLastUpdate);
    virtual void PostUpdate(uint32 gameTime, uint32 timeSinceLastUpdate);

    virtual void PreRender(uint32 gameTime, uint32 timeSinceLastRender);
    virtual void Render(uint32 gameTime, uint32 timeSinceLastRender);
    virtual void PostRender(uint32 gameTime, uint32 timeSinceLastRender);

    virtual void TouchStart(uint16 x, uint16 y);
    virtual void TouchMove(uint16 x, uint16 y);
    virtual void TouchEnd(uint16 x, uint16 y);

    virtual void SetCompletionCallback(IGameObject *eventObject, EventCallbackPtr eventMethod, void *subscriberData = null);
    virtual void SetTouchStartCallback(IGameObject *eventObject, EventCallbackPtr eventMethod, void *subscriberData = null);
    virtual void SetTouchMoveCallback(IGameObject *eventObject, EventCallbackPtr eventMethod, void *subscriberData = null);
    virtual void SetTouchEndCallback(IGameObject *eventObject, EventCallbackPtr eventMethod, void *subscriberData = null);
};


