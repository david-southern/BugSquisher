#include "cmath"

#include "s3e.h"
#include "Iw2D.h"

#include "AnimatableParameters.h"

/**
* Code for these easing functions was ported from http://code.google.com/p/tweener/
*
* @param t		Current time (in frames or seconds).
* @param b		Starting value.
* @param c		Change needed in value.
* @param d		Expected easing duration (in frames or seconds).
* @param a		Amplitude.
* @param p		Period.
* @param s		Overshoot ammount: higher s means greater overshoot (0 produces cubic easing with no overshoot, and the default value of 1.70158 produces an overshoot of 10 percent).
* @return		The correct value.
*/

float easeInQuad (float t, float b, float c, float d)
{
    t/=d;

    return c*(t)*t + b;
}

float easeOutQuad (float t, float b, float c, float d)
{
    t/=d;
    return -c *(t)*(t-2) + b;
}

float easeInOutQuad (float t, float b, float c, float d)
{
    t/=d/2;
    if ((t) < 1) return c/2*t*t + b;
    t--;
    return -c/2 * (t*(t-2) - 1) + b;
}

float easeOutInQuad (float t, float b, float c, float d)
{
    if (t < d/2) return easeOutQuad (t*2, b, c/2, d);
    return easeInQuad((t*2)-d, b+c/2, c/2, d);
}

float easeInCubic (float t, float b, float c, float d)
{
    t/=d;
    return c*(t)*t*t + b;
}

float easeOutCubic (float t, float b, float c, float d)
{
    t=t/d-1;
    return c*((t)*t*t + 1) + b;
}

float easeInOutCubic (float t, float b, float c, float d)
{
    t/=d/2;
    t-=2;
    if ((t) < 1) return c/2*t*t*t + b;
    return c/2*((t)*t*t + 2) + b;
}

float easeOutInCubic (float t, float b, float c, float d)
{
    if (t < d/2) return easeOutCubic (t*2, b, c/2, d);
    return easeInCubic((t*2)-d, b+c/2, c/2, d);
}

float easeInQuart (float t, float b, float c, float d)
{
    t/=d;
    return c*(t)*t*t*t + b;
}

float easeOutQuart (float t, float b, float c, float d)
{
    t=t/d-1;
    return -c * ((t)*t*t*t - 1) + b;
}

float easeInOutQuart (float t, float b, float c, float d)
{
    t/=d/2;
    if ((t) < 1) return c/2*t*t*t*t + b;

    t-=2;
    return -c/2 * ((t)*t*t*t - 2) + b;
}

float easeOutInQuart (float t, float b, float c, float d)
{
    if (t < d/2) return easeOutQuart (t*2, b, c/2, d);
    return easeInQuart((t*2)-d, b+c/2, c/2, d);
}

float easeInQuint (float t, float b, float c, float d)
{
    t/=d;
    return c*(t)*t*t*t*t + b;
}

float easeOutQuint (float t, float b, float c, float d)
{
    t=t/d-1;
    return c*((t)*t*t*t*t + 1) + b;
}

float easeInOutQuint (float t, float b, float c, float d)
{
    t/=d/2;
    if ((t) < 1) return c/2*t*t*t*t*t + b;
    t-=2;
    return c/2*((t)*t*t*t*t + 2) + b;
}

float easeOutInQuint (float t, float b, float c, float d)
{
    if (t < d/2) return easeOutQuint (t*2, b, c/2, d);
    return easeInQuint((t*2)-d, b+c/2, c/2, d);
}

float easeInSine (float t, float b, float c, float d)
{
    return -c * (float)cos(t/d * (PI/2)) + c + b;
}

float easeOutSine (float t, float b, float c, float d)
{
    return c * (float)sin(t/d * (PI/2)) + b;
}

float easeInOutSine (float t, float b, float c, float d)
{
    return -c/2 * ((float)cos(PI*t/d) - 1) + b;
}

float easeOutInSine (float t, float b, float c, float d)
{
    if (t < d/2) return easeOutSine (t*2, b, c/2, d);
    return easeInSine((t*2)-d, b+c/2, c/2, d);
}

