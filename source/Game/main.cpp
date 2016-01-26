#include <time.h>
#include <stdlib.h>
#include <vector>

#include "s3e.h"
#include "Iw2D.h"

#include "GameVariables.h"

#include "GameManager.h"
#include "AllSprites.h"
#include "AllTransforms.h"

#include "Bug.h"
#include "PointsManager.h"
#include "PhonemeManager.h"
#include "MainMenu.h"
#include "LevelSelection.h"
#include "BugsLevel.h"
#include "ApplesLevel.h"
#include "SandboxLevel.h"
#include "WallLevel.h"
#include "LevelSummary.h"

int VAR_POINTS;
int VAR_CURRENT_PHONEME;
int VAR_CURRENT_THINGY_COUNT;
int VAR_CURRENT_CORRECT_THINGY_COUNT;

int IMAGE_MAIN_MENU;
int IMAGE_LEVEL_SELECT;
int IMAGE_CHANGE_PLAYER_BUTTON;
int IMAGE_START_GAME_BUTTON;
int IMAGE_PLAY_BUGS_BUTTON;
int IMAGE_PLAY_APPLES_BUTTON;
int IMAGE_PLAY_SANDBOX_BUTTON;
int IMAGE_PLAY_WALL_BUTTON;
int IMAGE_MAIN_MENU_BUTTON;

int IMAGE_LADYBUG;
int IMAGE_SPLAT;
int IMAGE_BUGS_BACKGROUND;

int IMAGE_APPLES_BACKGROUND;
int IMAGE_APPLE;

int IMAGE_SANDBOX_BACKGROUND;
int IMAGE_SANDCASTLE;
int IMAGE_DUMPTRUCK_FULL;
int IMAGE_DUMPTRUCK_EMPTY;

int IMAGE_WALL_BACKGROUND;
int IMAGE_BRICK1;
int IMAGE_BRICK2;
int IMAGE_BRICK3;
int IMAGE_BRICK_TARGET;

int IMAGE_STAR_0;
int IMAGE_STAR_1;
int IMAGE_STAR_2;
int IMAGE_STAR_3;
int IMAGE_STAR_4;
int IMAGE_STAR_5;

int FONT_TREB_16;
int FONT_TREB_24;
int FONT_TREB_36;

int FONT_COOPER_18;
int FONT_COOPER_24;
int FONT_COOPER_48;

int SOUND_DING;
int SOUND_BUZZ;

int STATE_STARTUP;
int STATE_MAIN_MENU;
int STATE_LEVEL_SELECT;

int STATE_BUGS_PLAYING;
int STATE_BUGS_SUMMARY;

int STATE_APPLES_PLAYING;
int STATE_APPLES_SUMMARY;

int STATE_SANDBOX_PLAYING;
int STATE_SANDBOX_SUMMARY;

int STATE_WALL_PLAYING;
int STATE_WALL_SUMMARY;

char EVENT_APPLE_DROP[50];
char EVENT_NEW_DUMPTRUCK[50];

WordBuilder *wordBuilder = null;
LevelSummary *levelSummary = null;

