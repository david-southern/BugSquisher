#include <time.h>
#include <stdlib.h>
#include <vector>

#include "s3e.h"
#include "Iw2D.h"

#include "GameManager.h"
#include "AllSprites.h"
#include "AllTransforms.h"

#define BUG_COUNT 7
#define BUG_SPEED 11

int BUG_HANDLE;
int SPLAT_HANDLE;
int LEAVES_HANDLE;
int ARROW_HANDLE;
int FIRE_HANDLE;
int TREB_FONT_HANDLE;
int COOPER_FONT_HANDLE;

int bogsSquished = 0;

void DoSimpleTest(GameManager *mgr, uint32 marker_handle, uint32 width, uint32 height);
void MakeNormalShip(GameManager *mgr, int marker_handle, int x, int y);
void MakeOrbiters(GameManager *mgr, int x, int y, uint32 marker_handle);
void DoManyShipTest(GameManager *mgr, int maxX, int maxY, int width, int height, uint32 marker_handle);
void AnimationTest(GameManager *mgr, uint32 marker_handle, uint32 width, uint32 height);
void LabeledSprite(GameManager *mgr, uint32 marker_handle, uint32 width, uint32 height);
void PathTest(GameManager *mgr, uint32 marker_handle, uint32 width, uint32 height);

S3E_MAIN_DECL void IwMain()
{
    srand(time(NULL));

    GameManager *mgr;
    mgr = new GameManager();

    mgr->SetRequestedFPS(100);

    mgr->GetResourceManager()->RegisterResourceGroup("images.group");

    BUG_HANDLE = mgr->GetResourceManager()->RegisterImage("bug");
    SPLAT_HANDLE = mgr->GetResourceManager()->RegisterImage("splat");
    LEAVES_HANDLE = mgr->GetResourceManager()->RegisterImage("leaves");
    ARROW_HANDLE = mgr->GetResourceManager()->RegisterImage("right_arrow");
    FIRE_HANDLE = mgr->GetResourceManager()->RegisterImage("FireBlast");

    TREB_FONT_HANDLE = mgr->GetResourceManager()->RegisterFont("trebuchet16pt");
    COOPER_FONT_HANDLE = mgr->GetResourceManager()->RegisterFont("cooper_24pt");

    GameManager::SetDebugFont(mgr->GetResourceManager()->GetFont(TREB_FONT_HANDLE));

    Iw2DSetFont(mgr->GetResourceManager()->GetFont(TREB_FONT_HANDLE));

    Sprite *background = new Sprite(mgr);
    background->SetImage(mgr->GetResourceManager()->GetImage(LEAVES_HANDLE));
    background->SetBaseLocation(CIwVec2(0, 0));
    background->SetZIndex(-999);
    mgr->RegisterGameObject(background);

    int width = mgr->GetSurfaceWidth();
    int height = mgr->GetSurfaceHeight();

#define SIMPLE_TEST 0
#define MANY_SHIP_TEST 1
#define ORBITER_TEST 2
#define ANIMATION_TEST 3
#define LABELED_SPRITE 4
#define PATH_TEST 5

    int test_selection = SIMPLE_TEST;

    switch(test_selection)
    {
    case SIMPLE_TEST:
        DoSimpleTest(mgr, ARROW_HANDLE, width, height);
        break;

    case MANY_SHIP_TEST:
        DoManyShipTest(mgr, 10, 10, width, height, ARROW_HANDLE);
        break;

    case ORBITER_TEST:
        MakeOrbiters(mgr, width / 2, height / 2, ARROW_HANDLE);
        break;

    case ANIMATION_TEST:
        AnimationTest(mgr, 0, width, height);
        break;

    case LABELED_SPRITE:
        LabeledSprite(mgr, 0, width, height);
        break;

    case PATH_TEST:
        PathTest(mgr, ARROW_HANDLE, width, height);
        break;
    }

    mgr->MainLoop();

    delete mgr;
}

int resetCount = 0;

TextSprite *intSprite;
TextSprite *floatSprite;
TextSprite *stringSprite;

int INT_VAR_HANDLE;
int FLOAT_VAR_HANDLE;
int STRING_VAR_HANDLE;

