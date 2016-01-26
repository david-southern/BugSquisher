#include <vector>
#include <algorithm>

#include "GameInterfaces.h"
#include "GameManager.h"
#include "TouchManager.h"

#include "Sprite.h"

int IGameObject::NEXT_GAME_OBJECT_INDEX = 0;

int GameManager::debugPrintIndex;
uint32 GameManager::displayWidth;
uint32 GameManager::displayHeight;

GameManager::GameManager()
{
    Iw2DInit();
    IwResManagerInit(); 

    Iw2DSetSubPixelCoords(true);

    TouchManager::Init(this);
    resourceManager = new ResourceManager();
    stateManager = new StateManager(this);

    gameTerminated = false;
    currentClockTick = 0;
    lastClockTick = 0;

    displayWidth = Iw2DGetSurfaceWidth();
    displayHeight = Iw2DGetSurfaceHeight();

    lockGameObjectList = false;

    gameObjects = new std::vector<IGameObject*>;
    gameObjects->reserve(100);

    pendingAdditions = new std::vector<IGameObject*>;
    pendingAdditions->reserve(100);

    pendingRemovals = new std::vector<IGameObject*>;
    pendingRemovals->reserve(100);

    showFPS = false;
    ZIndexChanged = false;

    SetRequestedFPS(30);
}

GameManager::~GameManager()
{
    for(uint32 index = 0; index < gameObjects->size(); index++)
    {
        delete gameObjects->at(index);
    }

    delete gameObjects;

    for(uint32 index = 0; index < pendingAdditions->size(); index++)
    {
        delete pendingAdditions->at(index);
    }

    delete pendingAdditions;

    for(uint32 index = 0; index < pendingRemovals->size(); index++)
    {
        delete pendingRemovals->at(index);
    }

    delete pendingRemovals;

    delete resourceManager;
    delete stateManager;
    TouchManager::Shutdown();

    IwResManagerTerminate(); 
    Iw2DTerminate();
}

CIw2DFont *GameManager::debugFont = null;

void GameManager::SetDebugFont(CIw2DFont *debugFont)
{
    GameManager::debugFont = debugFont;
}

CIw2DFont *GameManager::GetDebugFont()
{
    return GameManager::debugFont;
}

void GameManager::DebugPrint(char *buffer, uint32 textColor)
{
    DEBUG_TRACER(GameManager::DebugPrint);

    CIw2DFont *prevFont = null;
    
    if(debugFont != null)
    {
        prevFont = Iw2DGetFont();
        Iw2DSetFont(debugFont);
    }

    CIwMat2D currentTransform = Iw2DGetTransformMatrixSubPixel();
    Iw2DSetTransformMatrixSubPixel(CIwMat2D::g_Identity);

    CIwColour prevColor = Iw2DGetColour();
    Iw2DSetColour(textColor);
    Iw2DDrawString(buffer, CIwSVec2(SCREEN_TO_SUBPIXEL(CIwVec2(10, debugPrintIndex * 14 + 10))), CIwSVec2(SCREEN_TO_SUBPIXEL(CIwVec2(displayWidth - 20, debugPrintIndex))), IW_2D_FONT_ALIGN_LEFT, IW_2D_FONT_ALIGN_TOP);
    debugPrintIndex++;
    Iw2DSetColour(prevColor);

    Iw2DSetTransformMatrixSubPixel(currentTransform);

    if(prevFont != null)
    {
        Iw2DSetFont(prevFont);
    }
}

void GameManager::DumpMemoryLeaks(char *id, bool reset)
{
    static int leakCount = 0;
    static int checkPoint = 0;

    if(!reset)
    {
        char fileName[1024];
        sprintf(fileName, "leaks.%d.%s.txt", leakCount, id);
        IwMemBucketDebugCheck(IwMemBucketGetID(), checkPoint, fileName);
    }
    else
    {
        leakCount++;
    }
    checkPoint = IwMemBucketCheckpoint();
}


void GameManager::MainLoop()
{
    NotifyZIndexChanged();

    uint32 lastUpdateTime = GetCurrentClockTime();

    uint32 currentSecond = 0;
    uint32 frameCount = 0;
    uint32 lastFPS = 0;
    uint32 lastSkippedFrames = 0;
    uint32 skippedFrames = 0;

    while(!gameTerminated)
    {
        DEBUG_TRACER(MainLoop);

        debugPrintIndex = 0;

        s3eDeviceYield(0);

        // Check for user quit
        if (s3eDeviceCheckQuitRequest())
        {
            break;
        }

        currentClockTick = (uint32)s3eTimerGetMs();
        uint32 deltaTime = currentClockTick - lastUpdateTime;

        if(deltaTime < millisPerFrame)
        {
            skippedFrames++;
            continue;
        }

        if(showFPS)
        {
            debugPrintIndex = 1;
        }

        lockGameObjectList = true;
        stateManager->Update(currentClockTick);
        lockGameObjectList = false;

        HandleUpdate(currentClockTick, deltaTime);
        HandleRender(currentClockTick, deltaTime);

        if(showFPS)
        {
            debugPrintIndex = 0;
            char buffer[1024];
            sprintf(buffer, "FPS: %d, skipped: %d", lastFPS, lastSkippedFrames);
            DebugPrint(buffer, 0xff00ffff);
        }

        uint32 thisSecond = currentClockTick / 1000;

        if(showFPS && thisSecond != currentSecond)
        {
            lastSkippedFrames = skippedFrames;
            skippedFrames = 0;
            lastFPS = frameCount;
            frameCount = 0;
            currentSecond = thisSecond;
        }
        frameCount++;
        lastUpdateTime = currentClockTick;

        //Present the rendered surface to the screen
        Iw2DSurfaceShow();
    }
}

