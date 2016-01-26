#pragma once

#include <vector>

#include "s3e.h"
#include "Iw2D.h"

#include "Sprite.h"

enum PathMode { PathModeRelative = 1, PathModeAbsolute = 2 };

class PathSegment
{
protected:
    PathMode pathMode;
    float speedFactor;
    CIwVec2 startPoint;
    bool startPointSet;
    iwangle orientation, tangent, normal;

public:
    PathSegment()
    {
        pathMode = PathModeRelative;
        speedFactor = 1;
        startPointSet = false;
        startPoint = CIwVec2(0, 0);
    }

    virtual bool IsValid() = 0;
    virtual void RecalculatePathData() = 0;

    virtual PathMode GetPathMode();
    virtual void SetPathMode(PathMode pathMode);
    virtual float GetSpeedFactor();
    virtual void SetSpeedFactor(float speedFactor);
    virtual CIwVec2 GetStartPoint();
    virtual void SetStartPoint(CIwVec2 startPoint);
    virtual void SetRelativeStartPoint(CIwVec2 startPoint);

    virtual CIwVec2 GetEndPoint() = 0;
    virtual float GetLength() = 0;
    virtual CIwVec2 GetLocation(float progressRatio) = 0;
    virtual iwangle GetNormal(float progressRatio) = 0;
    virtual iwangle GetTangent(float progressRatio) = 0;
    virtual iwangle GetOrientation(float progressRatio) = 0;

    virtual void RenderSegment() = 0;
};

class PathSegmentLine : public PathSegment
{
    CIwVec2 endPoint;
    bool endPointSet;
    float length;
    CIwVec2 slope;

public:
    PathSegmentLine(CIwVec2 endPoint);
    PathSegmentLine(CIwVec2 endPoint, float speedFactor);
    PathSegmentLine(CIwVec2 startPoint, CIwVec2 endPoint);
    PathSegmentLine(CIwVec2 startPoint, CIwVec2 endPoint, float speedFactor);

    virtual bool IsValid();
    virtual void RecalculatePathData();

    virtual CIwVec2 GetEndPoint();
    virtual void SetEndPoint(CIwVec2 endPoint);

    virtual float GetLength();
    virtual CIwVec2 GetLocation(float progressRatio);
    virtual iwangle GetNormal(float progressRatio);
    virtual iwangle GetTangent(float progressRatio);
    virtual iwangle GetOrientation(float progressRatio);

    virtual void RenderSegment();

};

class PathSegmentBezier : public PathSegment
{
    CIwVec2 cp1, cp2, endPoint;
    bool endPointSet;
    float length;
    CIwVec2 tan01Slope, tan12Slope, tan23Slope;
    CIwVec2 lastLocation;
    iwangle lastTangent, lastNormal;
    double ratioIncrement;
    double lastSegmentRatio;

public:
    PathSegmentBezier(CIwVec2 cp1, CIwVec2 cp2, CIwVec2 endPoint);
    PathSegmentBezier(CIwVec2 cp1, CIwVec2 cp2, CIwVec2 endPoint, float speedFactor);
    PathSegmentBezier(CIwVec2 startPoint, CIwVec2 cp1, CIwVec2 cp2, CIwVec2 endPoint);
    PathSegmentBezier(CIwVec2 startPoint, CIwVec2 cp1, CIwVec2 cp2, CIwVec2 endPoint, float speedFactor);

    virtual bool IsValid();
    virtual void RecalculatePathData();

    CIwVec2 GetBezierPoint(double pathRatio);

    virtual CIwVec2 GetCP1();
    virtual void SetCP1(CIwVec2 cp1);
    virtual CIwVec2 GetCP2();
    virtual void SetCP2(CIwVec2 cp2);
    virtual CIwVec2 GetEndPoint();
    virtual void SetEndPoint(CIwVec2 endPoint);

    virtual void UpdateState(float progressRatio);

    virtual float GetLength();
    virtual CIwVec2 GetLocation(float progressRatio);
    virtual iwangle GetNormal(float progressRatio);
    virtual iwangle GetTangent(float progressRatio);
    virtual iwangle GetOrientation(float progressRatio);

    virtual void RenderSegment();
};

class PathFollowingSprite : public Sprite
{
    std::vector<PathSegment *> *segments;
    std::vector<uint32> *segmentStartTimes;

    uint32 totalPathLength;
    bool pathCalculated;
    uint32 totalPathTimeMillis;
    uint32 pixelsPerSecond;
    uint32 calcPathMillis;
    int32 loopCount;

    bool renderPath;
    CIwColour pathColor;

    void RecalculatePathData();
    void RenderImage(uint32 gameTime);

public:
    PathFollowingSprite(GameManager *gameManager);
    ~PathFollowingSprite();

    void ClearSegments();
    void AddSegment(PathSegment *segment);
    void SetTotalPathTimeMillis(uint32 pathTimeMillis);
    void SetPathSpeedPixelsPerSecond(uint32 pixelsPerSecond);
    int32 GetLoopCount();
    void SetLoopCount(int32 loopCount);

    void Update(uint32 gameTime, uint32 timeSinceLastUpdate);

    void SetBaseLocation(CIwVec2 location);
    void SetBaseLocationSource(ILocationSource *locationSource);

    void SetRenderPath(bool renderPath);
    bool GetRenderPath();

    void SetRenderColor(CIwColour pathColor);
    CIwColour GetRenderColor();
};