int32 changeVarValue(void *v1, void *v2)
{
    StateManager *stateManager = (StateManager *)v1;
    EventCallback *callbackData = (EventCallback *)v2;
    StateVariable curVal = stateManager->GetStateVar(FLOAT_VAR_HANDLE);
    curVal.floatValue += 1.1f;
    stateManager->SetStateVar(FLOAT_VAR_HANDLE, curVal);

    curVal.intValue = (int)curVal.floatValue;
    stateManager->SetStateVar(INT_VAR_HANDLE, curVal);
    stateManager->PostFutureEvent("FloatChange", NULL, callbackData->gameTime + 3000);
    return 0;
}

void DoSimpleTest(GameManager *mgr, uint32 marker_handle, uint32 width, uint32 height)
{
    StateManager *state = mgr->GetStateManager();

    StateVariable tmpVar;

    tmpVar.intValue = 21;
    INT_VAR_HANDLE = state->RegisterStateVariable("IntVar", tmpVar);

    tmpVar.floatValue = 3.1415f;
    FLOAT_VAR_HANDLE = state->RegisterStateVariable("FloatVar", tmpVar);

    tmpVar.stringValue = "Hello World!";
    STRING_VAR_HANDLE = state->RegisterStateVariable("StringVar", tmpVar);

    PathFollowingSprite *intPath = new PathFollowingSprite(mgr);
    intPath->AddSegment(new PathSegmentBezier(CIwVec2(100,100), CIwVec2(200, 900), CIwVec2(700, -50), CIwVec2(800,600)));
    intPath->SetTotalPathTimeMillis(5000);
    mgr->RegisterGameObject(intPath);

    CIwColour transparent;

    transparent.Set(0, 0, 0, 0);

    char buffer[1024];
    sprintf(buffer, "Int: %d", state->GetStateVar(INT_VAR_HANDLE).intValue);
    intSprite = new TextSprite(mgr, buffer);
    intSprite->SetBackgroundColor(g_IwGxColours[IW_GX_COLOUR_WHITE]);
    intSprite->SetBorderColor(g_IwGxColours[IW_GX_COLOUR_BLUE]);
    intSprite->SetBorderThickness(CIwVec2(3, 3));
    intSprite->SetPadding(CIwVec2(3, 3));
    intSprite->SetBaseLocation(CIwVec2(300, 100));
    //intSprite->AddTransformation(new AnchorToTransformation(intPath, CIwVec2(0,0)));
    //intSprite->AddTransformation(new OrbitTransformation(0, 5000));
    mgr->RegisterGameObject(intSprite);

    sprintf(buffer, "Float: %f", state->GetStateVar(FLOAT_VAR_HANDLE).floatValue);
    floatSprite = new TextSprite(mgr, buffer);
    floatSprite->SetBackgroundColor(g_IwGxColours[IW_GX_COLOUR_WHITE]);
    floatSprite->SetBorderColor(g_IwGxColours[IW_GX_COLOUR_GREEN]);
    floatSprite->SetBorderThickness(CIwVec2(3, 3));
    floatSprite->SetPadding(CIwVec2(3, 3));
    floatSprite->SetBaseLocation(CIwVec2(300, 200));
    mgr->RegisterGameObject(floatSprite);

    sprintf(buffer, "String: %s", state->GetStateVar(STRING_VAR_HANDLE).stringValue);
    stringSprite = new TextSprite(mgr, buffer);
    stringSprite->SetBackgroundColor(g_IwGxColours[IW_GX_COLOUR_WHITE]);
    stringSprite->SetBorderColor(g_IwGxColours[IW_GX_COLOUR_CYAN]);
    stringSprite->SetBorderThickness(CIwVec2(5, 5));
    stringSprite->SetPadding(CIwVec2(2, 2));
    stringSprite->SetBaseLocation(CIwVec2(300, 300));
    mgr->RegisterGameObject(stringSprite);

    sprintf(buffer, "Next string");
    stringSprite = new TextSprite(mgr, buffer);
    stringSprite->SetBackgroundColor(g_IwGxColours[IW_GX_COLOUR_WHITE]);
    stringSprite->SetBorderColor(g_IwGxColours[IW_GX_COLOUR_BLACK]);
    stringSprite->SetMargin(CIwVec2(4, 4));
    stringSprite->SetBorderThickness(CIwVec2(4, 4));
    stringSprite->SetPadding(CIwVec2(4, 4));
    stringSprite->SetBaseLocation(CIwVec2(400, 400));
    mgr->RegisterGameObject(stringSprite);

    state->SubscribeStateVarChange(INT_VAR_HANDLE, stateVarChanged, intSprite);
    state->SubscribeStateVarChange(FLOAT_VAR_HANDLE, stateVarChanged, floatSprite);

    state->SubscribeEvent("FloatChange", changeVarValue, NULL);

    state->PostFutureEvent("FloatChange", NULL, mgr->GetCurrentClockTime() + 3000);
}