void GameManager::SetRequestedFPS(uint32 requestedFPS)
{
    maxFPS = MIN(MAX(1, requestedFPS), 60);
    millisPerFrame = 1000 / maxFPS;
}

uint32 GameManager::GetFPS()
{
    return maxFPS;
}

void GameManager::PointerEvent(s3ePointerEvent* pointerEvent)
{
    DEBUG_TRACER(GameManager::PointerEvent);
    
    // Make sure that if we add gameObjects as a side effect of Touches that we don't HitTest the new objects
    uint32 maxTest = gameObjects->size();

    for (uint32 index = 0; index < maxTest; index++)
    {
        if(gameObjects->at(index)->HitTest(pointerEvent->m_x, pointerEvent->m_y))
        {
            if(pointerEvent->m_Pressed)
            {
                gameObjects->at(index)->TouchStart(pointerEvent->m_x, pointerEvent->m_y);
            }
            else
            {
                gameObjects->at(index)->TouchEnd(pointerEvent->m_x, pointerEvent->m_y);
            }
        }
    }
}

void GameManager::PointerMotionEvent(s3ePointerMotionEvent* pointerMotionEvent)
{
    DEBUG_TRACER(GameManager::PointerMotionEvent);
    for (uint32 index = 0; index < gameObjects->size(); index++)
    {
        if(gameObjects->at(index)->HitTest(pointerMotionEvent->m_x, pointerMotionEvent->m_y))
        {
            gameObjects->at(index)->TouchMove(pointerMotionEvent->m_x, pointerMotionEvent->m_y);
        }
    }
}

bool traceUpdate = false;

void GameManager::HandleUpdate(uint32 currentTime, uint32 deltaTime)
{
    DEBUG_TRACER(GameManager::HandleUpdate);

    for (uint32 index = 0; index < pendingAdditions->size(); index++)
    {
        IGameObject *addGO = pendingAdditions->at(index);

        if(traceUpdate)
        {
            s3eDebugTracePrintf("%d - Adding pending GO: %s", currentTime, addGO->debugId);
        }

        RegisterGameObject(addGO);
    }

    pendingAdditions->clear();

    for (uint32 index = 0; index < pendingRemovals->size(); index++)
    {
        IGameObject *remGO = pendingRemovals->at(index);

        if(traceUpdate)
        {
            s3eDebugTracePrintf("%d - Removing pending GO: %s", currentTime, remGO->debugId);
        }

        RemoveGameObject(remGO);
    }

    pendingRemovals->clear();

    lockGameObjectList = true;

    if(ZIndexChanged)
    {
        SortByZIndex();
    }

    uint32 updateSize = gameObjects->size();

    if(traceUpdate)
    {
        s3eDebugTracePrintf("%d - Updating %d GOs", currentTime, updateSize);
    }

    for (uint32 index = 0; index < updateSize; index++)
    {
        gameObjects->at(index)->PreUpdate(currentTime, deltaTime);
    }

    // Make sure that any game object that has a parent gets Updated after its parent

    bool childSkipped = false;
    bool workDone = false;

    for (uint32 index = 0; index < updateSize; index++)
    {
        IGameObject *updGO = gameObjects->at(index);

        if(traceUpdate)
        {
            s3eDebugTracePrintf("%d - Setting NeedsUpdate on GO %s(%d)", currentTime, updGO->debugId, index);
        }

        updGO->SetNeedsUpdate(true);
    }

    do
    {
        childSkipped = false;
        workDone = false;

        for (uint32 index = 0; index < updateSize; index++)
        {
            IGameObject *checkGO = gameObjects->at(index);

            if(traceUpdate)
            {
                s3eDebugTracePrintf("%d - Checking GO %s(%d) for update", currentTime, checkGO->debugId, index);
            }

            if(checkGO->GetNeedsUpdate())
            {
                if(checkGO->GetParent() == null || !checkGO->GetParent()->GetNeedsUpdate())
                {
                    checkGO->Update(currentTime, deltaTime);
                    checkGO->SetNeedsUpdate(false);
                    workDone = true;
                }
                else
                {
                    childSkipped = true;
                }
            }
        }

        if(!workDone)
        {
            // Just Update everything, hope for the best
            for (uint32 index = 0; index < updateSize; index++)
            {
                IGameObject *checkGO = gameObjects->at(index);

                if(checkGO->GetNeedsUpdate())
                {
                    workDone = true;
                    checkGO->Update(currentTime, deltaTime);
                }
            }

            if(workDone)
            {
                s3eDebugAssertShow(S3E_MESSAGE_CONTINUE, "No work done in HandleUpdate loop, when updates were needed, probably IGameObject->parent cycle");
            }

            break;
        }
    } while(childSkipped);

    if(traceUpdate)
    {
        s3eDebugTracePrintf("%d - Done updating GOs", currentTime);
    }

    for (int32 index = (int32)updateSize - 1; index >= 0; index--)
    {
        IGameObject *checkGO = gameObjects->at(index);
        if(!checkGO->Active())
        {
            if(traceUpdate)
            {
                s3eDebugTracePrintf("%d - Deleting Inactive GO: %s", currentTime, checkGO->debugId);
            }
            gameObjects->erase(gameObjects->begin() + index);
            stateManager->NotifyGameObjectDeletion(checkGO);
            delete checkGO;
            updateSize--;
        }
    }
    for (uint32 index = 0; index < updateSize; index++)
    {
        gameObjects->at(index)->PostUpdate(currentTime, deltaTime);
    }

    if(traceUpdate)
    {
        s3eDebugTracePrintf("%d - Finished HandleUpdate", currentTime);
    }

    lockGameObjectList = false;
}

