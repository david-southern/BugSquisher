#include <vector>

#include "s3e.h"
#include "Iw2D.h"

#include "PathFollowingSprite.h"

PathMode PathSegment::GetPathMode()
{
    return pathMode;
}

void PathSegment::SetPathMode(PathMode pathMode)
{
    this->pathMode = pathMode;
    RecalculatePathData();
}

float PathSegment::GetSpeedFactor()
{
    return speedFactor;
}

void PathSegment::SetSpeedFactor(float speedFactor)
{
    this->speedFactor = speedFactor;
    RecalculatePathData();
}

CIwVec2 PathSegment::GetStartPoint()
{
    if(pathMode == PathModeRelative && !startPointSet)
    {
        s3eDebugAssertShow(S3E_MESSAGE_CONTINUE, "PathSegment::GetStartPoint called before relative start point was set");
    }

    return SUBPIXEL_TO_SCREEN(startPoint);
}

void PathSegment::SetStartPoint(CIwVec2 startPoint)
{
    this->startPoint = SCREEN_TO_SUBPIXEL(startPoint);
    startPointSet = true;
    RecalculatePathData();
}

void PathSegment::SetRelativeStartPoint(CIwVec2 startPoint)
{
    if(pathMode == PathModeRelative)
    {
        SetStartPoint(startPoint);
    }
}

PathSegmentLine::PathSegmentLine(CIwVec2 endPoint)
{
    endPointSet = false;
    SetPathMode(PathModeRelative);
    SetEndPoint(endPoint);
}

PathSegmentLine::PathSegmentLine(CIwVec2 endPoint, float speedFactor)
{
    endPointSet = false;
    SetPathMode(PathModeRelative);
    SetEndPoint(endPoint);
    this->speedFactor = speedFactor;
}

PathSegmentLine::PathSegmentLine(CIwVec2 startPoint, CIwVec2 endPoint)
{
    endPointSet = false;
    SetPathMode(PathModeAbsolute);
    SetStartPoint(startPoint);
    SetEndPoint(endPoint);
}

PathSegmentLine::PathSegmentLine(CIwVec2 startPoint, CIwVec2 endPoint, float speedFactor)
{
    endPointSet = false;
    SetPathMode(PathModeAbsolute);
    SetStartPoint(startPoint);
    SetEndPoint(endPoint);
    this->speedFactor = speedFactor;
}

bool PathSegmentLine::IsValid()
{
    return startPointSet && endPointSet;
}

void PathSegmentLine::RecalculatePathData()
{
    if(!IsValid())
    {
        return;
    }

    CIwVec2 orientVec;

    if(pathMode == PathModeRelative)
    {
        orientVec = GetEndPoint();
        length = (float)sqrt(endPoint.x * endPoint.x + endPoint.y * endPoint.y); 
    }
    else
    {
        orientVec = GetEndPoint() - GetStartPoint();
        length = (float)sqrt((endPoint.x - startPoint.x) * (endPoint.x - startPoint.x) + (endPoint.y - startPoint.y) * (endPoint.y - startPoint.y)); 
    }

    if(orientVec == CIwVec2::g_Zero)
    {
        orientation = 0;
    }
    else
    {
        orientation = IwGeomAtan2(orientVec.y, orientVec.x); 
    }

    tangent = orientation;
    normal = ANGLE_ADDITION(orientation, ANGLE_90_DEGREES);

    slope.x = IwGeomCos(orientation);
    slope.y = IwGeomSin(orientation);

    length = length / speedFactor;
}

CIwVec2 PathSegmentLine::GetEndPoint()
{
    return SUBPIXEL_TO_SCREEN(endPoint);
}

void PathSegmentLine::SetEndPoint(CIwVec2 endPoint)
{
    this->endPoint = SCREEN_TO_SUBPIXEL(endPoint);
    endPointSet = true;
    RecalculatePathData();
}

float PathSegmentLine::GetLength()
{
    return length;
}

CIwVec2 PathSegmentLine::GetLocation(float progressRatio)
{
    CIwVec2 offset;
    offset.x = (int)(IW_FIXED_TO_FLOAT(slope.x) * length * progressRatio);
    offset.y = (int)(IW_FIXED_TO_FLOAT(slope.y) * length * progressRatio);

    return startPoint + offset;
}

iwangle PathSegmentLine::GetNormal(float progressRatio)
{
    return normal;
}