void MakeBezierSprite(GameManager *mgr, uint32 marker_handle, int x, int y, int dx, int dy, float scale)
{
    PathFollowingSprite *path = new PathFollowingSprite(mgr);
    path->SetImage(mgr->GetResourceManager()->GetImage(marker_handle));
    path->SetBaseLocation(CIwVec2(x, y));
    path->AddTransformation(new AnchorTransformation());
    path->AddTransformation(new ScaleTransformation(scale));
    mgr->RegisterGameObject(path);

    path->SetTotalPathTimeMillis(10000);
    path->AddSegment(new PathSegmentBezier(CIwVec2(x, y), CIwVec2(x, y + dy), CIwVec2(x + dx, y + dy), CIwVec2(x + dx, y)));
    path->AddSegment(new PathSegmentLine(CIwVec2(x + dx, y), CIwVec2(x + dx, y + dy)));
    path->AddSegment(new PathSegmentBezier(CIwVec2(x + dx, y + dy), CIwVec2(x - dx, y), CIwVec2(x + dx * 2, y), CIwVec2(x, y + dy)));
    path->AddSegment(new PathSegmentLine(CIwVec2(x, y + dy), CIwVec2(x, y)));
}

void PathTest(GameManager *mgr, uint32 marker_handle, uint32 width, uint32 height)
{
    MakeBezierSprite(mgr, marker_handle, 50, 50, width - 100, height - 100, 1);
}

void ManyBezPathTest(GameManager *mgr, uint32 marker_handle, uint32 width, uint32 height)
{
    int maxX = 50;
    int maxY = 50;

    int deltaX = width / maxX;
    int startX = deltaX / 2;
    int deltaY = height / maxY;
    int startY = deltaY / 2;

    for(int x = 0; x < maxX; x++)
    {
        for(int y = 0; y < maxY; y++)
        {
            MakeBezierSprite(mgr, marker_handle, startX + deltaX * x, startY + deltaY * y, deltaX / 2, deltaY / 2, 0.5);
        }
    }
}

void MakeSquareSprite(GameManager *mgr, uint32 marker_handle, int x, int y, int dx, int dy)
{
    PathFollowingSprite *path = new PathFollowingSprite(mgr);
    path->SetImage(mgr->GetResourceManager()->GetImage(marker_handle));
    path->SetBaseLocation(CIwVec2(x, y));
    path->AddTransformation(new AnchorTransformation());
    path->AddTransformation(new ScaleTransformation(0.25));
    mgr->RegisterGameObject(path);

    path->SetPathSpeedPixelsPerSecond(500);
    path->AddSegment(new PathSegmentLine(CIwVec2(dx, 0)));
    path->AddSegment(new PathSegmentLine(CIwVec2(0, dy)));
    path->AddSegment(new PathSegmentLine(CIwVec2(-dx, 0)));
    path->AddSegment(new PathSegmentLine(CIwVec2(0, -dy)));
}

void ManySquarePathTest(GameManager *mgr, uint32 marker_handle, uint32 width, uint32 height)
{
    int maxX = 30;
    int maxY = 30;

    int deltaX = width / maxX;
    int startX = deltaX / 4;
    int deltaY = height / maxY;
    int startY = deltaY / 4;

    for(int x = 0; x < maxX; x++)
    {
        for(int y = 0; y < maxY; y++)
        {
            MakeSquareSprite(mgr, marker_handle, startX + deltaX * x, startY + deltaY * y, deltaX / 2, deltaY / 2);
        }
    }
}