float easeInExpo (float t, float b, float c, float d)
{
    return (t==0) ? b : c * (float)pow(2, 10 * (t/d - 1)) + b - c * 0.001f;
}

float easeOutExpo (float t, float b, float c, float d)
{
    return (t==d) ? b+c : c * 1.001f * (-(float)pow(2, -10 * t/d) + 1) + b;
}

float easeInOutExpo (float t, float b, float c, float d)
{
    if (t==0) return b;
    if (t==d) return b+c;
    t/=d/2;
    if ((t) < 1) return c/2 * (float)pow(2, 10 * (t - 1)) + b - c * 0.0005f;
    return c/2 * 1.0005f * (-(float)pow(2, -10 * --t) + 2) + b;
}

float easeOutInExpo (float t, float b, float c, float d)
{
    if (t < d/2) return easeOutExpo (t*2, b, c/2, d);
    return easeInExpo((t*2)-d, b+c/2, c/2, d);
}

float easeInCirc (float t, float b, float c, float d)
{
    t/=d;

    return -c * ((float)sqrt(1 - (t)*t) - 1) + b;
}

float easeOutCirc (float t, float b, float c, float d)
{
    t=t/d-1;
    return c * (float)sqrt(1 - (t)*t) + b;
}

float easeInOutCirc (float t, float b, float c, float d)
{
    t/=d/2;
    if ((t) < 1) return -c/2 * ((float)sqrt(1 - t*t) - 1) + b;

    t-=2;
    return c/2 * ((float)sqrt(1 - (t)*t) + 1) + b;
}

float easeOutInCirc (float t, float b, float c, float d)
{
    if (t < d/2) return easeOutCirc (t*2, b, c/2, d);
    return easeInCirc((t*2)-d, b+c/2, c/2, d);
}

float easeInElastic (float t, float b, float c, float d, float p, float a)
{
    if (t==0) return b;
    t/=d;
    if ((t)==1) return b+c;

    float s;
    if(p <= 0) p = d * 0.3f;
    if (a <= 0 || a < fabs(c)) {
        a = c;
        s = p/4;
    } else {
        s = p/(2*PI) * (float)asin(c/a);
    }

    t-=1;
    return -(a*(float)pow(2,10*(t)) * (float)sin( (t*d-s)*(2*PI)/p )) + b;
}

float easeOutElastic (float t, float b, float c, float d, float p, float a)
{
    if (t==0) return b;
    t/=d;
    if ((t)==1) return b+c;
    float s;
    if(p <= 0) p = d * 0.3f;
    if (a <= 0 || a < fabs(c)) {
        a = c;
        s = p/4;
    } else {
        s = p/(2*PI) * (float)asin(c/a);
    }
    return (a*(float)pow(2,-10*t) * (float)sin( (t*d-s)*(2*PI)/p ) + c + b);
}

float easeInOutElastic (float t, float b, float c, float d, float p, float a)
{
    if (t==0) return b;
    t/=d/2;
    if ((t)==2) return b+c;
    float s;
    if(p <= 0) p = d * 0.3f;
    if (a <= 0 || a < fabs(c)) {
        a = c;
        s = p/4;
    } else {
        s = p/(2*PI) * (float)asin(c/a);
    }
    t-=1;
    if (t < 1) return -.5f*(a*(float)pow(2,10*(t)) * (float)sin( (t*d-s)*(2*PI)/p )) + b;
    return a*(float)pow(2,-10*(t)) * (float)sin( (t*d-s)*(2*PI)/p )*.5f + c + b;
}

float easeOutInElastic (float t, float b, float c, float d, float p, float a)
{
    if (t < d/2) return easeOutElastic (t*2, b, c/2, d, p, a);
    return easeInElastic((t*2)-d, b+c/2, c/2, d, p, a);
}

float easeInBack (float t, float b, float c, float d, float s)
{
    if(s <= 0) s = 1.70158f;
    t/=d;
    return c*(t)*t*((s+1)*t - s) + b;
}

