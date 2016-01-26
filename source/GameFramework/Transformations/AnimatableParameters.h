#pragma once

#include "s3e.h"
#include "Iw2D.h"
#include "ITransformation.h"

class ILocationSource
{
public:
    ILocationSource(bool _needsDelete = true) : locSourceNeedsDelete(_needsDelete)
    {
    }

    virtual ~ILocationSource() {}

    virtual CIwVec2 GetLocation(uint32 gameTime) = 0;

    bool locSourceNeedsDelete;
};

class IDirectionalSource
{
public:
    IDirectionalSource(bool _needsDelete = true) : dirSourceNeedsDelete(_needsDelete)
    {
    }

    virtual ~IDirectionalSource() {}

    virtual iwangle GetNormal(uint32 gameTime) = 0;
    virtual iwangle GetTangent(uint32 gameTime) = 0;
    virtual iwangle GetOrientation(uint32 gameTime) = 0;

    bool dirSourceNeedsDelete;
};

class IFloatSource
{
public:
    virtual ~IFloatSource() {}

    virtual float GetFloat(uint32 gameTime) = 0;
};

class IIntSource
{
public:
    virtual ~IIntSource() {}

    virtual int GetInt(uint32 gameTime) = 0;
};

class IColorSource
{
public:
    virtual ~IColorSource() {}

    virtual CIwColour GetColor(uint32 gameTime) = 0;
};

class LocationSourceConstant : public ILocationSource
{
    CIwVec2 location;

public:
    LocationSourceConstant(CIwVec2 location) : ILocationSource() { this->location = location; }

    virtual CIwVec2 GetLocation(uint32 gameTime) { return location; }
};

class DirectionalSourceConstant : public IDirectionalSource
{
    iwangle direction;
    iwangle tangent;

public:
    DirectionalSourceConstant(iwangle direction, iwangle tangent) : IDirectionalSource()
    {
        this->direction = direction;
        this->tangent = tangent;
    }

    virtual iwangle GetNormal(uint32 gameTime) { return direction; }
    virtual iwangle GetTangent(uint32 gameTime) { return tangent; }
    virtual iwangle GetOrientation(uint32 gameTime) {return direction; }
};

class FloatSourceConstant : public IFloatSource
{
    float value;

public:
    FloatSourceConstant(float value)
    {
        this->value = value;
    }

    virtual float GetFloat(uint32 gameTime) { return value; }
};

class IntSourceConstant : public IIntSource
{
    int value;

public:
    IntSourceConstant(int value) { this->value = value; }

    virtual int GetInt(uint32 gameTime) { return value; }
};

class ColorSourceConstant : public IColorSource
{
    CIwColour value;

public:
    ColorSourceConstant(CIwColour value) { this->value = value; }

    virtual CIwColour GetColor(uint32 gameTime) { return value; }
};

enum EasingFunction
{
    EasingFunctionLinear,
    EasingFunctionSine, 
    EasingFunctionQuadratic, 
    EasingFunctionCubic, 
    EasingFunctionQuartic, 
    EasingFunctionQuintic, 
    EasingFunctionExponential, 
    EasingFunctionElastic, 
    EasingFunctionCircular, 
    EasingFunctionBounce,
    EasingFunctionBack
};

enum EasingDirection
{
    EasingDirectionIn,
    EasingDirectionOut,
    EasingDirectionInOut,
    EasingDirectionOutIn
};

class EasingSettings
{
    uint32 durationMillis;
    uint32 birthdayMillis;
    EasingFunction easingFunction;
    EasingDirection easingDirection;

public:
    EasingSettings(uint32 _durationMillis, EasingFunction _easingFunction, EasingDirection _easingDirection);
    float GetEasingFactor(uint32 gameTime);

    float amplitude;
    float period;
};

class LocationSourceDualTween : public ILocationSource
{
    IIntSource *xSource, *ySource;

public:
    LocationSourceDualTween(IIntSource *_xSource, IIntSource *_ySource);
    ~LocationSourceDualTween();
    virtual CIwVec2 GetLocation(uint32 gameTime);
};


class LocationSourceTween : public ILocationSource
{
    CIwFVec2 start;
    CIwFVec2 deltaLocation;
    EasingSettings settings;

public:
    LocationSourceTween(CIwVec2 _start, CIwVec2 _end, EasingSettings _settings);
    virtual CIwVec2 GetLocation(uint32 gameTime);
};

class DirectionalSourceTween : public IDirectionalSource
{
    iwangle startDirection;
    iwangle deltaDirection;
    iwangle deltaTangent;
    iwangle curDirection, curTangent;
    EasingSettings settings;
    uint32 lastUpdate;

    void UpdateAngles(uint32 gameTime);

public:
    DirectionalSourceTween(iwangle _startDirection, iwangle _endDirection, iwangle _startTangent, EasingSettings _settings);

    virtual iwangle GetNormal(uint32 gameTime);
    virtual iwangle GetTangent(uint32 gameTime);
    virtual iwangle GetOrientation(uint32 gameTime);
};

class FloatSourceTween : public IFloatSource
{
    float startValue, deltaValue;
    EasingSettings settings;

public:
    FloatSourceTween(float _startValue, float _endValue, EasingSettings _settings);

    virtual float GetFloat(uint32 gameTime);
};

class IntSourceTween : public IIntSource
{
    float startValue, deltaValue;
    EasingSettings settings;

public:
    IntSourceTween(int _startValue, int _endValue, EasingSettings _settings);

    virtual int GetInt(uint32 gameTime);
};

class ColorSourceTween : public IColorSource
{
    float startA, startR, startG, startB;
    float deltaA, deltaR, deltaG, deltaB;
    EasingSettings settings;

public:
    ColorSourceTween(CIwColour _startValue, CIwColour _endValue, EasingSettings _settings);

    virtual CIwColour GetColor(uint32 gameTime);
};

