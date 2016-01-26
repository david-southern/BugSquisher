#include "WordBuilder.h"
#include "LevelSummary.h"

#define POINTS_CHANGE_EVENT "PointsChangeEvent"

extern int APPLE_COUNT;

extern int BUG_COUNT;
extern int BUG_SPEED;

extern int VAR_POINTS;
extern int VAR_CURRENT_PHONEME;
extern int VAR_CURRENT_THINGY_COUNT;
extern int VAR_CURRENT_CORRECT_THINGY_COUNT;

extern int IMAGE_MAIN_MENU;
extern int IMAGE_LEVEL_SELECT;
extern int IMAGE_CHANGE_PLAYER_BUTTON;
extern int IMAGE_START_GAME_BUTTON;
extern int IMAGE_PLAY_BUGS_BUTTON;
extern int IMAGE_PLAY_APPLES_BUTTON;
extern int IMAGE_PLAY_SANDBOX_BUTTON;
extern int IMAGE_PLAY_WALL_BUTTON;
extern int IMAGE_MAIN_MENU_BUTTON;

extern int IMAGE_LADYBUG;
extern int IMAGE_SPLAT;
extern int IMAGE_BUGS_BACKGROUND;

extern int IMAGE_APPLES_BACKGROUND;
extern int IMAGE_APPLE;

extern int IMAGE_SANDBOX_BACKGROUND;
extern int IMAGE_SANDCASTLE;
extern int IMAGE_DUMPTRUCK_FULL;
extern int IMAGE_DUMPTRUCK_EMPTY;

extern int IMAGE_WALL_BACKGROUND;
extern int IMAGE_BRICK1;
extern int IMAGE_BRICK2;
extern int IMAGE_BRICK3;
extern int IMAGE_BRICK_TARGET;

extern int IMAGE_STAR_0;
extern int IMAGE_STAR_1;
extern int IMAGE_STAR_2;
extern int IMAGE_STAR_3;
extern int IMAGE_STAR_4;
extern int IMAGE_STAR_5;

extern int FONT_TREB_16;
extern int FONT_TREB_24;
extern int FONT_TREB_36;

extern int FONT_COOPER_18;
extern int FONT_COOPER_24;
extern int FONT_COOPER_48;

extern int SOUND_DING;
extern int SOUND_BUZZ;

extern int STATE_MAIN_MENU;
extern int STATE_LEVEL_SELECT;

extern int STATE_BUGS_PLAYING;
extern int STATE_BUGS_SUMMARY;

extern int STATE_APPLES_PLAYING;
extern int STATE_APPLES_SUMMARY;

extern int STATE_SANDBOX_PLAYING;
extern int STATE_SANDBOX_SUMMARY;

extern int STATE_WALL_PLAYING;
extern int STATE_WALL_SUMMARY;

extern char EVENT_APPLE_DROP[50];
extern char EVENT_NEW_DUMPTRUCK[50];

extern WordBuilder *wordBuilder;
extern LevelSummary *levelSummary;