float easeOutBack (float t, float b, float c, float d, float s)
{
    if(s <= 0) s = 1.70158f;
    t=t/d-1;
    return c*((t)*t*((s+1)*t + s) + 1) + b;
}

float easeInOutBack (float t, float b, float c, float d, float s)
{
    if(s <= 0) s = 1.70158f;
    s*=(1.525f);

    t/=d/2;
    if ((t) < 1) return c/2*(t*t*(((s)+1)*t - s)) + b;
    t-=2;
    return c/2*((t)*t*(((s)+1)*t + s) + 2) + b;
}

float easeOutInBack (float t, float b, float c, float d, float s)
{
    if (t < d/2) return easeOutBack (t*2, b, c/2, d, s);
    return easeInBack((t*2)-d, b+c/2, c/2, d, s);
}

float easeOutBounce (float t, float b, float c, float d)
{
    t/=d;
    if ((t) < (1/2.75f)) {
        return c*(7.5625f*t*t) + b;
    } else if (t < (2/2.75f)) {
        t-=(1.5f/2.75f);
        return c*(7.5625f*(t)*t + .75f) + b;
    } else if (t < (2.5f/2.75f)) {
        t-=(2.25f/2.75f);
        return c*(7.5625f*(t)*t + .9375f) + b;
    } else {
        t-=(2.625f/2.75f);
        return c*(7.5625f*(t)*t + .984375f) + b;
    }
}

float easeInBounce (float t, float b, float c, float d)
{
    return c - easeOutBounce (d-t, 0, c, d) + b;
}

float easeInOutBounce (float t, float b, float c, float d)
{
    if (t < d/2) return easeInBounce (t*2, 0, c, d) * .5f + b;
    else return easeOutBounce (t*2-d, 0, c, d) * .5f + c*.5f + b;
}

float easeOutInBounce (float t, float b, float c, float d)
{
    if (t < d/2) return easeOutBounce (t*2, b, c/2, d);
    return easeInBounce((t*2)-d, b+c/2, c/2, d);
}

EasingSettings::EasingSettings(uint32 _durationMillis, EasingFunction _easingFunction, EasingDirection _easingDirection)
{
    durationMillis = _durationMillis;
    easingFunction = _easingFunction;
    easingDirection = _easingDirection;
    birthdayMillis = 0;
    amplitude = 0;
    period = 0;
}

