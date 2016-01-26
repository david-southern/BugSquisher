#include <vector>

#include "s3e.h"
#include "Iw2D.h"

#include "GameInterfaces.h"
#include "ITransformation.h"
#include "Sprite.h"

Sprite::Sprite(GameManager *gameManager) : IGameObject(gameManager), ILocationSource(false), IDirectionalSource(false)
{
    DEBUG_TRACER(Sprite::Sprite);
    lifespanMillis = -1;
    birthMillis = 0;
    transformations = new std::vector<ITransformation *>;
    image = null;
    baseLocation = CIwVec2(0, 0);
    baseLocationSource = null;
    baseOrientation= 0;
    baseTangent = 0;
    baseNormal = ANGLE_90_DEGREES;
    hitBox[0] = CIwVec2(0,0);
    hitBox[1] = CIwVec2(0,0);
    hitBox[2] = CIwVec2(0,0);
    hitBox[3] = CIwVec2(0,0);
    transformedLocation = baseLocation;
    transformedOrientation = 0;
    transformedTangent = 0;
    transformedNormal = ANGLE_90_DEGREES;
    transformedScale = IW_FIXED(1);
    zIndex = 0;
    parent = null;
    debugId[0] = '\0';
}

Sprite::~Sprite()
{
    if(baseLocationSource != null) delete baseLocationSource;

    DEBUG_TRACER(Sprite::~Sprite);
    ClearTransformations();
    delete transformations;
}

GameManager *Sprite::GetManager()
{
    return mgr;
}

void Sprite::SetImage(CIw2DImage *newImage)
{
    image = newImage;
}

CIw2DImage *Sprite::GetImage()
{
    return image;
}

CIwVec2 Sprite::GetLocation(uint32 gameTime)
{
    return transformedLocation;
}

iwangle Sprite::GetNormal(uint32 gameTime)
{
    return transformedNormal;
}

iwangle Sprite::GetTangent(uint32 gameTime)
{
    return transformedTangent;
}

iwangle Sprite::GetOrientation(uint32 gameTime)
{
    return transformedOrientation;
}

iwfixed Sprite::GetScale(uint32 gameTime)
{
    return transformedScale;
}

int32 Sprite::GetWidth()
{
    return image == null ? 0 : image->GetWidth();
}

int32 Sprite::GetHeight()
{
    return image == null ? 0 : image->GetHeight();
}

CIwVec2 Sprite::GetBaseLocation()
{
    return SUBPIXEL_TO_SCREEN(baseLocation);
}

void Sprite::SetBaseLocation(CIwVec2 location)
{
    baseLocation = SCREEN_TO_SUBPIXEL(location);
    SetBaseLocationSource(null);
}

void Sprite::SetBaseLocationSource(ILocationSource *locationSource)
{
    if(baseLocationSource != null)
    {
        delete baseLocationSource;
    }
    baseLocationSource = locationSource;
}

bool Sprite::Active()
{
    if(birthMillis == 0 || lifespanMillis == 0)
    {
        return false;
    }

    return true;
}

void Sprite::SetLifespan(int32 lifespanMillis)
{
    this->lifespanMillis = lifespanMillis;
}

int32 Sprite::GetLifespan()
{
    return lifespanMillis;
}

void Sprite::UpdateAge(uint32 gameTime)
{
    if(lifespanMillis == 0)
    {
        return;
    }

    if(birthMillis == 0)
    {
        birthMillis = gameTime;
    }

    if(GetLifespanRemaining(gameTime) <= 0)
    {
        Stop();
        return;
    }
}

int32 Sprite::GetAge(uint32 gameTime)
{
    if(!Active())
    {
        return 0;
    }

    return gameTime - birthMillis;
}

int32 Sprite::GetLifespanRemaining(uint32 gameTime)
{
    if(!Active())
    {
        return 0;
    }

    if(lifespanMillis == -1)
    {
        return 1;
    }

    return lifespanMillis - GetAge(gameTime);
}

int32 Sprite::GetZIndex()
{
    return zIndex;
}

void Sprite::SetZIndex(int32 newZIndex)
{
    if(zIndex != newZIndex)
    {
        zIndex = newZIndex;
        mgr->NotifyZIndexChanged();
    }
}

void Sprite::SetCompletionCallback(IGameObject *eventObject, EventCallbackPtr eventMethod, void *subscriberData)
{
    completionCallback.eventObject = eventObject;
    completionCallback.eventMethod = eventMethod;
    completionCallback.subscriberData = subscriberData;
}

void Sprite::ClearTransformations()
{
    for(uint32 transIndex = 0; transIndex < transformations->size(); transIndex++)
    {
        delete transformations->at(transIndex);
    }

    transformations->clear();
}