iwangle PathSegmentLine::GetTangent(float progressRatio)
{
    return tangent;
}

iwangle PathSegmentLine::GetOrientation(float progressRatio)
{
    return orientation;
}

void PathSegmentLine::RenderSegment()
{
    Iw2DDrawLine(CIwSVec2(startPoint), CIwSVec2(endPoint));
}

PathSegmentBezier::PathSegmentBezier(CIwVec2 cp1, CIwVec2 cp2, CIwVec2 endPoint)
{
    endPointSet = false;
    SetPathMode(PathModeRelative);
    SetCP1(cp1);
    SetCP2(cp2);
    SetEndPoint(endPoint);
}

PathSegmentBezier::PathSegmentBezier(CIwVec2 cp1, CIwVec2 cp2, CIwVec2 endPoint, float speedFactor)
{
    endPointSet = false;
    SetPathMode(PathModeRelative);
    SetCP1(cp1);
    SetCP2(cp2);
    SetSpeedFactor(speedFactor);
    SetEndPoint(endPoint);
}

PathSegmentBezier::PathSegmentBezier(CIwVec2 startPoint, CIwVec2 cp1, CIwVec2 cp2, CIwVec2 endPoint)
{
    endPointSet = false;
    SetStartPoint(startPoint);
    SetPathMode(PathModeAbsolute);
    SetCP1(cp1);
    SetCP2(cp2);
    SetEndPoint(endPoint);
}

PathSegmentBezier::PathSegmentBezier(CIwVec2 startPoint, CIwVec2 cp1, CIwVec2 cp2, CIwVec2 endPoint, float speedFactor)
{
    endPointSet = false;
    SetPathMode(PathModeAbsolute);
    SetStartPoint(startPoint);
    SetCP1(cp1);
    SetCP2(cp2);
    SetSpeedFactor(speedFactor);
    SetEndPoint(endPoint);
}


bool PathSegmentBezier::IsValid()
{
    return startPointSet && endPointSet;
}

CIwVec2 PathSegmentBezier::GetBezierPoint(double t)
{
    if(t < 0)
    {
        return startPoint;
    }

    if(t > 1)
    {
        return endPoint;
    }

    // B(t) = (1-t)^3 * P0 + 3(1-t)^2 * t * P1 + 3(1-t) * t^2 * P2 + t^3 * P3
    // where t goes from 0 to 1

    double oneMinusT = (1-t);
    double oMTSquared = oneMinusT * oneMinusT;
    double oMTCubed = oMTSquared * oneMinusT;

    double tSquared = t * t;
    double tCubed = tSquared * t;

    double x = 
        oMTCubed * startPoint.x
        + 3 * oMTSquared * t * cp1.x
        + 3 * oneMinusT * tSquared * cp2.x
        + tCubed * endPoint.x;
    double y = 
        oMTCubed * startPoint.y
        + 3 * oMTSquared * t * cp1.y
        + 3 * oneMinusT * tSquared * cp2.y
        + tCubed * endPoint.y;

    return CIwVec2((int32)x, (int32)y);
}