S3E_MAIN_DECL void IwMain()
{
    strcpy(EVENT_APPLE_DROP, "AppleDrop");
    strcpy(EVENT_NEW_DUMPTRUCK, "NewDumptruck");
    srand(time(null));

    wordBuilder = new WordBuilder;

    GameManager *mgr;
    mgr = new GameManager();

    mgr->SetRequestedFPS(100);

    mgr->GetResourceManager()->RegisterResourceGroup("images.group");

    IMAGE_MAIN_MENU = mgr->GetResourceManager()->RegisterImage("MainMenu");
    IMAGE_CHANGE_PLAYER_BUTTON = mgr->GetResourceManager()->RegisterImage("ChangePlayerButton");
    IMAGE_START_GAME_BUTTON = mgr->GetResourceManager()->RegisterImage("StartGameButton");
    IMAGE_LEVEL_SELECT = mgr->GetResourceManager()->RegisterImage("LevelSelectionScreen");
    IMAGE_PLAY_BUGS_BUTTON = mgr->GetResourceManager()->RegisterImage("BogsIcon");
    IMAGE_PLAY_APPLES_BUTTON = mgr->GetResourceManager()->RegisterImage("ApplesIcon");
    IMAGE_PLAY_SANDBOX_BUTTON = mgr->GetResourceManager()->RegisterImage("SandboxIcon");
    IMAGE_PLAY_WALL_BUTTON = mgr->GetResourceManager()->RegisterImage("WallIcon");
    IMAGE_MAIN_MENU_BUTTON = mgr->GetResourceManager()->RegisterImage("MainMenuButton");

    IMAGE_LADYBUG = mgr->GetResourceManager()->RegisterImage("ladybug");
    IMAGE_SPLAT = mgr->GetResourceManager()->RegisterImage("splat");
    IMAGE_BUGS_BACKGROUND = mgr->GetResourceManager()->RegisterImage("bugs_background");

    IMAGE_APPLES_BACKGROUND = mgr->GetResourceManager()->RegisterImage("apples_background");
    IMAGE_APPLE = mgr->GetResourceManager()->RegisterImage("apple");

    IMAGE_SANDBOX_BACKGROUND = mgr->GetResourceManager()->RegisterImage("sandbox_background");
    IMAGE_SANDCASTLE = mgr->GetResourceManager()->RegisterImage("sandcastle");
    IMAGE_DUMPTRUCK_FULL = mgr->GetResourceManager()->RegisterImage("dumptruck_full");
    IMAGE_DUMPTRUCK_EMPTY = mgr->GetResourceManager()->RegisterImage("dumptruck_empty");

    IMAGE_WALL_BACKGROUND = mgr->GetResourceManager()->RegisterImage("wall_background");
    IMAGE_BRICK1 = mgr->GetResourceManager()->RegisterImage("brick1");
    IMAGE_BRICK2 = mgr->GetResourceManager()->RegisterImage("brick2");
    IMAGE_BRICK3 = mgr->GetResourceManager()->RegisterImage("brick3");
    IMAGE_BRICK_TARGET = mgr->GetResourceManager()->RegisterImage("brick_target");

    IMAGE_STAR_0 = mgr->GetResourceManager()->RegisterImage("star_0");
    IMAGE_STAR_1 = mgr->GetResourceManager()->RegisterImage("star_1");
    IMAGE_STAR_2 = mgr->GetResourceManager()->RegisterImage("star_2");
    IMAGE_STAR_3 = mgr->GetResourceManager()->RegisterImage("star_3");
    IMAGE_STAR_4 = mgr->GetResourceManager()->RegisterImage("star_4");
    IMAGE_STAR_5 = mgr->GetResourceManager()->RegisterImage("star_5");

    FONT_TREB_16 = mgr->GetResourceManager()->RegisterFont("trebuchet16pt");
    FONT_TREB_24 = mgr->GetResourceManager()->RegisterFont("trebuchet24pt");
    FONT_TREB_36 = mgr->GetResourceManager()->RegisterFont("trebuchet36pt");

    FONT_COOPER_18 = mgr->GetResourceManager()->RegisterFont("cooper_18pt");
    FONT_COOPER_24 = mgr->GetResourceManager()->RegisterFont("cooper_24pt");
    FONT_COOPER_48 = mgr->GetResourceManager()->RegisterFont("cooper_48pt");

    SOUND_DING = mgr->GetResourceManager()->RegisterAudio("DING.wav");
    SOUND_BUZZ = mgr->GetResourceManager()->RegisterAudio("BUZZ.wav");

    GameManager::SetDebugFont(mgr->GetResourceManager()->GetFont(FONT_TREB_16));

    StateManager *state = mgr->GetStateManager();

    STATE_STARTUP = state->RegisterGameState("Startup");
    STATE_MAIN_MENU = state->RegisterGameState("MainMenu");
    STATE_LEVEL_SELECT = state->RegisterGameState("LevelSelect");

    STATE_BUGS_PLAYING = state->RegisterGameState("BugsPlaying");
    STATE_BUGS_SUMMARY = state->RegisterGameState("BugsSummary");

    STATE_APPLES_PLAYING = state->RegisterGameState("ApplesPlaying");
    STATE_APPLES_SUMMARY = state->RegisterGameState("ApplesSummary");

    STATE_SANDBOX_PLAYING = state->RegisterGameState("SandboxPlaying");
    STATE_SANDBOX_SUMMARY = state->RegisterGameState("SandboxSummary");

    STATE_WALL_PLAYING = state->RegisterGameState("WallPlaying");
    STATE_WALL_SUMMARY = state->RegisterGameState("WallSummary");

    state->DefineValidStateTransition(STATE_STARTUP, STATE_MAIN_MENU);
    state->DefineValidStateTransition(STATE_MAIN_MENU, STATE_LEVEL_SELECT);
    state->DefineValidStateTransition(STATE_LEVEL_SELECT, STATE_MAIN_MENU);

    state->DefineValidStateTransition(STATE_LEVEL_SELECT, STATE_BUGS_PLAYING);
    state->DefineValidStateTransition(STATE_BUGS_PLAYING, STATE_BUGS_SUMMARY);
    state->DefineValidStateTransition(STATE_BUGS_SUMMARY, STATE_LEVEL_SELECT);

    state->DefineValidStateTransition(STATE_LEVEL_SELECT, STATE_APPLES_PLAYING);
    state->DefineValidStateTransition(STATE_APPLES_PLAYING, STATE_APPLES_SUMMARY);
    state->DefineValidStateTransition(STATE_APPLES_SUMMARY, STATE_LEVEL_SELECT);

    state->DefineValidStateTransition(STATE_LEVEL_SELECT, STATE_SANDBOX_PLAYING);
    state->DefineValidStateTransition(STATE_SANDBOX_PLAYING, STATE_SANDBOX_SUMMARY);
    state->DefineValidStateTransition(STATE_SANDBOX_SUMMARY, STATE_LEVEL_SELECT);

    state->DefineValidStateTransition(STATE_LEVEL_SELECT, STATE_WALL_PLAYING);
    state->DefineValidStateTransition(STATE_WALL_PLAYING, STATE_WALL_SUMMARY);
    state->DefineValidStateTransition(STATE_WALL_SUMMARY, STATE_LEVEL_SELECT);

    state->SetInitialState(STATE_STARTUP);

    StateVariable initialValue;

    initialValue.intValue = 0;
    VAR_POINTS = state->RegisterVariable("GamePoints", initialValue);
    VAR_CURRENT_THINGY_COUNT = state->RegisterVariable("CurrentBugCount", initialValue);
    VAR_CURRENT_CORRECT_THINGY_COUNT = state->RegisterVariable("CorrectBugCount", initialValue);

    initialValue.stringValue = null;
    VAR_CURRENT_PHONEME = state->RegisterVariable("CurrentPhoneme", initialValue);

    // These objects do not register, so we have to delete them ourselves
    MainMenu *mainMenu = new MainMenu(mgr);
    LevelSelection *levelSelect = new LevelSelection(mgr);
    BugsLevel *bugsLevel = new BugsLevel(mgr);
    ApplesLevel *applesLevel = new ApplesLevel(mgr);
    SandboxLevel *sandboxLevel = new SandboxLevel(mgr);
    WallLevel *wallLevel = new WallLevel(mgr);

    levelSummary = new LevelSummary(mgr);

    state->ChangeState(STATE_MAIN_MENU);

    mgr->MainLoop();

    delete levelSummary;
    delete wallLevel;
    delete sandboxLevel;
    delete applesLevel;
    delete bugsLevel;
    delete levelSelect;
    delete mainMenu;

    delete mgr;
    delete wordBuilder;
}