void LabeledSprite(GameManager *mgr, uint32 marker_handle, uint32 width, uint32 height)
{
    Sprite *image = new Sprite(mgr);
    image->SetImage(mgr->GetResourceManager()->GetImage(ARROW_HANDLE));
    image->AddTransformation(new AnchorTransformation());
    image->SetBaseLocation(CIwVec2(mgr->GetSurfaceWidth() / 2, mgr->GetSurfaceHeight() / 2));
    OrbitTransformation *orbiter = new OrbitTransformation(150, 5000, 0, CounterClockwise);
    image->AddTransformation(orbiter);
    mgr->RegisterGameObject(image);

    TextSprite *text = new TextSprite(mgr, (char *)"Normal");
    text->AddTransformation(new AnchorTransformation());
    text->AddTransformation(new AnchorToTransformation(image, image, (float)(text->GetHeight() / 2 + image->GetHeight() / 2 + 10), 0, RelativeToNormal));
    text->AddTransformation(new OrientToTransformation((IDirectional*)image, OrientAlongOrientation));
    mgr->RegisterGameObject(text);
}

void AnimationTest(GameManager *mgr, uint32 marker_handle, uint32 width, uint32 height)
{
    FlipbookSprite *tmp = new FlipbookSprite(mgr);
    tmp->SetAnimationParams(mgr->GetResourceManager()->GetImage(FIRE_HANDLE), 64, 48, 20, 20);
    tmp->SetBaseLocation(CIwVec2(mgr->GetSurfaceWidth() / 2, mgr->GetSurfaceHeight() / 2));
    tmp->AddTransformation(new AnchorTransformation());
    OrbitTransformation *orbiter = new OrbitTransformation(150, 5000, 0, Clockwise);
    tmp->AddTransformation(orbiter);
    mgr->RegisterGameObject(tmp);
}

int32 ResetSprite(void *systemData, void *userData)
{
    GameManager *mgr;

    if(systemData != NULL)
    {
        Sprite *spritePtr = (Sprite *)systemData;

        mgr = spritePtr->GetManager();
        mgr->RemoveGameObject(spritePtr);
        delete spritePtr;
    }
    else
    {
        mgr = (GameManager*)userData;
    }

    Sprite *tmp = new Sprite(mgr);
    tmp->SetImage(mgr->GetResourceManager()->GetImage(ARROW_HANDLE));
    tmp->SetBaseLocation(CIwVec2(mgr->GetSurfaceWidth() / 2, mgr->GetSurfaceHeight() / 2));
    tmp->AddTransformation(new AnchorTransformation());
    OrbitTransformation *orbiter = new OrbitTransformation(150, 5000, 0, Clockwise);
    tmp->AddTransformation(orbiter);
    mgr->RegisterGameObject(tmp);

    tmp->SetLifespan(++resetCount * 500);
    tmp->SetCompletionCallback(ResetSprite, NULL);

    return 0;
}

void DoLifetimeTest(GameManager *mgr, uint32 marker_handle, uint32 width, uint32 height)
{
    ResetSprite(NULL, mgr);
}

void DoManyShipTest(GameManager *mgr, int maxX, int maxY, int width, int height, uint32 marker_handle)
{
        int deltaX = width / maxX;
        int startX = deltaX / 2;
        int deltaY = height / maxY;
        int startY = deltaY / 2;

        for(int x = 0; x < maxX; x++)
        {
            for(int y = 0; y < maxY; y++)
            {
                MakeNormalShip(mgr, marker_handle, startX + deltaX * x, startY + deltaY * y);
            }
        }
}