void PathSegmentBezier::RecalculatePathData()
{
    if(!IsValid())
    {
        return;
    }

    // We are in subpixel space, so a threshold of 16 comes out to 2 screen pixels
    // Square it so we don't have to do a sqrt on the threshold check
    double SMOOTHNESS_THRESHOLD = 16 * 16;

    ratioIncrement = 0.1;
    double deltaLength;

    // Check the ratioIncrement at the beginning, middle, and end of the curve, in case
    // the curve speeds are different at various points
    do
    {
        ratioIncrement /= 2;
        CIwVec2 checkPoint = GetBezierPoint(ratioIncrement);
        double deltaX = checkPoint.x - startPoint.x;
        double deltaY = checkPoint.y - startPoint.y;
        deltaLength = deltaX * deltaX + deltaY * deltaY;
    } while(deltaLength > SMOOTHNESS_THRESHOLD);

    CIwVec2 midPoint = GetBezierPoint(0.5);
    do
    {
        CIwVec2 checkPoint = GetBezierPoint(0.5 + ratioIncrement);
        double deltaX = checkPoint.x - midPoint.x;
        double deltaY = checkPoint.y - midPoint.y;
        deltaLength = deltaX * deltaX + deltaY * deltaY;
        if(deltaLength > SMOOTHNESS_THRESHOLD)
        {
            ratioIncrement /= 2;
        }
    } while(deltaLength > SMOOTHNESS_THRESHOLD);

    do
    {
        CIwVec2 checkPoint = GetBezierPoint(1.0 - ratioIncrement);
        double deltaX = checkPoint.x - endPoint.x;
        double deltaY = checkPoint.y - endPoint.y;
        deltaLength = deltaX * deltaX + deltaY * deltaY;
        if(deltaLength > SMOOTHNESS_THRESHOLD)
        {
            ratioIncrement /= 2;
        }
    } while(deltaLength > SMOOTHNESS_THRESHOLD);

    double doubleLength = 0;

    double checkIncrement = 0;
    double nextIncrement = ratioIncrement;

    while(checkIncrement < 1)
    {
        CIwVec2 firstPoint = GetBezierPoint(checkIncrement);
        CIwVec2 secondPoint = GetBezierPoint(nextIncrement > 1 ? 1 : nextIncrement);
        
        double deltaX = secondPoint.x - firstPoint.x;
        double deltaY = secondPoint.y - firstPoint.y;
        doubleLength += sqrt(deltaX * deltaX + deltaY * deltaY);

        checkIncrement = nextIncrement;
        nextIncrement += ratioIncrement;
    }

    length = (float)(doubleLength / speedFactor);

    tan01Slope = cp1 - startPoint;
    tan12Slope= cp2 - cp1;
    tan23Slope= endPoint - cp2;

    lastLocation = CIwVec2(0, 0);
    lastTangent = 0;
    lastNormal = 0;
    lastSegmentRatio = 999;
}


CIwVec2 PathSegmentBezier::GetCP1()
{
    return SUBPIXEL_TO_SCREEN(cp1);
}

void PathSegmentBezier::SetCP1(CIwVec2 cp1)
{
    this->cp1 = SCREEN_TO_SUBPIXEL(cp1);
    RecalculatePathData();
}

CIwVec2 PathSegmentBezier::GetCP2()
{
    return SUBPIXEL_TO_SCREEN(cp2);
}

void PathSegmentBezier::SetCP2(CIwVec2 cp2)
{
    this->cp2 = SCREEN_TO_SUBPIXEL(cp2);
    RecalculatePathData();
}

CIwVec2 PathSegmentBezier::GetEndPoint()
{
    return SUBPIXEL_TO_SCREEN(endPoint);
}

void PathSegmentBezier::SetEndPoint(CIwVec2 endPoint)
{
    this->endPoint = SCREEN_TO_SUBPIXEL(endPoint);
    this->endPointSet = true;
    RecalculatePathData();
}

void PathSegmentBezier::UpdateState(float progressRatio)
{
    if(lastSegmentRatio == progressRatio)
    {
        return;
    }

    lastSegmentRatio = progressRatio;

    iwfixed interpolationPoint = IW_FIXED(progressRatio);

    CIwVec2 tan01Point = startPoint + tan01Slope * interpolationPoint;
    CIwVec2 tan12Point = cp1 + tan12Slope * interpolationPoint;
    CIwVec2 tan23Point = cp2 + tan23Slope * interpolationPoint;

    CIwVec2 tan1Slope = tan12Point - tan01Point;
    CIwVec2 tan2Slope = tan23Point - tan12Point;

    CIwVec2 tan1Point = tan01Point + tan1Slope * interpolationPoint;
    CIwVec2 tan2Point = tan12Point + tan2Slope * interpolationPoint;

    CIwVec2 tanSlope = tan2Point - tan1Point;

    lastTangent = IwGeomAtan2(tanSlope.y, tanSlope.x);
    lastNormal = ANGLE_ADDITION(lastTangent, ANGLE_90_DEGREES);

    lastLocation = GetBezierPoint(progressRatio);
}

float PathSegmentBezier::GetLength()
{
    return length;
}

iwangle PathSegmentBezier::GetNormal(float progressRatio)
{
    UpdateState(progressRatio);
    return lastNormal;
}

iwangle PathSegmentBezier::GetTangent(float progressRatio)
{
    UpdateState(progressRatio);
    return lastTangent;
}

iwangle PathSegmentBezier::GetOrientation(float progressRatio)
{
    UpdateState(progressRatio);
    return lastTangent;
}