void Sprite::AddTransformation(ITransformation *newTransform)
{
    transformations->push_back(newTransform);
}

bool Sprite::HitTest(int16 x, int16 y)
{
    DEBUG_TRACER(Sprite::HitTest);
    CIwVec2 p(x, y);

    p = SCREEN_TO_SUBPIXEL(p);

    CIwVec2 p1, p2;

    bool inside = false;

#define HIT_POLY_COUNT 4

    CIwVec2 oldPoint = hitBox[HIT_POLY_COUNT - 1];

    for (int i = 0; i < HIT_POLY_COUNT; i++)
    {
        CIwVec2 newPoint = hitBox[i];

        if (newPoint.x > oldPoint.x)
        {
            p1 = oldPoint;
            p2 = newPoint;
        }
        else
        {
            p1 = newPoint;
            p2 = oldPoint;
        }

        if ((newPoint.x < p.x) == (p.x <= oldPoint.x)
            && ((long)p.y - (long)p1.y) * (long)(p2.x - p1.x) < ((long)p2.y - (long)p1.y) * (long)(p.x - p1.x))
        {
            inside = !inside;
        }

        oldPoint = newPoint;
    }

    return inside;
}

void Sprite::Stop()
{
    DEBUG_TRACER(Sprite::Stop);

    // Make sure to call Kill regardless of whether the sprite is currently active or not
    // This will make sure that if we Stop() a sprite before its first Update() then it still stops.
    // Remember if it was Active() in order to call the completionCallback if required
    bool wasActive = Active();

    Kill();

    if(wasActive)
    {
        completionCallback.Dispatch(mgr->GetCurrentClockTime());
    }
}

void Sprite::Kill()
{
    DEBUG_TRACER(Sprite::Kill);
    lifespanMillis = 0;
    birthMillis = 0;
}

void Sprite::PreUpdate(uint32 gameTime, uint32 timeSinceLastUpdate)
{
}

void Sprite::Update(uint32 gameTime, uint32 timeSinceLastUpdate)
{
    DEBUG_TRACER(Sprite::Update);

    UpdateAge(gameTime);

    if(!Active())
    {
        return;
    }

    TransformationState state;

    Sprite *spriteParent = (Sprite *)parent;

    if(baseLocationSource != null) baseLocation = SCREEN_TO_SUBPIXEL(baseLocationSource->GetLocation(gameTime));

    state.gameTime = gameTime;
    state.sprite = this;
    state.anchorPoint = CIwVec2(0, 0);
    state.originalLocation = spriteParent == null ? baseLocation : spriteParent->GetLocation(gameTime);
    state.currentLocation = state.originalLocation;
    state.orientation = spriteParent == null ? baseOrientation : spriteParent->GetOrientation(gameTime);
    state.normal = spriteParent == null ? baseNormal : spriteParent->GetNormal(gameTime);
    state.tangent = spriteParent == null ? baseTangent : spriteParent->GetTangent(gameTime);
    state.scaleFactor = spriteParent == null ? IW_FIXED(1) : spriteParent->GetScale(gameTime);

    hitBox[0] = CIwVec2(0,0);
    hitBox[1] = SCREEN_TO_SUBPIXEL(CIwVec2(0, GetHeight()));
    hitBox[2] = SCREEN_TO_SUBPIXEL(CIwVec2(GetWidth(), GetHeight()));
    hitBox[3] = SCREEN_TO_SUBPIXEL(CIwVec2(GetWidth(), 0));

    for(uint32 transIndex = 0; transIndex < transformations->size(); transIndex++)
    {
        transformations->at(transIndex)->ApplyTransformation(state);
    }

    CIwMat2D tmp;

    currentTransformation.SetIdentity();
    currentTransformation.SetTrans(-state.anchorPoint);
    tmp.SetIdentity();
    tmp.SetRot(state.orientation);
    currentTransformation = currentTransformation * tmp;
    tmp.SetIdentity();
    tmp.Scale(state.scaleFactor);
    currentTransformation = currentTransformation * tmp;
    tmp.SetIdentity();
    tmp.SetTrans(state.anchorPoint);
    currentTransformation = currentTransformation * tmp;
    tmp.SetIdentity();
    tmp.SetTrans(state.currentLocation - state.anchorPoint);
    currentTransformation = currentTransformation * tmp;

    transformedLocation = currentTransformation.TransformVec(state.anchorPoint);
    transformedOrientation = state.orientation;
    transformedTangent = state.tangent;
    transformedNormal = state.normal;
    transformedScale = state.scaleFactor;

    hitBox[0] = currentTransformation.TransformVec(hitBox[0]);
    hitBox[1] = currentTransformation.TransformVec(hitBox[1]);
    hitBox[2] = currentTransformation.TransformVec(hitBox[2]);
    hitBox[3] = currentTransformation.TransformVec(hitBox[3]);

#ifdef DO_DEBUG_TRACING
    s3eDebugTracePrintf("Updating transformations for sprite %s, with parent %s, setting location to: %d, %d", this->debugId, 
        spriteParent == null ? "NULL" : spriteParent->debugId, transformedLocation.x, transformedLocation.y);
#endif
}