void GameManager::HandleRender(uint32 currentTime, uint32 deltaTime)
{
    DEBUG_TRACER(GameManager::HandleRender);

    lockGameObjectList = true;

    for (uint32 index = 0; index < gameObjects->size(); index++)
    {
        gameObjects->at(index)->PreRender(currentTime, deltaTime);
    }
    for (uint32 index = 0; index < gameObjects->size(); index++)
    {
        gameObjects->at(index)->Render(currentTime, deltaTime);
    }
    for (uint32 index = 0; index < gameObjects->size(); index++)
    {
        gameObjects->at(index)->PostRender(currentTime, deltaTime);
    }

    lockGameObjectList = false;
}


void GameManager::TerminateGame()
{
    gameTerminated = true;
}

ResourceManager *GameManager::GetResourceManager()
{
    return resourceManager;
}

StateManager *GameManager::GetStateManager()
{
    return stateManager;
}

uint32 GameManager::GetCurrentClockTime()
{
    return currentClockTick;
}

int32 GameManager::GetSurfaceWidth()
{
    return displayWidth;
}

int32 GameManager::GetSurfaceHeight()
{
    return displayHeight;
}

void GameManager::RegisterGameObject(IGameObject *gameObject)
{
    DEBUG_TRACER(GameManager::RegisterGameObject);

    if(lockGameObjectList)
    {
        pendingAdditions->push_back(gameObject);
    }
    else
    {
        gameObjects->push_back(gameObject);
        NotifyZIndexChanged();
    }
}

void GameManager::RemoveGameObject(IGameObject *gameObject)
{
    DEBUG_TRACER(GameManager::RemoveGameObject);

    if(lockGameObjectList)
    {
        pendingRemovals->push_back(gameObject);
    }
    else
    {
        for(uint32 index = 0; index < gameObjects->size(); index++)
        {
            if(gameObjects->at(index) == gameObject)
            {
                gameObjects->erase(gameObjects->begin() + index);
                stateManager->NotifyGameObjectDeletion(gameObject);
                delete gameObject;
                return;
            }
        }
    }
}

void GameManager::RemoveAllGameObjects()
{
    DEBUG_TRACER(GameManager::RemoveAllGameObjects);

    // Do this through pendingRemovals, otherwise the gameObjects collection is modified
    // as we are iterating it, and that doesn't work
    for(uint32 index = 0; index < gameObjects->size(); index++)
    {
        pendingRemovals->push_back(gameObjects->at(index));
    }
}

bool compareZIndex(IGameObject *first, IGameObject *second)
{
    if(first == null && second == null) return false;
    if(first == null) return true;
    if(second == null) return false;

    int firstZIndex = first->GetZIndex();
    int secondZIndex = second->GetZIndex();

    return firstZIndex < secondZIndex;
}

void GameManager::NotifyZIndexChanged()
{
    ZIndexChanged = true;
}

void GameManager::SortByZIndex()
{
    DEBUG_TRACER(GameManager::SortByZIndex);

    std::sort(gameObjects->begin(), gameObjects->end(), compareZIndex);
    ZIndexChanged = false;
}

CIwVec2 Convert(CIwFVec2 inVec)
{
    return CIwVec2((int)inVec.x, (int)inVec.y);
}