void MakeNormalShip(GameManager *mgr, int marker_handle, int x, int y)
{
    Sprite *bug1 = new Sprite(mgr);
    bug1->SetImage(mgr->GetResourceManager()->GetImage(marker_handle));
    bug1->SetBaseLocation(CIwVec2(x, y));
    bug1->AddTransformation(new AnchorTransformation());
    OrbitTransformation *orbiter = new OrbitTransformation(0, 5000, 0, Clockwise);
    bug1->AddTransformation(orbiter);
    mgr->RegisterGameObject(bug1);
    Sprite *anchorBug = bug1;

    bug1 = new Sprite(mgr);
    bug1->SetImage(mgr->GetResourceManager()->GetImage(marker_handle));
    bug1->AddTransformation(new AnchorTransformation());
    bug1->AddTransformation(new AnchorToTransformation(anchorBug, anchorBug, 70, 60));
    bug1->AddTransformation(new ScaleTransformation(0.3f));
    orbiter = new OrbitTransformation(20, 2000, 0);
    bug1->AddTransformation(orbiter);
    bug1->AddTransformation(new OrientToTransformation(orbiter, OrientAlongNormal));
    mgr->RegisterGameObject(bug1);

    bug1 = new Sprite(mgr);
    bug1->SetImage(mgr->GetResourceManager()->GetImage(marker_handle));
    bug1->AddTransformation(new AnchorTransformation());
    bug1->AddTransformation(new AnchorToTransformation(anchorBug, anchorBug, 70, 60));
    bug1->AddTransformation(new ScaleTransformation(0.3f));
    orbiter = new OrbitTransformation(20, 2000, 120);
    bug1->AddTransformation(orbiter);
    bug1->AddTransformation(new OrientToTransformation(orbiter, OrientAlongNormal));
    mgr->RegisterGameObject(bug1);

    bug1 = new Sprite(mgr);
    bug1->SetImage(mgr->GetResourceManager()->GetImage(marker_handle));
    bug1->AddTransformation(new AnchorTransformation());
    bug1->AddTransformation(new AnchorToTransformation(anchorBug, anchorBug, 70, 60));
    bug1->AddTransformation(new ScaleTransformation(0.3f));
    orbiter = new OrbitTransformation(20, 2000, -120);
    bug1->AddTransformation(orbiter);
    bug1->AddTransformation(new OrientToTransformation(orbiter, OrientAlongNormal));
    mgr->RegisterGameObject(bug1);

    bug1 = new Sprite(mgr);
    bug1->SetImage(mgr->GetResourceManager()->GetImage(marker_handle));
    bug1->AddTransformation(new AnchorTransformation());
    bug1->AddTransformation(new AnchorToTransformation(anchorBug, anchorBug, 70, -60));
    bug1->AddTransformation(new ScaleTransformation(0.3f));
    orbiter = new OrbitTransformation(20, 2000, 0);
    bug1->AddTransformation(orbiter);
    bug1->AddTransformation(new OrientToTransformation(orbiter, OrientAlongNormal));
    mgr->RegisterGameObject(bug1);

    bug1 = new Sprite(mgr);
    bug1->SetImage(mgr->GetResourceManager()->GetImage(marker_handle));
    bug1->AddTransformation(new AnchorTransformation());
    bug1->AddTransformation(new AnchorToTransformation(anchorBug, anchorBug, 70, -60));
    bug1->AddTransformation(new ScaleTransformation(0.3f));
    orbiter = new OrbitTransformation(20, 2000, 120);
    bug1->AddTransformation(orbiter);
    bug1->AddTransformation(new OrientToTransformation(orbiter, OrientAlongNormal));
    mgr->RegisterGameObject(bug1);

    bug1 = new Sprite(mgr);
    bug1->SetImage(mgr->GetResourceManager()->GetImage(marker_handle));
    bug1->AddTransformation(new AnchorTransformation());
    bug1->AddTransformation(new AnchorToTransformation(anchorBug, anchorBug, 70, -60));
    bug1->AddTransformation(new ScaleTransformation(0.3f));
    orbiter = new OrbitTransformation(20, 2000, -120);
    bug1->AddTransformation(orbiter);
    bug1->AddTransformation(new OrientToTransformation(orbiter, OrientAlongNormal));
    mgr->RegisterGameObject(bug1);
}