void Sprite::PostUpdate(uint32 gameTime, uint32 timeSinceLastUpdate)
{
}

void Sprite::PreRender(uint32 gameTime, uint32 timeSinceLastRender)
{
}

void Sprite::Render(uint32 gameTime, uint32 timeSinceLastRender)
{
    DEBUG_TRACER(Sprite::Render);
    if(!Active())
    {
        return;
    }

#ifdef DO_DEBUG_TRACING
    Sprite *spriteParent = (Sprite*)parent;
    s3eDebugTracePrintf("Rendering sprite %s, with parent %s, at: %d, %d", this->debugId, 
        spriteParent == null ? "NULL" : spriteParent->debugId, transformedLocation.x, transformedLocation.y);
#endif

    Iw2DSetTransformMatrixSubPixel(currentTransformation);

    RenderImage(gameTime);

    Iw2DSetTransformMatrixSubPixel(CIwMat2D::g_Identity);

    //Iw2DSetColour(0xff0000ff);

    //CIwVec2 size(GetWidth(), GetHeight());
    //size = SCREEN_TO_SUBPIXEL(size);
    //CIwSVec2 sizeS(size);
    //Iw2DDrawRect(CIwSVec2(baseLocation), sizeS);

    //Iw2DSetColour(0xffff0000);
    //
    //Iw2DDrawLine(CIwSVec2(hitBox[0]), CIwSVec2(hitBox[1]));
    //Iw2DDrawLine(CIwSVec2(hitBox[1]), CIwSVec2(hitBox[2]));
    //Iw2DDrawLine(CIwSVec2(hitBox[2]), CIwSVec2(hitBox[3]));
    //Iw2DDrawLine(CIwSVec2(hitBox[3]), CIwSVec2(hitBox[0]));


    ////Iw2DSetColour(0xffff0000);
    ////Iw2DDrawLine(location - CIwVec2(100, 0),location + CIwVec2(100, 0));

    ////Iw2DDrawArc(location, CIwVec2(200, 200), 0, IW_ANGLE_2PI, 42);
    ////Iw2DDrawLine(location - CIwVec2(0, 100),location + CIwVec2(0, 100));

    //Iw2DSetColour(0xffffffff);
}

void Sprite::RenderImage(uint32 gameTime)
{
    DEBUG_TRACER(RenderImage);
    if(image != null)
    {
        Iw2DDrawImage(image, CIwSVec2(0, 0));
    }
}


void Sprite::PostRender(uint32 gameTime, uint32 timeSinceLastRender)
{
}


void Sprite::TouchStart(uint16 x, uint16 y)
{
    DEBUG_TRACER(Sprite::TouchStart);
    touchStartCallback.Dispatch(mgr->GetCurrentClockTime());
}

void Sprite::TouchMove(uint16 x, uint16 y)
{
    DEBUG_TRACER(Sprite::TouchMove);
    touchMoveCallback.Dispatch(mgr->GetCurrentClockTime());
}

void Sprite::TouchEnd(uint16 x, uint16 y)
{
    DEBUG_TRACER(Sprite::TouchEnd);
    touchEndCallback.Dispatch(mgr->GetCurrentClockTime());
}

void Sprite::SetTouchStartCallback(IGameObject *eventObject, EventCallbackPtr eventMethod, void *subscriberData)
{
    touchStartCallback.eventObject = eventObject;
    touchStartCallback.eventMethod = eventMethod;
    touchStartCallback.subscriberData = subscriberData;
}

void Sprite::SetTouchMoveCallback(IGameObject *eventObject, EventCallbackPtr eventMethod, void *subscriberData)
{
    touchMoveCallback.eventObject = eventObject;
    touchMoveCallback.eventMethod = eventMethod;
    touchMoveCallback.subscriberData = subscriberData;
}

void Sprite::SetTouchEndCallback(IGameObject *eventObject, EventCallbackPtr eventMethod, void *subscriberData)
{
    touchEndCallback.eventObject = eventObject;
    touchEndCallback.eventMethod = eventMethod;
    touchEndCallback.subscriberData = subscriberData;
}