#ifdef TWEEN_TESTING
void GenTest(GameManager *mgr, int imageHandle, int x, int y, EasingFunction func, EasingDirection dir)
{
    Sprite *markerSprite = new Sprite(mgr);
    markerSprite->SetImage(mgr->GetResourceManager()->GetImage(imageHandle));
    markerSprite->SetBaseLocationSource(new LocationSourceDualTween(
        new IntSourceTween(30 + x * 100, 120 + x * 100, EasingSettings(30000, EasingFunctionLinear, EasingDirectionIn)),
        new IntSourceTween(120 + y * 100, 30 + y * 100, EasingSettings(30000, func, dir))
        ));
    mgr->RegisterGameObject(markerSprite);
}


    int MARKER = mgr->GetResourceManager()->RegisterImage("dot");

    int x = 0, y = 0;

    GenTest(mgr, MARKER, x, y++, EasingFunctionLinear, EasingDirectionIn);
    x = 0;

    GenTest(mgr, MARKER, x++, y, EasingFunctionSine, EasingDirectionIn);
    GenTest(mgr, MARKER, x++, y, EasingFunctionSine, EasingDirectionOut);
    GenTest(mgr, MARKER, x++, y, EasingFunctionSine, EasingDirectionInOut);
    GenTest(mgr, MARKER, x++, y, EasingFunctionSine, EasingDirectionOutIn);
    x++;
    GenTest(mgr, MARKER, x++, y, EasingFunctionQuadratic, EasingDirectionIn);
    GenTest(mgr, MARKER, x++, y, EasingFunctionQuadratic, EasingDirectionOut);
    GenTest(mgr, MARKER, x++, y, EasingFunctionQuadratic, EasingDirectionInOut);
    GenTest(mgr, MARKER, x++, y++, EasingFunctionQuadratic, EasingDirectionOutIn);
    x = 0;

    GenTest(mgr, MARKER, x++, y, EasingFunctionCubic, EasingDirectionIn);
    GenTest(mgr, MARKER, x++, y, EasingFunctionCubic, EasingDirectionOut);
    GenTest(mgr, MARKER, x++, y, EasingFunctionCubic, EasingDirectionInOut);
    GenTest(mgr, MARKER, x++, y, EasingFunctionCubic, EasingDirectionOutIn);
    x++;
    GenTest(mgr, MARKER, x++, y, EasingFunctionQuartic, EasingDirectionIn);
    GenTest(mgr, MARKER, x++, y, EasingFunctionQuartic, EasingDirectionOut);
    GenTest(mgr, MARKER, x++, y, EasingFunctionQuartic, EasingDirectionInOut);
    GenTest(mgr, MARKER, x++, y++, EasingFunctionQuartic, EasingDirectionOutIn);
    x = 0;

    GenTest(mgr, MARKER, x++, y, EasingFunctionQuintic, EasingDirectionIn);
    GenTest(mgr, MARKER, x++, y, EasingFunctionQuintic, EasingDirectionOut);
    GenTest(mgr, MARKER, x++, y, EasingFunctionQuintic, EasingDirectionInOut);
    GenTest(mgr, MARKER, x++, y, EasingFunctionQuintic, EasingDirectionOutIn);
    x++;
    GenTest(mgr, MARKER, x++, y, EasingFunctionExponential, EasingDirectionIn);
    GenTest(mgr, MARKER, x++, y, EasingFunctionExponential, EasingDirectionOut);
    GenTest(mgr, MARKER, x++, y, EasingFunctionExponential, EasingDirectionInOut);
    GenTest(mgr, MARKER, x++, y++, EasingFunctionExponential, EasingDirectionOutIn);
    x = 0;

    GenTest(mgr, MARKER, x++, y, EasingFunctionCircular, EasingDirectionIn);
    GenTest(mgr, MARKER, x++, y, EasingFunctionCircular, EasingDirectionOut);
    GenTest(mgr, MARKER, x++, y, EasingFunctionCircular, EasingDirectionInOut);
    GenTest(mgr, MARKER, x++, y, EasingFunctionCircular, EasingDirectionOutIn);
    x++;
    GenTest(mgr, MARKER, x++, y, EasingFunctionElastic, EasingDirectionIn);
    GenTest(mgr, MARKER, x++, y, EasingFunctionElastic, EasingDirectionOut);
    GenTest(mgr, MARKER, x++, y, EasingFunctionElastic, EasingDirectionInOut);
    GenTest(mgr, MARKER, x++, y++, EasingFunctionElastic, EasingDirectionOutIn);
    x = 0;

    GenTest(mgr, MARKER, x++, y, EasingFunctionBack, EasingDirectionIn);
    GenTest(mgr, MARKER, x++, y, EasingFunctionBack, EasingDirectionOut);
    GenTest(mgr, MARKER, x++, y, EasingFunctionBack, EasingDirectionInOut);
    GenTest(mgr, MARKER, x++, y, EasingFunctionBack, EasingDirectionOutIn);
    x++;
    GenTest(mgr, MARKER, x++, y, EasingFunctionBounce, EasingDirectionIn);
    GenTest(mgr, MARKER, x++, y, EasingFunctionBounce, EasingDirectionOut);
    GenTest(mgr, MARKER, x++, y, EasingFunctionBounce, EasingDirectionInOut);
    GenTest(mgr, MARKER, x++, y++, EasingFunctionBounce, EasingDirectionOutIn);
    x = 0;

    mgr->MainLoop();
#endif