CIwVec2 PathSegmentBezier::GetLocation(float progressRatio)
{
    UpdateState(progressRatio);
    return lastLocation;
}

void PathSegmentBezier::RenderSegment()
{
    double renderRatioInc = 0.05;
    double checkIncrement = 0;
    double nextIncrement = renderRatioInc;

    while(checkIncrement < 1)
    {
        CIwVec2 firstPoint = GetBezierPoint(checkIncrement);
        CIwVec2 secondPoint = GetBezierPoint(nextIncrement > 1 ? 1 : nextIncrement);
        
        Iw2DDrawLine(CIwSVec2(firstPoint), CIwSVec2(secondPoint));

        checkIncrement = nextIncrement;
        nextIncrement += renderRatioInc;
    }
}

PathFollowingSprite::PathFollowingSprite(GameManager *gameManager) : Sprite(gameManager)
{
    segments = new std::vector<PathSegment *>;
    segmentStartTimes = new std::vector<uint32>;

    totalPathLength = 0;
    pathCalculated = false;
    totalPathTimeMillis = 0;
    pixelsPerSecond = 0;
    loopCount = -1;

    renderPath = false;
    pathColor.Set(0, 0, 0, 0);
}

PathFollowingSprite::~PathFollowingSprite()
{
    ClearSegments();
    delete segments;
    delete segmentStartTimes;
}

void PathFollowingSprite::ClearSegments()
{
    for(uint32 segmentIndex = 0; segmentIndex < segments->size(); segmentIndex++)
    {
        delete segments->at(segmentIndex);
    }

    segments->clear();
    totalPathLength = 0;
    pathCalculated = false;
}

void PathFollowingSprite::AddSegment(PathSegment *segment)
{
    segments->push_back(segment);
    totalPathLength = 0;
    pathCalculated = false;
}

void PathFollowingSprite::SetTotalPathTimeMillis(uint32 pathTimeMillis)
{
    totalPathTimeMillis = pathTimeMillis;
    pixelsPerSecond = 0;
    pathCalculated = false;
}

void PathFollowingSprite::SetPathSpeedPixelsPerSecond(uint32 pixelsPerSecond)
{
    this->pixelsPerSecond = SCREEN_TO_SUBPIXEL(pixelsPerSecond);
    totalPathTimeMillis = 0;
    pathCalculated = false;
}

int32 PathFollowingSprite::GetLoopCount()
{
    return loopCount;
}

void PathFollowingSprite::SetLoopCount(int32 loopCount)
{
    this->loopCount = loopCount;
}

void PathFollowingSprite::RecalculatePathData()
{
    segmentStartTimes->clear();

    totalPathLength = 0;

    CIwVec2 relStartPoint = GetBaseLocation();

    for(uint32 segmentIndex = 0; segmentIndex < segments->size(); segmentIndex++)
    {
        segments->at(segmentIndex)->SetRelativeStartPoint(relStartPoint);
        totalPathLength += (uint32)segments->at(segmentIndex)->GetLength();
        relStartPoint = SUBPIXEL_TO_SCREEN(segments->at(segmentIndex)->GetLocation(1.0));
    }

    calcPathMillis = 0;

    if(pixelsPerSecond > 0)
    {
        calcPathMillis = (uint32)(((float)totalPathLength / pixelsPerSecond) * 1000);
    }
    else
    {
        calcPathMillis = totalPathTimeMillis;
    }

    if(calcPathMillis == 0)
    {
        s3eDebugAssertShow(S3E_MESSAGE_CONTINUE, "You must call either SetTotalPathTimeMillis or SetPathSpeedPixelsPerSecond before starting a PathFollowingSprite");
        Kill();
        return;
    }

    uint32 segmentStartTime = 0;
    for(uint32 segmentIndex = 0; segmentIndex < segments->size(); segmentIndex++)
    {
        segmentStartTimes->push_back(segmentStartTime);
        segmentStartTime += (uint32)(((float)segments->at(segmentIndex)->GetLength() / totalPathLength) * calcPathMillis);
    }

    pathCalculated = true;
}

