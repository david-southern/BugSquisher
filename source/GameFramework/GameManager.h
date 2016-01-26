#pragma once

#include <vector>

#include "s3e.h"
#include "GameInterfaces.h"
#include "ResourceManager.h"
#include "StateManager.h"

using namespace std;

class GameManager
{
    ResourceManager *resourceManager;
    StateManager *stateManager;
    uint32 lastClockTick;
    uint32 currentClockTick;

    static CIw2DFont *debugFont;
    static int debugPrintIndex;
    static uint32 displayWidth;
    static uint32 displayHeight;

    bool gameTerminated;
    uint32 maxFPS;
    uint32 millisPerFrame;

    std::vector<IGameObject*> *gameObjects;
    std::vector<IGameObject*> *pendingAdditions;
    std::vector<IGameObject*> *pendingRemovals;
    bool ZIndexChanged;

    void HandleUpdate(uint32 currentTime, uint32 deltaTime);
    void HandleRender(uint32 currentTime, uint32 deltaTime);
    void SortByZIndex();

    bool lockGameObjectList;

public:
    GameManager();
    ~GameManager();

    void MainLoop();

    void TerminateGame();

    ResourceManager *GetResourceManager();
    StateManager *GetStateManager();

    void SetRequestedFPS(uint32 requestedFPS);
    uint32 GetFPS();

    uint32 GetCurrentClockTime();
    int32 GetSurfaceWidth();
    int32 GetSurfaceHeight();

    void RegisterGameObject(IGameObject *gameObject);
    void RemoveGameObject(IGameObject *gameObject);
    void RemoveAllGameObjects();

    void NotifyZIndexChanged();

    void PointerEvent(s3ePointerEvent* pointerEvent);
    void PointerMotionEvent(s3ePointerMotionEvent* pointerMotionEvent);

    static void SetDebugFont(CIw2DFont *debugFont);
    static CIw2DFont *GetDebugFont();
    static void DebugPrint(char *buffer, uint32 textColor = 0xff000000);
    static void DumpMemoryLeaks(char *id, bool reset = false);
    bool showFPS;
};

extern CIwVec2 Convert(CIwFVec2 inVec);