float EasingSettings::GetEasingFactor(uint32 gameTime)
{
    if(birthdayMillis == 0)
    {
        birthdayMillis = gameTime;
    }

    float age = (float)(gameTime - birthdayMillis) / durationMillis;

    if(age <= 0)
    {
        return 0;
    }

    if(age >= 1)
    {
        return 1;
    }

    switch(easingFunction)
    {
    case EasingFunctionLinear:
        return age;

    case EasingFunctionSine:
        switch(easingDirection)
        {
        case EasingDirectionIn:
            return easeInSine(age, 0, 1, 1);

        case EasingDirectionOut:
            return easeOutSine(age, 0, 1, 1);

        case EasingDirectionOutIn:
            return easeOutInSine(age, 0, 1, 1);

        case EasingDirectionInOut:
            return easeInOutSine(age, 0, 1, 1);
        }
        break;

    case EasingFunctionQuadratic: 
        switch(easingDirection)
        {
        case EasingDirectionIn:
            return easeInQuad(age, 0, 1, 1);

        case EasingDirectionOut:
            return easeOutQuad(age, 0, 1, 1);

        case EasingDirectionInOut:
            return easeInOutQuad(age, 0, 1, 1);

        case EasingDirectionOutIn:
            return easeOutInQuad(age, 0, 1, 1);
        }
        break;

    case EasingFunctionCubic: 
        switch(easingDirection)
        {
        case EasingDirectionIn:
            return easeInCubic(age, 0, 1, 1);

        case EasingDirectionOut:
            return easeOutCubic(age, 0, 1, 1);

        case EasingDirectionInOut:
            return easeInOutCubic(age, 0, 1, 1);

        case EasingDirectionOutIn:
            return easeOutInCubic(age, 0, 1, 1);
        }
        break;

    case EasingFunctionQuartic: 
        switch(easingDirection)
        {
        case EasingDirectionIn:
            return easeInQuart(age, 0, 1, 1);

        case EasingDirectionOut:
            return easeOutQuart(age, 0, 1, 1);

        case EasingDirectionInOut:
            return easeInOutQuart(age, 0, 1, 1);

        case EasingDirectionOutIn:
            return easeOutInQuart(age, 0, 1, 1);
        }
        break;

    case EasingFunctionQuintic: 
        switch(easingDirection)
        {
        case EasingDirectionIn:
            return easeInQuint(age, 0, 1, 1);

        case EasingDirectionOut:
            return easeOutQuint(age, 0, 1, 1);

        case EasingDirectionInOut:
            return easeInOutQuint(age, 0, 1, 1);

        case EasingDirectionOutIn:
            return easeOutInQuint(age, 0, 1, 1);
        }
        break;

    case EasingFunctionExponential: 
        switch(easingDirection)
        {
        case EasingDirectionIn:
            return easeInExpo(age, 0, 1, 1);

        case EasingDirectionOut:
            return easeOutExpo(age, 0, 1, 1);

        case EasingDirectionInOut:
            return easeInOutExpo(age, 0, 1, 1);

        case EasingDirectionOutIn:
            return easeOutInExpo(age, 0, 1, 1);
        }
        break;

    case EasingFunctionElastic: 
        switch(easingDirection)
        {
        case EasingDirectionIn:
            return easeInElastic(age, 0, 1, 1, period, amplitude);

        case EasingDirectionOut:
            return easeOutElastic(age, 0, 1, 1, period, amplitude);

        case EasingDirectionInOut:
            return easeInOutElastic(age, 0, 1, 1, period, amplitude);

        case EasingDirectionOutIn:
            return easeOutInElastic(age, 0, 1, 1, period, amplitude);
        }
        break;

    case EasingFunctionCircular: 
        switch(easingDirection)
        {
        case EasingDirectionIn:
            return easeInCirc(age, 0, 1, 1);

        case EasingDirectionOut:
            return easeOutCirc(age, 0, 1, 1);

        case EasingDirectionInOut:
            return easeInOutCirc(age, 0, 1, 1);

        case EasingDirectionOutIn:
            return easeOutInCirc(age, 0, 1, 1);
        }
        break;

    case EasingFunctionBounce:
        switch(easingDirection)
        {
        case EasingDirectionIn:
            return easeInBounce(age, 0, 1, 1);

        case EasingDirectionOut:
            return easeOutBounce(age, 0, 1, 1);

        case EasingDirectionInOut:
            return easeInOutBounce(age, 0, 1, 1);

        case EasingDirectionOutIn:
            return easeOutInBounce(age, 0, 1, 1);
        }
        break;

    case EasingFunctionBack:
        switch(easingDirection)
        {
        case EasingDirectionIn:
            return easeInBack(age, 0, 1, 1, amplitude);

        case EasingDirectionOut:
            return easeOutBack(age, 0, 1, 1, amplitude);

        case EasingDirectionInOut:
            return easeInOutBack(age, 0, 1, 1, amplitude);

        case EasingDirectionOutIn:
            return easeOutInBack(age, 0, 1, 1, amplitude);
        }
        break;
    }

    return 0;
}

LocationSourceDualTween::LocationSourceDualTween(IIntSource *_xSource, IIntSource *_ySource) : ILocationSource()
{
    xSource = _xSource;
    ySource = _ySource;
}

LocationSourceDualTween::~LocationSourceDualTween()
{
    delete xSource;
    delete ySource;
}

CIwVec2 LocationSourceDualTween::GetLocation(uint32 gameTime)
{
    return CIwVec2(xSource->GetInt(gameTime), ySource->GetInt(gameTime));
}