void PathFollowingSprite::Update(uint32 gameTime, uint32 timeSinceLastUpdate)
{
    Sprite::UpdateAge(gameTime);

    if(!Active())
    {
        return;
    }

    if(!pathCalculated)
    {
        RecalculatePathData();
        
        if(!Active())
        {
            return;
        }
    }

    if(loopCount > -1)
    {
        uint32 totalLoops = GetAge(gameTime) / calcPathMillis;
        if(totalLoops >= (uint32)loopCount)
        {
            Stop();
            return;
        }
    }

    uint32 pathPosition = GetAge(gameTime) % calcPathMillis;

    uint32 currentSegmentIndex = UINT32_MAX;

    for(uint32 segmentIndex = 1; segmentIndex < segmentStartTimes->size(); segmentIndex++)
    {
        if(segmentStartTimes->at(segmentIndex) > pathPosition)
        {
            currentSegmentIndex = segmentIndex - 1;
            break;
        }
    }

    if(currentSegmentIndex == UINT32_MAX)
    {
        currentSegmentIndex = segmentStartTimes->size() - 1;
    }

    uint32 currentSegmentStartTime = segmentStartTimes->at(currentSegmentIndex);    
    uint32 nextSegmentStartTime = calcPathMillis;

    if(currentSegmentIndex < segmentStartTimes->size() - 1)
    {
        nextSegmentStartTime = segmentStartTimes->at(currentSegmentIndex + 1);
    }

    float segmentTime = (float)(nextSegmentStartTime - currentSegmentStartTime);
    float segmentRatio = (pathPosition - currentSegmentStartTime) / segmentTime;

    baseLocation = segments->at(currentSegmentIndex)->GetLocation(segmentRatio);
    baseOrientation = segments->at(currentSegmentIndex)->GetOrientation(segmentRatio);
    baseNormal = segments->at(currentSegmentIndex)->GetNormal(segmentRatio);
    baseTangent = segments->at(currentSegmentIndex)->GetTangent(segmentRatio);

    // Make sure to call Sprite;:Udpate after our update, since we modify baseLocation, and that is what Sprite::Update
    // uses to start the transormation chain
    Sprite::Update(gameTime, timeSinceLastUpdate);
}

void PathFollowingSprite::SetBaseLocation(CIwVec2 location)
{
    s3eDebugAssertShow(S3E_MESSAGE_CONTINUE, "Do not call SetBaseLocation on PathFollowingSprite, this sprite manages its own baseLocation");
}

void PathFollowingSprite::SetBaseLocationSource(ILocationSource *locationSource)
{
    s3eDebugAssertShow(S3E_MESSAGE_CONTINUE, "Do not call SetBaseLocationSource on PathFollowingSprite, this sprite manages its own baseLocation");
    if(locationSource != null)
    {
        delete locationSource;
    }
}


void PathFollowingSprite::SetRenderPath(bool renderPath)
{
    this->renderPath = renderPath;
}

bool PathFollowingSprite::GetRenderPath()
{
    return renderPath;
}


void PathFollowingSprite::SetRenderColor(CIwColour pathColor)
{
    this->pathColor = pathColor;
}

CIwColour PathFollowingSprite::GetRenderColor()
{
    return pathColor;
}

void PathFollowingSprite::RenderImage(uint32 gameTime)
{
    if(!Active())
    {
        return;
    }

    if(!pathCalculated)
    {
        RecalculatePathData();
        
        if(!Active())
        {
            return;
        }
    }

    CIwColour debugColors[3];
    debugColors[0] = g_IwGxColours[IW_GX_COLOUR_BLUE];
    debugColors[1] = g_IwGxColours[IW_GX_COLOUR_MAGENTA];
    debugColors[2] = g_IwGxColours[IW_GX_COLOUR_RED];
    int debugColorIndex = 0;

    if(renderPath)
    {
        Iw2DSetFont(mgr->GetDebugFont());

        CIwMat2D restoreMat = Iw2DGetTransformMatrixSubPixel();

        Iw2DSetTransformMatrixSubPixel(CIwMat2D::g_Identity);

        Iw2DSetColour(pathColor);
        for(uint32 segmentIndex = 0; segmentIndex < segments->size(); segmentIndex++)
        {
            if(pathColor.a == 0)
            {
                Iw2DSetColour(debugColors[debugColorIndex++]);
                if(debugColorIndex > 3)
                {
                    debugColorIndex = 0;
                }
            }
            segments->at(segmentIndex)->RenderSegment();
        }

        Iw2DSetTransformMatrixSubPixel(restoreMat);
    }

    Iw2DSetColour(0xffffffff);

    Sprite::RenderImage(gameTime);
}