void MakeOrbiters(GameManager *mgr, int x, int y, uint32 marker_handle)
{
        Sprite *bug1 = new Sprite(mgr);
    bug1->SetImage(mgr->GetResourceManager()->GetImage(marker_handle));
    bug1->SetBaseLocation(CIwVec2(x, y));
    bug1->AddTransformation(new AnchorTransformation());
    OrbitTransformation *orbiter = new OrbitTransformation(200, 15000);
    bug1->AddTransformation(orbiter);
    orbiter = new OrbitTransformation(70, 7000);
    bug1->AddTransformation(orbiter);
    mgr->RegisterGameObject(bug1);
    Sprite *anchorBug = bug1;
    ILocationSource *locBug = bug1;

    bug1 = new Sprite(mgr);
    bug1->SetImage(mgr->GetResourceManager()->GetImage(marker_handle));
    bug1->AddTransformation(new AnchorTransformation());
    bug1->AddTransformation(new AnchorToTransformation(anchorBug, CIwVec2(-56, 56)));
    bug1->AddTransformation(new ScaleTransformation(0.3f));
    orbiter = new OrbitTransformation(40, 2000);
    bug1->AddTransformation(orbiter);
    mgr->RegisterGameObject(bug1);

    bug1 = new Sprite(mgr);
    bug1->SetImage(mgr->GetResourceManager()->GetImage(marker_handle));
    bug1->AddTransformation(new AnchorTransformation());
    bug1->AddTransformation(new AnchorToTransformation(anchorBug, CIwVec2(-56, -56)));
    bug1->AddTransformation(new ScaleTransformation(0.3f));
    orbiter = new OrbitTransformation(40, 2000);
    bug1->AddTransformation(orbiter);
    mgr->RegisterGameObject(bug1);

    bug1 = new Sprite(mgr);
    bug1->SetImage(mgr->GetResourceManager()->GetImage(marker_handle));
    bug1->AddTransformation(new AnchorTransformation());
    bug1->AddTransformation(new AnchorToTransformation(anchorBug, CIwVec2(80, 0)));
    bug1->AddTransformation(new ScaleTransformation(0.3f));
    orbiter = new OrbitTransformation(40, 2000, 180);
    bug1->AddTransformation(orbiter);
    mgr->RegisterGameObject(bug1);

    bug1 = new Sprite(mgr);
    bug1->SetImage(mgr->GetResourceManager()->GetImage(marker_handle));
    bug1->SetBaseLocation(CIwVec2(x, y));
    bug1->AddTransformation(new AnchorTransformation());
    bug1->AddTransformation(new OrientToTransformation(locBug, OrientToward));
    mgr->RegisterGameObject(bug1);

    bug1 = new Sprite(mgr);
    bug1->SetImage(mgr->GetResourceManager()->GetImage(marker_handle));
    bug1->SetBaseLocation(CIwVec2(x / 2, y));
    bug1->AddTransformation(new AnchorTransformation());
    bug1->AddTransformation(new OrientToTransformation(locBug, OrientToward));
    mgr->RegisterGameObject(bug1);

    bug1 = new Sprite(mgr);
    bug1->SetImage(mgr->GetResourceManager()->GetImage(marker_handle));
    bug1->SetBaseLocation(CIwVec2(x, y/2));
    bug1->AddTransformation(new AnchorTransformation());
    bug1->AddTransformation(new OrientToTransformation(locBug, OrientToward));
    mgr->RegisterGameObject(bug1);

    bug1 = new Sprite(mgr);
    bug1->SetImage(mgr->GetResourceManager()->GetImage(marker_handle));
    bug1->SetBaseLocation(CIwVec2(x, y * 3 / 2));
    bug1->AddTransformation(new AnchorTransformation());
    bug1->AddTransformation(new OrientToTransformation(locBug, OrientToward));
    mgr->RegisterGameObject(bug1);

    bug1 = new Sprite(mgr);
    bug1->SetImage(mgr->GetResourceManager()->GetImage(marker_handle));
    bug1->SetBaseLocation(CIwVec2(x * 3 / 2, y));
    bug1->AddTransformation(new AnchorTransformation());
    bug1->AddTransformation(new OrientToTransformation(locBug, OrientToward));
    mgr->RegisterGameObject(bug1);
}