LocationSourceTween::LocationSourceTween(CIwVec2 _start, CIwVec2 _end, EasingSettings _settings) : settings(_settings), ILocationSource()
{
    start = CIwFVec2((float)_start.x, (float)_start.y);
    CIwFVec2 end((float)_end.x, (float)_end.y);
    deltaLocation = end - start;
}

CIwVec2 LocationSourceTween::GetLocation(uint32 gameTime)
{
    float tweenFactor = settings.GetEasingFactor(gameTime);

    CIwFVec2 result = start + deltaLocation * tweenFactor;
    return CIwVec2((int)result.x, (int)result.y);
}

DirectionalSourceTween::DirectionalSourceTween(iwangle _startDirection, iwangle _endDirection, iwangle _startTangent, EasingSettings _settings) : settings(_settings), IDirectionalSource()
{
    startDirection = _startDirection;
    deltaDirection = ANGLE_SUBTRACTION(_endDirection, _startDirection);
    deltaTangent = ANGLE_SUBTRACTION(_startTangent, _startDirection);
    lastUpdate = 0;
}

void DirectionalSourceTween::UpdateAngles(uint32 gameTime)
{
    if(lastUpdate == gameTime)
    {
        return;
    }

    lastUpdate = gameTime;

    float tweenFactor = settings.GetEasingFactor(gameTime);

    curDirection = ANGLE_ADDITION(startDirection, ANGLE_MULTIPLICATION(deltaDirection, IW_FIXED(tweenFactor)));
    curTangent = ANGLE_ADDITION(curDirection, deltaTangent);
}

iwangle DirectionalSourceTween::GetNormal(uint32 gameTime) 
{ 
    UpdateAngles(gameTime);
    return curDirection;
}
iwangle DirectionalSourceTween::GetTangent(uint32 gameTime)
{ 
    UpdateAngles(gameTime);
    return curTangent;
}
iwangle DirectionalSourceTween::GetOrientation(uint32 gameTime)
{ 
    UpdateAngles(gameTime);
    return curDirection;
}

FloatSourceTween::FloatSourceTween(float _startValue, float _endValue, EasingSettings _settings) : settings(_settings)
{
    startValue = _startValue;
    deltaValue = _endValue - _startValue;
}

float FloatSourceTween::GetFloat(uint32 gameTime) 
{
    float tweenFactor = settings.GetEasingFactor(gameTime);

    return startValue + deltaValue * tweenFactor;
}

IntSourceTween::IntSourceTween(int _startValue, int _endValue, EasingSettings _settings) : settings(_settings)
{
    startValue = (float)_startValue;
    deltaValue = (float)_endValue - _startValue;
}

int IntSourceTween::GetInt(uint32 gameTime) 
{
    float tweenFactor = settings.GetEasingFactor(gameTime);

    return (int)(startValue + deltaValue * tweenFactor + 0.5);
}

ColorSourceTween::ColorSourceTween(CIwColour _startValue, CIwColour _endValue, EasingSettings _settings) : settings(_settings)
{
    startA = (float)_startValue.a;
    startR = (float)_startValue.r;
    startG = (float)_startValue.g;
    startB = (float)_startValue.b;

    deltaA = (float)(_endValue.a - _startValue.a);
    deltaR = (float)(_endValue.r - _startValue.r);
    deltaG = (float)(_endValue.g - _startValue.g);
    deltaB = (float)(_endValue.b - _startValue.b);
}

CIwColour ColorSourceTween::GetColor(uint32 gameTime) 
{ 
    float tweenFactor = settings.GetEasingFactor(gameTime);

    int retA = (int)(startA + deltaA * tweenFactor + 0.5);
    int retR = (int)(startR + deltaR * tweenFactor + 0.5);
    int retG = (int)(startG + deltaG * tweenFactor + 0.5);
    int retB = (int)(startB + deltaB * tweenFactor + 0.5);

    CIwColour retVal;

    retVal.Set(retR, retG, retB, retA);

    return retVal;
}

