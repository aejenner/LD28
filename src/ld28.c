#define ALLEGRO_STATICLINK
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <strings.h>
#include <signal.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#define SAMPLE_COUNT 5
#define DISPLAY_HEIGHT 640
#define DISPLAY_WIDTH 480

#define FONT_NAME "./averiagruesalibre.ttf"
#define IMAGE_CITYMAP "./citymap.jpg"
#define MUSIC_PEAR "./bu-a-pears-demoscene.it"
#define MUSIC_BIRD "./bu-the-birds-sweets.it"
#define MUSIC_CHEAP "./bu-cheap-and-walking.it"
#define MUSIC_AGE "./bu-age-of-bodies.it"
#define SOUND_BIG "./clickbig.wav"
#define SOUND_SMALL "./clicksmall.wav"

#define TEXT_TITLE "\"Yogo Saves the World\""
#define TEXT_DISPLAY_TITLE "LD28: Yogo Saves the World"
#define TEXT_STORY1 "\"Some catastrophe occurred.\""
#define TEXT_STORY2 "\"We aren't sure what.\""
#define TEXT_STORY3 "\"But we do know this:\""
#define TEXT_STORY4 "\"The world will end in one day.\""
#define TEXT_STORY5 "\"You only get one day to save everyone.\""
#define TEXT_STORY6 "\"We probably should have woke you sooner.\""
#define TEXT_STORY7 "\"To be honest, you weren't our first choice.\""
#define TEXT_STORY8 "\"Or second choice. Anyway do your best!\""

#define TEXT_START "(Press any key to start)"
#define TEXT_FAIL "The world fades..."
#define TEXT_FAIL_UNCON "You never awaken."
#define TEXT_DRUNK0 "Okay"
#define TEXT_DRUNK1 "Tipsy"
#define TEXT_DRUNK2 "Sauced"
#define TEXT_DRUNK3 "Drunk"
#define TEXT_DRUNK4 "Wasted"
#define TEXT_DRUNK5 "Unconscious"
#define TEXT_DRUNK_FAIL1 "Whoops."
#define TEXT_DRUNK_FAIL2 "Uh oh."
#define TEXT_DRUNK_FAIL3 "My head hurts..."

#define TEXT_NAME "Yogo"
#define TEXT_NAME_FULL "Tensec Yogo"

#define TEXT_PLACE_CITY "City Map"
#define TEXT_PLACE_APT "Appartment 802"
#define TEXT_PLACE_PLAZA1 "Ivy Plaza"
#define TEXT_PLACE_PLAZA2 "Riverside Plaza"
#define TEXT_PLACE_PARK "The Only Park"
#define TEXT_PLACE_BAR "SWAG Bar & Grill"
#define TEXT_PLACE_SHOP1 "Lunar Imports"
#define TEXT_PLACE_SHOP2 "Wheel of Sales"
#define TEXT_PLACE_STREET "Streets"
#define TEXT_PLACE_SEWERS "Nasty Sewers"
#define TEXT_PLACE_GATE "East Gate"

#define TEXT_TALK_TO "Talk to:"

#define TEXT_TIME "Time: "
#define TEXT_STATUS "Status: "

#define START_TIME 34343.0

//typedefs
typedef struct ActorStruct* Actor;
typedef struct Vec2Struct* Vec2;
typedef struct MapNodeStruct* MapNode;
typedef struct MapPathStruct* MapPath;
typedef struct MapStruct* Map;

//struct defs
struct ActorStruct { //is this needed?
  Vec2 location;
  float rotation;
  int isStatic, isHidden;
  Vec2 startErp, endErp;
  float alpha, duration;
  int isErping;
  void (*tick)(Actor, float);
  void (*draw)(Actor, float);
  void (*created)(Actor);
  void (*destroyed)(Actor);
  void (*clicked)(Actor);
  int mapID;
  Actor owner;
};

struct Vec2Struct { //is this needed?
  float x, y;
};

struct MapNodeStruct {
  Vec2 location;
  char* name;
  int isHidden;
  char* opt1;
  char* opt2;
  char* opt3;
  char* opt4;
  void (*action1)();
  void (*action2)();
  void (*action3)();
  void (*action4)();
};

struct MapPathStruct {
  int start, end;
  float x1, y1, x2, y2;
  int isHidden;
  int weight;
  int type; //edo down to optional
};

struct MapStruct {
  char* name;
  int location; //where the player is
  int numNodes, numPaths;
  MapNode* nodes;
  MapPath* paths;
  void (*draw)(float); //draw bg and map elements
};

//globals
MapNode sewersNode;
MapPath sewersPath;
MapNode doomNode;
MapPath doomPath;
MapNode bossNode;
MapPath bossPath;
float drunkenness;
int uncon;
int status;
double gametime = START_TIME; //in seconds, out of 84400
double gamespeed = 1.0;
int numMaps = 0;
Map* maps;
int activeMap = 0;
int introSequence = 0;
char* message;
double messageAlpha;
float fade;
int fading;
ALLEGRO_EVENT_QUEUE* events;
ALLEGRO_TIMER* introTimer;
int actorsCap = 1;
int thugging = 0;
Actor* actors;
Actor player = NULL;
ALLEGRO_BITMAP* citymap;
ALLEGRO_BITMAP* aptmap;
ALLEGRO_BITMAP* shop1map;
ALLEGRO_BITMAP* shop2map;
ALLEGRO_BITMAP* barmap;
ALLEGRO_BITMAP* sewermap;
ALLEGRO_AUDIO_STREAM* musicBird;
ALLEGRO_AUDIO_STREAM* musicCheap;
ALLEGRO_AUDIO_STREAM* musicPear;
ALLEGRO_AUDIO_STREAM* musicAge;
ALLEGRO_SAMPLE* clickbig;
ALLEGRO_SAMPLE* clicksmall;

//function headers
void segfhandler(int);

Actor makeActor(Vec2, float, int, int, void (*tick)(Actor, float),
                void (*draw)(Actor, float), void (*created)(Actor),
                void (*destroyed)(Actor), void (*clicked)(Actor),
                int, Actor);
int addActor(Actor);
int destroyActor(Actor);

Vec2 vec2_make(float, float);
Vec2 vec2_clone(Vec2);
void vec2_lerp(Vec2, Vec2, Vec2, float);
void vec2_smoothstep(Vec2, Vec2, Vec2, float);
void vec2_add(Vec2, Vec2, Vec2);
float vec2_dot(Vec2, Vec2);
void vec2_divide(Vec2, Vec2, Vec2);
void vec2_scale(Vec2, Vec2, float);

MapNode makeMapNode(float, float, char*, int);
Map makeMap(char*, void (*draw)(float));
MapPath makeMapPath(int, int, int, int, float, float, float, float);
int addMapNode(Map, MapNode);
int addMapPath(Map, MapPath);
int addMap(Map);

void tickActor(Actor, float);
void erpActor(Actor, Vec2, float);

void resetEverything();
void booze();

void drawCityMap(float);
void drawAptMap(float);
void drawBarMap(float);
void drawShop1Map(float);
void drawShop2Map(float);
void drawPlazaMap(float);
void drawSewersMap(float);

void tickPlayer(Actor, float);
void drawPlayer(Actor, float);

void clickHandler(int, int);
void setMessage(char*);

void changeMap(int);

void cityStreetAction();
void cityAptAction();
void cityParkAction();
void cityParkTalkAction();
void cityIvyAction();
void cityIvyStatueAction();
void cityRiverAction();
void cityRiverChatAction();
void cityRiverJumpAction();
void citySewersAction();
void citySewersEnterAction();
void cityShop1Action();
void cityShop1EnterAction();
void cityBarAction();
void cityBarEnterAction();
void barAction();
void barBoozeAction();
void aptMainAction();
void aptBoozeAction();
void aptFoodAction();
void aptMirrorAction();
void aptHandsAction();
void sewersMainAction();
void sewersBossAction();
void sewersBattleAction();
void sewersDoomAction();
void sewersDOOMSDAY();

//functions
void sewersMainAction() {
  changeMap(1);
  gametime += 10;
}

void sewersBossAction() {
  setMessage("There is this really huge thug blocking the way.");
  gametime += 20;
}

void sewersBattleAction() {
  if (drunkenness < 4) {
    setMessage("Oh no, there is no way I'm tangling with that guy.");
  } else {
    setMessage("Woah like, ouch. I beat the thug though I think.");
    gametime += 1800;
    bossNode->opt1 = "";
    bossNode->opt2 = "";
    bossNode->action1 = NULL;
    bossNode->action2 = NULL;
    doomNode->isHidden = 0;
    doomPath->isHidden = 0;
  }
}

void sewersDoomAction() {
  setMessage("A complex device with lots of blinking lights. I'm not sure I can figure it out.");
  gametime += 300;
}
void sewersDOOMSDAY() {
  if (drunkenness < 1) {
    setMessage("Hey I disabled it! The day is saved! THE END");
  } else {
    setMessage("Err... whoops? I think it is going off early!");
    //erpActor(player, player->location, 1000000);
    booze();
    booze();
    booze();
    booze();
    booze();
  }
}

void cityShop1Action() {
  setMessage("I buy a lot of stuff from here.");
  gametime += 10;
}

void cityShop1EnterAction() {
  setMessage("It seems they are closed :(");
  gametime += 10;
}

void cityBarAction() {
  setMessage("Decent booze and never crowded because the food sucks.");
  gametime += 10;
}

void cityBarEnterAction() {
  changeMap(3);
}

void barAction() {
  changeMap(1);
}

void barBoozeAction() {
  setMessage("Finally a decent cocktail.");
  gametime += 300;
  booze();
}

//citySewersAction impl
void citySewersAction() {
  setMessage("This place smells really bad.");
  gametime += 10;
}

//citySewersEnterAction impl
void citySewersEnterAction() {
  changeMap(4);
}

//cityRiverAction impl
void cityRiverAction() {
  setMessage("This small plaza has four coffee shops and two dozen hipsters.");
  gametime += 10;
}

//cityRiverChatAction impl
void cityRiverChatAction() {
  if (drunkenness < 3) {
    setMessage("Wow these people are terriying.");
    gametime += 10;
  } else {
    setMessage("Huh, I never knew about the merits of wearing flat-lensed glasses before.");
    gametime += 300;
  }
}

//cityStreetAction impl
void cityStreetAction() {
  setMessage("There is no way I'm doing that.");
  gametime += 10;
}

//cityRiverJumpAction
void cityRiverJumpAction() {
  if (drunkenness < 2) {
    setMessage("That is a really dumb idea.");
    gametime += 10;
  } else if (drunkenness < 4) {
    setMessage("Hey, there's like something behind the sewers!");
    gametime += 300;
    sewersNode->isHidden = 0;
    sewersPath->isHidden = 0;
  } else {
    drunkenness += 10;
  }
}

//cityParkAction impl
void cityParkAction() {
  setMessage("It is a nice park, but the only one in the city so it's always crowded.");
  gametime += 10;
}

//cityParkTalkAction impl
void cityParkTalkAction() {
  setMessage("Too many! Have to run.");
  gametime += 3000;
  maps[1]->location = 0;
  changeMap(2);
}

//cityIvyAction impl
void cityIvyAction() {
  setMessage("A large plaza. It was named after a war hero I think.");
  gametime += 10;
}

//cityIvyStatueAction impl
void cityIvyStatueAction() {
  setMessage("A statue of some guy. A plaque at the base reads: \"Commander Ividyon.\"");
  gametime += 20;
}

//sigfhandler impl
void segfhandler(int g) {}

//makeActor impl
Actor makeActor(Vec2 location, float rotation, int isStatic, int isHidden,
                void (*tick)(Actor, float),
                void (*draw)(Actor, float), void (*created)(Actor),
                void (*destroyed)(Actor), void (*clicked)(Actor),
                int mapID, Actor owner) {
  Actor a = malloc(sizeof(struct ActorStruct));
  a->location = vec2_clone(location);
  printf("%f %f\n", location->x, location->y);
  a->rotation = rotation;
  a->isStatic = isStatic;
  a->isHidden = isHidden;
  a->startErp = vec2_make(0, 0);
  a->endErp = vec2_make(0, 0);
  a->isErping = 0;
  a->duration = 0;
  a->tick = tick;
  a->draw = draw;
  a->created = created;
  a->destroyed = destroyed;
  a->clicked = clicked;
  a->mapID = mapID;
  a->owner = owner;
  return a;
}

//addActor impl
int addActor(Actor a) {
  int i;
  for (i = 0; i < actorsCap; i++) {
    if (actors[i] == NULL) {
       actors[i] = a;
       return i;
    }
  }

  realloc(actors, sizeof(Actor) * actorsCap * 2);
  for (i = actorsCap; i < actorsCap * 2; i++) {
    actors[i] = NULL;
  }

  i = actorsCap;
  actors[actorsCap] = a;
  actorsCap *= 2;

  if (a->created) a->created(a);

  return i;
}

//destroyActor impl
int destroyActor(Actor a) {
  if (a->destroyed != NULL) a->destroyed(a);
  
  int i;
  for (i = 0; i < actorsCap; i++) {
    if (actors[i] == a) {
      actors[i] = NULL;
      free(a->location);
      free(a->startErp);
      free(a->endErp);
      free(a);
      return i;
    }
  }

  return -1;
}

//vec2_make impl
Vec2 vec2_make(float x, float y) {
   Vec2 v = malloc(sizeof(struct Vec2Struct));
   v->x = x;
   v->y = y;
   return v;
}

//vec2_clone impl
Vec2 vec2_clone(Vec2 v) {
  Vec2 vc = malloc(sizeof(struct Vec2Struct));
  vc->x = v->x;
  vc->y = v->y;
  return vc;
}

//vec2_lerp impl
void vec2_lerp(Vec2 out, Vec2 a, Vec2 b, float alpha) {
  out->x = (1 - alpha) * a->x + alpha * b->x;
  out->y = (1 - alpha) * a->y + alpha * b->y;
}

//vec2_smoothstep
void vec2_smoothstep(Vec2 out, Vec2 a, Vec2 b, float alpha) {
  alpha = 3 * alpha * alpha - 2 * alpha * alpha * alpha;
  out->x = (1 - alpha) * a->x + alpha * b->x;
  out->y = (1 - alpha) * a->y + alpha * b->y;
}

//vec2_add impl
void vec2_add(Vec2 out, Vec2 a, Vec2 b) {
  out->x = a->x + b->x;
  out->y = a->y + b->y;
}

//vec2_subtract impl
void vec2_subtract(Vec2 out, Vec2 a, Vec2 b) {
  out->x = a->x - b->x;
  out->y = a->y - b->y;
}

//vec2_dot impl
float vec2_dot(Vec2 a, Vec2 b) {
  return a->x * b->x + a->y * b->y;
}

//vec2_scale impl
void vec2_scale(Vec2 out, Vec2 v, float s) {
  out->x = v->x * s;
  out->y = v->y * s;
}

//makeMapNode impl
MapNode makeMapNode(float x, float y, char* name, int isHidden) {
  MapNode mn = malloc(sizeof(struct MapNodeStruct));
  mn->location = vec2_make(x, y);
  mn->name = name;
  mn->isHidden = isHidden;
  mn->opt1 = NULL;
  mn->opt2 = NULL;
  mn->opt3 = NULL;
  mn->opt4 = NULL;
  mn->action1 = NULL;
  mn->action2 = NULL;
  mn->action3 = NULL;
  mn->action4 = NULL;
  return mn;
}

//makeMap impl
Map makeMap(char* name, void (*draw)(float)) {
  Map m = malloc(sizeof(struct MapStruct));

  m->name = name;
  m->draw = draw;
  m->location = 0;
  m->numNodes = 0;
  m->numPaths = 0;
  m->nodes = NULL;
  m->paths = NULL;

  return m;
}

//makeMapPath impl
MapPath makeMapPath(int start, int end, int weight, int isHidden, float x1,
                    float y1, float x2, float y2) {
  MapPath mp = malloc(sizeof(struct MapPathStruct));
  mp->start = start;
  mp->end = end;
  mp->weight = weight;
  mp->isHidden = isHidden;
  mp->x1 = x1;
  mp->y1 = y1;
  mp->x2 = x2;
  mp->y2 = y2;
  return mp;
}

//addMapNode impl
int addMapNode(Map m, MapNode mn) {
  int i = m->numNodes;
  //FIXME samurai code
  if (m->nodes == NULL) m->nodes = malloc(sizeof(MapNode));
  else m->nodes = realloc(m->nodes, sizeof(MapNode) * m->numNodes + 1);
  m->nodes[m->numNodes] = mn;
  m->numNodes++;
  return i;
}

//addMapPath impl
int addMapPath(Map m, MapPath mp) {
  int i = m->numPaths;
  //FIXME samurai code
  if (m->paths == NULL) m->paths = malloc(sizeof(MapNode));
  m->paths = realloc(m->paths, sizeof(MapNode) * m->numPaths + 1);
  m->paths[m->numPaths] = mp;
  m->numPaths++;
  return i;
}

//addMap impl
int addMap(Map m) {
  int i = numMaps;
  Map* newm = realloc(maps, sizeof(Map) * (numMaps + 1));
  if (newm) {
    maps = newm;
    maps[numMaps] = m;
    numMaps++;
    return i;
  }
  printf("WARNING: Failed to reallocate space for maps.\n");
  return -1;
}

//tickActor impl
void tickActor(Actor a, float delta) {
  if (a->isStatic) return;

  if (a->isErping) {
    a->alpha += delta / a->duration;
    vec2_smoothstep(a->location, a->startErp, a->endErp, a->alpha);
    if (a->alpha > 1) a->isErping = 0;
  }

  if (a->tick) a->tick(a, delta);
}

//erpActor impl
void erpActor(Actor a, Vec2 dest, float duration) {
  if (a->startErp) free(a->startErp);
  if (a->endErp) free(a->endErp);
  a->startErp = vec2_clone(a->location);
  a->endErp = vec2_clone(dest);
  a->alpha = 0;
  a->duration = duration;
  a->isErping = 1;
}

//resetEverything impl
void resetEverything() {
  //reset music
  al_set_audio_stream_playing(musicCheap, 0);
  al_set_audio_stream_playing(musicAge, 0);
  al_set_audio_stream_playing(musicPear, 0);
  al_set_audio_stream_playing(musicBird, 1);

  //flush event queue
  if (events != NULL && !al_is_event_queue_empty(events)) {
    al_flush_event_queue(events);
  }

  //reset intro timer
  if (introTimer != NULL) al_destroy_timer(introTimer);
  introTimer = al_create_timer(1);
  al_register_event_source(events, al_get_timer_event_source(introTimer));
  al_start_timer(introTimer);

  //destroy all actors
  int i, j;
  //for (i = 0; i < actorsCap; i++) {
  //  if (actors[i] != NULL) destroyActor(actors[i]);
  //} don't bother

  //don't bother with this
  //actorsCap = 0;
  //free(actors);

  //destroy all maps
  for (i = 1; i < numMaps; i++) {
    if (maps[i] == NULL) continue;
    //free(maps[i]->name);

    for (j = 0; j < maps[i]->numNodes; j++) {
      if (maps[i]->nodes[j] != NULL) {
        free(maps[i]->nodes[j]->location);
        //free(maps[i]->nodes[j]->name);
        free(maps[i]->nodes[j]);
      }
    }

    if (maps[i]->numNodes > 0) free(maps[i]->nodes);

    for (j = 0; j < maps[i]->numPaths; j++) {
      if (maps[i]->paths[j] != NULL) free(maps[i]->paths[j]);
    }

    if (maps[i]->numPaths > 0) free(maps[i]->paths);

    free(maps[i]);
  }

  free(maps);

  numMaps = 1;
  Map m = makeMap("Dummy Map", NULL);
  maps = malloc(sizeof(Map));
  maps[0] = m;
  activeMap = 0;

  //reset globals
  gametime = START_TIME; //in seconds, out of 84400
  gamespeed = 1.0;
  fade = 1;
  fading = 0;
  thugging = 0;

  //maps

  //city
  Map city = makeMap(TEXT_PLACE_CITY, drawCityMap);
  MapNode cityApt = makeMapNode(350, 300, TEXT_PLACE_APT, 0);
  cityApt->opt1 = "Go Home";
  cityApt->action1 = cityAptAction;

  MapNode cityStreet1 = makeMapNode(400, 384, TEXT_PLACE_STREET, 0);
  MapNode cityStreet2 = makeMapNode(550, 350, TEXT_PLACE_GATE, 0);
  cityStreet2->opt1 = "Leave City";
  cityStreet2->action1 = cityStreetAction;
  MapNode cityStreet3 = makeMapNode(115, 400, TEXT_PLACE_STREET, 0);
  MapNode cityPlaza1 = makeMapNode(550, 184, TEXT_PLACE_PLAZA1, 0);
  cityPlaza1->opt1 = "Examine";
  cityPlaza1->action1 = cityIvyAction;
  cityPlaza1->opt2 = "Statue";
  cityPlaza1->action2 = cityIvyStatueAction;
  MapNode cityPlaza2 = makeMapNode(275, 225, TEXT_PLACE_PLAZA2, 0);
  cityPlaza2->opt1 = "Examine";
  cityPlaza2->action1 = cityRiverAction;
  cityPlaza2->opt2 = "Chat";
  cityPlaza2->action2 = cityRiverChatAction;
  cityPlaza2->opt3 = "Jump in River";
  cityPlaza2->action3 = cityRiverJumpAction;
  MapNode cityPark = makeMapNode(425, 184, TEXT_PLACE_PARK, 0);
  cityPark->opt1 = "Examine";
  cityPark->action1 = cityParkAction;
  cityPark->opt2 = "Talk to Crowd";
  cityPark->action2 = cityParkTalkAction;
  MapNode citySewers = makeMapNode(225, 125, TEXT_PLACE_SEWERS, 1);
  sewersNode = citySewers;
  citySewers->opt1 = "Examine";
  citySewers->action1 = citySewersAction;
  citySewers->opt2 = "Enter";
  citySewers->action2 = citySewersEnterAction;
  MapNode cityBar = makeMapNode(520, 240, TEXT_PLACE_BAR, 0);
  cityBar->opt1 = "Examine";
  cityBar->action1 = cityBarAction;
  cityBar->opt2 = "Go Inside";
  cityBar->action2 = cityBarEnterAction;
  MapNode cityShop1 = makeMapNode(60, 375, TEXT_PLACE_SHOP1, 0);
  cityShop1->opt1 = "Examine";
  cityShop1->action1 = cityShop1Action;
  cityShop1->opt2 = "Check Door";
  cityShop1->action2 = cityShop1EnterAction;

  int cityAptID = addMapNode(city, cityApt);
  int cityStreet1ID = addMapNode(city, cityStreet1);
  int cityStreet2ID = addMapNode(city, cityStreet2);
  int cityStreet3ID = addMapNode(city, cityStreet3);
  int cityPlaza1ID = addMapNode(city, cityPlaza1);
  int cityPlaza2ID = addMapNode(city, cityPlaza2);
  int cityParkID = addMapNode(city, cityPark);
  int citySewersID = addMapNode(city, citySewers);
  int cityBarID = addMapNode(city, cityBar);
  int cityShop1ID = addMapNode(city, cityShop1);

  MapPath cityPath0 = makeMapPath(cityAptID, cityStreet1ID, 5, 0, 360, 374, 360, 375);
  MapPath cityPath1 = makeMapPath(cityStreet1ID, cityStreet2ID, 10, 0, 400, 360, 550, 380);
  MapPath cityPath2 = makeMapPath(cityStreet1ID, cityStreet3ID, 15, 0, 425, 400, 174, 380);
  MapPath cityPath3 = makeMapPath(cityParkID, cityPlaza1ID, 9, 0, 455, 190, 500, 184);
  MapPath cityPath4 = makeMapPath(cityParkID, cityPlaza2ID, 9, 0, 400, 190, 300, 190);
  MapPath cityPath5 = makeMapPath(cityPlaza1ID, cityStreet2ID, 10, 0, 540, 190, 550, 190);
  MapPath cityPath6 = makeMapPath(cityPlaza2ID, cityStreet1ID, 10, 0, 280, 380, 280, 380);
  MapPath cityPath7 = makeMapPath(cityPlaza2ID, citySewersID, 5, 1, 325, 100, 325, 100);
  MapPath cityPath8 = makeMapPath(cityPlaza1ID, cityBarID, 5, 0, 525, 220, 525, 220);
  MapPath cityPath9 = makeMapPath(cityStreet3ID, cityShop1ID, 5, 0, 50, 425, 50, 425);
  sewersPath = cityPath7;

  addMapPath(city, cityPath0);
  addMapPath(city, cityPath1);
  addMapPath(city, cityPath2);
  addMapPath(city, cityPath3);
  addMapPath(city, cityPath4);
  addMapPath(city, cityPath5);
  addMapPath(city, cityPath6);
  addMapPath(city, cityPath7);
  addMapPath(city, cityPath8);
  addMapPath(city, cityPath9);

  addMap(city);

  //apt
  Map apt = makeMap(TEXT_PLACE_APT, drawAptMap);
  MapNode aptNodeMain = makeMapNode(375, 175, "Appartment", 0);
  MapNode aptNodeKitchen = makeMapNode(250, 384, "Kitchen", 0);
  MapNode aptNodeBathroom = makeMapNode(375, 375, "Bathroom", 0);
  aptNodeMain->opt1 = "Go Out";
  aptNodeMain->action1 = aptMainAction;
  aptNodeKitchen->opt1 = "Eat Food";
  aptNodeKitchen->action1 = aptFoodAction;
  aptNodeKitchen->opt2 = "Booze";
  aptNodeKitchen->action2 = aptBoozeAction;
  aptNodeBathroom->opt1 = "Mirror";
  aptNodeBathroom->action1 = aptMirrorAction;
  aptNodeBathroom->opt2 = "Warm Hands";
  aptNodeBathroom->action2 = aptHandsAction;

  int aptNodeMainID = addMapNode(apt, aptNodeMain);
  int aptNodeKitchenID = addMapNode(apt, aptNodeKitchen);
  int aptNodeBathroomID = addMapNode(apt, aptNodeBathroom);

  MapPath aptPath1 = makeMapPath(aptNodeMainID, aptNodeKitchenID, 0, 0, 325, 325, 325, 325);
  MapPath aptPath2 = makeMapPath(aptNodeMainID, aptNodeBathroomID, 0, 0, 325, 325, 325, 325);

  addMapPath(apt, aptPath1);
  addMapPath(apt, aptPath2);
  
  addMap(apt);

  //bar map
  Map bar = makeMap(TEXT_PLACE_BAR, drawBarMap);
  
  MapNode barMain = makeMapNode(275, 250, "Bar Floor", 0);
  barMain->opt1 = "Leave Bar";
  barMain->action1 = barAction;
  barMain->opt2 = "Booze";
  barMain->action2 = barBoozeAction;

  addMapNode(bar, barMain);

  addMap(bar);

  //shop1 map gets cut :(

  //TODO sewer map
  Map sewers = makeMap(TEXT_PLACE_SEWERS, drawSewersMap);

  MapNode sewersMain = makeMapNode(225, 375, "Smelly Sewer Entrance", 0);
  sewersMain->opt1 = "Exit";
  sewersMain->action1 = sewersMainAction;
  MapNode sewersBoss = makeMapNode(350, 250, "Dramatic Boss Battle", 0);
  bossNode = sewersBoss;
  sewersBoss->opt1 = "Boss";
  sewersBoss->action1 = sewersBossAction;
  sewersBoss->opt2 = "Battle";
  sewersBoss->action2 = sewersBattleAction;
  MapNode sewersDoom = makeMapNode(200, 100, "Doomsday Device", 1);
  doomNode = sewersDoom;
  sewersDoom->opt1 = "Examine";
  sewersDoom->action1 = sewersDoomAction;
  sewersDoom->opt2 = "Use Device";
  sewersDoom->action2 = sewersDOOMSDAY;

  int sewersMainID = addMapNode(sewers, sewersMain);
  int sewersBossID = addMapNode(sewers, sewersBoss);
  int sewersDoomID = addMapNode(sewers, sewersDoom);

  MapPath sewersPath1 = makeMapPath(sewersBossID, sewersDoomID, 9, 1, 350, 100, 350, 100);
  doomPath = sewersPath1;
  MapPath sewersPath2 = makeMapPath(sewersMainID, sewersBossID, 8, 0, 350, 400, 350, 400);

  addMapPath(sewers, sewersPath1);
  addMapPath(sewers, sewersPath2);
  
  addMap(sewers);
  //jumper

  activeMap = 2;

  //player actor
  if (player == NULL)  {
    Vec2 v = vec2_make(320, 240);
    player = makeActor(v, 0, 0, 0, tickPlayer,
    drawPlayer, NULL, NULL, NULL, 2, NULL);
  //Actor makeActor(Vec2 location, float rotation, int isStatic, int isHidden,
  //            void (*tick)(Actor, float),
  //            void (*draw)(Actor, float), void (*created)(Actor),
  //            void (*destroyed)(Actor), void (*clicked)(Actor),
  //            int mapID, Actor owner) {
    free(v);
    //addActor(player);
  } else {
    player->mapID = 2;
    player->location->x = 320;
    player->location->y = 320;
  }
}

//drawCityMap impl
void drawCityMap(float delta) {
  al_draw_bitmap(citymap, 0, 0, 0);
}

//drawAptMap impl
void drawAptMap(float delta) {
  al_draw_bitmap(aptmap, 0, 0, 0);
}

//drawBarMap impl
void drawBarMap(float delta) {
  al_draw_bitmap(barmap, 0, 0, 0);
}

//drawShop1Map impl
void drawShop1Map(float delta) {
  al_draw_bitmap(shop1map, 0, 0, 0);
}

void drawShop2Map(float);

//drawSewersMap impl
void drawSewersMap(float delta) {
  al_draw_bitmap(sewermap, 0, 0, 0);
}

//tickPlayer impl
void tickPlayer(Actor a, float d) {
  if (a->isErping && a->duration > 5 && !uncon) {
    gamespeed = 250;
  } else if (a->isErping && a->duration > 5 && uncon) {
    gamespeed = 1800;
  } else {
    gamespeed = 1;
  }

  if (uncon && !a->isErping) {
    setMessage(TEXT_DRUNK_FAIL3);
    drunkenness = 0;
    uncon = 0;
    fading = 0;
  }
}

//drawPlayer impl
void drawPlayer(Actor a, float d) {
  al_draw_ellipse(a->location->x, a->location->y, 5, 5,
      al_map_rgb(0, 0, 0), 4);
  al_draw_filled_ellipse(a->location->x, a->location->y, 5, 5,
      al_map_rgb(255, 127, 255));
}

//clickHandler impl
void clickHandler(int x, int y) {
  if (player->isErping) return;
  if (y > 450) {
    MapNode n = maps[activeMap]->nodes[maps[activeMap]->location];
    if (x < 160 && n->action1) {
      n->action1();
      al_play_sample(clickbig, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
    } else if (x < 320 && n->action2) {
      n->action2();
      al_play_sample(clickbig, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
    } else if (x < 480 && n->action3) {
      n->action3();
      al_play_sample(clickbig, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
    } else if (n->action4){
      n->action4();
      al_play_sample(clickbig, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
    }
  } else {
    int i, j;
    float d;
    for (i = 0; i < maps[activeMap]->numNodes; i++) {
      Vec2 v = vec2_make(x, y);
      vec2_subtract(v, v, maps[activeMap]->nodes[i]->location);
      d = vec2_dot(v, v);
      if (d < 1024) {
        int a, b, l;
        for (j = 0; j < maps[activeMap]->numPaths; j++) {
          a = maps[activeMap]->paths[j]->start;
          b = maps[activeMap]->paths[j]->end;
          l = maps[activeMap]->location;
          if (((a == l) && (b == i)) || ((a == i) && (b == l))) {
            v->x = maps[activeMap]->nodes[i]->location->x + (rand() % 32 - 16);
            v->y = maps[activeMap]->nodes[i]->location->y + (rand() % 32 - 16);
            erpActor(player, v, 1 + maps[activeMap]->paths[j]->weight * 60);
            al_play_sample(clicksmall, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
            free(v);
            maps[activeMap]->location = i;
            return;
          }
        }
      }
      free(v);
    }
  }
}

//setMessage impl
void setMessage(char* m) { //FIXME this is going to leak!
  message = m;
  messageAlpha = 1;
}

//changeMap impl
void changeMap(int i) {
   player->mapID = i;
   activeMap = i;
   Vec2 v = vec2_make(0, 0);
   int l = maps[activeMap]->location;
   v->x = maps[activeMap]->nodes[l]->location->x + (rand() % 32 - 16);
   v->y = maps[activeMap]->nodes[l]->location->y + (rand() % 32 - 16);
   erpActor(player, v, 1);
   free(v);
}

//cityAptAction impl
void cityAptAction() {
  changeMap(2);
}

//aptMainAction impl
void aptMainAction() {
  changeMap(1);
}

//booze impl
void booze() {
  erpActor(player, player->location, 300);
  drunkenness += 1 + ((rand() % 32) / 32.0);
  if (drunkenness >= 5) {
    setMessage("The world grows dark...");
    float duration = 3600.0 * (2.2 + ((rand() % 64) / 24.0));
    erpActor(player, player->location, duration);
    uncon = 1;
    fading = 1;
  }
}

//aptBoozeAction impl
void aptBoozeAction() {
  setMessage("Nothing like drinking alone on the last day before the end of the world.");
  booze();
}

//aptFoodAction impl
void aptFoodAction() {
  setMessage("I'm not really hungry right now.");
}

//aptMirrorAction impl
void aptMirrorAction() {
  setMessage("There is this really ugly girl in the window, oh wait, it's me.");
  gametime += 30;
}

//aptHandsAction impl
void aptHandsAction() {
  setMessage("Ah, just like EE-sama!");
  gametime += 30;
}

//main impl
int main (int argc, char** argv) {
  //boilerplat-y allegro code
  al_init();
  al_install_keyboard();
  al_install_mouse();
  al_install_audio();
  al_init_font_addon();
  al_init_primitives_addon();
  al_init_ttf_addon();
  al_init_acodec_addon();
  al_init_image_addon();
  al_reserve_samples(SAMPLE_COUNT);

  ALLEGRO_DISPLAY* display;

  display = al_create_display(DISPLAY_HEIGHT, DISPLAY_WIDTH);

  al_set_window_title(display, TEXT_DISPLAY_TITLE);

  //need to set up event queue
  events = al_create_event_queue();
  ALLEGRO_EVENT e;

  al_register_event_source(events, al_get_display_event_source(display));
  al_register_event_source(events, al_get_keyboard_event_source());
  al_register_event_source(events, al_get_mouse_event_source());

  double current, delta, truedelta, last = ((double)clock() / (CLOCKS_PER_SEC));
  int quitting = 0;

  actorsCap = 10;
  actors = calloc(sizeof(Actor), 10);

  numMaps = 1;
  //maps = malloc(sizeof(struct MapStruct));
  maps = malloc(sizeof(Map));
  maps[0] = makeMap("Dummy Map", NULL);
  activeMap = 0;

  //seed rand
  srand(time(0));

  //load all the things
  char* timeString = calloc(sizeof(char), 8);

  //bitmaps
  citymap = al_load_bitmap(IMAGE_CITYMAP);
  if (!citymap) printf("WARNING: Failed to load %s!\n", IMAGE_CITYMAP);
  aptmap = al_load_bitmap("aptmap.jpg");
  shop1map = al_load_bitmap("shop1map.jpg");
  shop2map = al_load_bitmap("shop2map.jpg");
  barmap = al_load_bitmap("barmap.jpg");
  sewermap = al_load_bitmap("sewermap.jpg");

  //font
  ALLEGRO_FONT* bigFont = al_load_font(FONT_NAME, 22, 0);
  ALLEGRO_FONT* littleFont = al_load_font(FONT_NAME, 14, 0);
  if (!bigFont) printf("WARNING: Failed to load %s!\n", FONT_NAME);

  //music
  ALLEGRO_VOICE* voice = al_create_voice(44100, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
  ALLEGRO_MIXER* mixer = al_create_mixer(44100, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2);
  al_set_default_mixer(mixer);
  if (!voice) printf("WARNING: Failed to create voice!\n");
  if (!mixer) printf("WARNING: Failed to create mixer!\n");
  musicBird = al_load_audio_stream(MUSIC_BIRD, 4, 2048);
  if (!musicBird) printf("WARNING: Failed to load %s!\n", MUSIC_BIRD);
  al_set_audio_stream_playing(musicBird, 0);
  musicCheap = al_load_audio_stream(MUSIC_CHEAP, 4, 2048);
  if (!musicCheap) printf("WARNING: Failed to load %s!\n", MUSIC_CHEAP);
  al_set_audio_stream_playing(musicCheap, 0);
  musicPear = al_load_audio_stream(MUSIC_PEAR, 4, 2048);
  if (!musicPear) printf("WARNING: Failed to load %s!\n", MUSIC_PEAR);
  al_set_audio_stream_playing(musicPear, 0);
  musicAge = al_load_audio_stream(MUSIC_AGE, 4, 2048);
  if (!musicAge) printf("WARNING: Failed to load %s!\n", MUSIC_AGE);
  al_set_audio_stream_playing(musicAge, 0);
  al_set_audio_stream_playmode(musicBird, ALLEGRO_PLAYMODE_LOOP);
  al_set_audio_stream_playmode(musicCheap, ALLEGRO_PLAYMODE_LOOP);
  al_set_audio_stream_playmode(musicPear, ALLEGRO_PLAYMODE_LOOP);
  al_set_audio_stream_playmode(musicAge, ALLEGRO_PLAYMODE_LOOP);
  al_attach_audio_stream_to_mixer(musicCheap, mixer);
  al_attach_audio_stream_to_mixer(musicBird, mixer);
  al_attach_audio_stream_to_mixer(musicAge, mixer);
  al_attach_audio_stream_to_mixer(musicPear, mixer);
  al_attach_mixer_to_voice(mixer, voice);
  al_set_mixer_playing(mixer, 1);

  //sfx
  clickbig = al_load_sample(SOUND_BIG);
  if (!clickbig) printf("WARNING: Failed to load %s!\n", SOUND_BIG);
  clicksmall = al_load_sample(SOUND_SMALL);
  if (!clicksmall) printf("WARNING: Failed to load %s!\n", SOUND_SMALL);

  resetEverything();

  //rofl trap code
  //signal(SIGSEGV, segfhandler);

  while(!quitting) {
    //delta
    current = ((double)clock() / (CLOCKS_PER_SEC));
    truedelta = current - last;
    delta = gamespeed * truedelta * 3; //FIXME why 3?
    last = current;

    //handle events first
    //NOTE timers are event sources in allegro 5
    while (al_get_next_event(events, &e)) {
      switch (e.type) {
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
        case ALLEGRO_KEY_ESCAPE:
          quitting = 1;
          break;
        case ALLEGRO_EVENT_TIMER:
          if (e.timer.source == introTimer) {
            //intro timer popped
            al_set_timer_speed(introTimer, 3);
            if (introSequence < 7) al_start_timer(introTimer);
            switch (introSequence) {
              case 0:
                setMessage(TEXT_STORY1);
                break;
              case 1:
                setMessage(TEXT_STORY2);
                break;
              case 2:
                setMessage(TEXT_STORY3);
                break;
              case 3:
                setMessage(TEXT_STORY4);
                break;
              case 4:
                setMessage(TEXT_STORY5);
                break;
              case 5:
                setMessage(TEXT_STORY6);
                break;
              case 6:
                setMessage(TEXT_STORY7);
                break;
              case 7:
                setMessage(TEXT_STORY8);
                break;
            }
            introSequence++;
          }
          break;
        case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
          clickHandler(e.mouse.x, e.mouse.y);
          break;
      }
    }

    //then tick everything
    int i;
    //for (i = 0; i < actorsCap; i++) {
    //  if (actors[i] != NULL) tickActor(actors[i], delta);
    //}
    if (player) tickActor(player, delta);

    //update game time
    gametime = gametime + delta;
    //printf("%f %f %f\n", gametime, delta, truedelta);

    if (messageAlpha > 0) messageAlpha -= truedelta / 2.0;

    if (fading && fade < 1) fade += truedelta;
    else fade -= truedelta;
    if (fade > 1) fade = 1;
    else if (fade < 0) fade = 0;

    if (gametime > 86400) {
      setMessage("The world ends :(");
      fading = 1;
      gamespeed = 1;
    }

    if (gametime > 86430) {
      resetEverything();
    }

    //then draw stuff !!!
    al_clear_to_color(al_map_rgb(48, 48, 48));

    //draw map background
    if (maps[activeMap] && maps[activeMap]->draw) maps[activeMap]->draw(delta);

    //draw map paths
    int loc = maps[activeMap]->location;
    float* points = malloc(sizeof(float) * 8);
    int sid, eid;
    ALLEGRO_COLOR color = al_map_rgba(0, 0, 0, 0);
    for (i = 0; i < maps[activeMap]->numPaths; i++) {
      if (maps[activeMap]->paths[i]->start != loc &&
           maps[activeMap]->paths[i]->end != loc) {
        color = al_map_rgb(127, 127, 127);
      } else {
        color = al_map_rgb(255, 127, 0);
      }
      if (maps[activeMap]->paths[i]->isHidden) continue;
      sid = maps[activeMap]->paths[i]->start;
      eid = maps[activeMap]->paths[i]->end;
      points[0] = maps[activeMap]->nodes[sid]->location->x;
      points[1] = maps[activeMap]->nodes[sid]->location->y;
      points[2] = maps[activeMap]->paths[i]->x1;
      points[3] = maps[activeMap]->paths[i]->y1;
      points[4] = maps[activeMap]->paths[i]->x2;
      points[5] = maps[activeMap]->paths[i]->y2;
      points[6] = maps[activeMap]->nodes[eid]->location->x;
      points[7] = maps[activeMap]->nodes[eid]->location->y;
      al_draw_spline(points, color, 2);
    }
    free(points);

    //draw map nodes
    float cx, cy;
    for (i = 0; i < maps[activeMap]->numNodes; i++) {
      if (maps[activeMap]->nodes[i]->isHidden) continue;
      cx = maps[activeMap]->nodes[i]->location->x;
      cy = maps[activeMap]->nodes[i]->location->y;
      color = (i == maps[activeMap]->location) ? al_map_rgb(255, 255, 0) :
              al_map_rgba(0, 0, 0, 200);
      al_draw_ellipse(cx, cy, 12, 12, color, 2);
      al_draw_filled_ellipse(cx, cy, 8, 8, color);
    }

    //draw actors
    /*
    for (i = 0; i < actorsCap; i++) {
      if (actors[i] != NULL && actors[i]->draw && !actors[i]->isHidden) {
         actors[i]->draw(actors[i], delta);
      }
    }*/
    if (player && player->draw) player->draw(player, delta);

    //then figure out how to do a drunk effect?
    //hint write a shader

    //fading
    if (fade > 0) {
      al_draw_filled_rectangle(0, 0, 640, 480, al_map_rgba_f(0, 0, 0, fade));
    }

    //then ui
    //Tensec Yogo
    al_draw_text(bigFont, al_map_rgba(0, 0, 0, 192), 20, 20, 0, TEXT_NAME_FULL);
    al_draw_text(bigFont, al_map_rgb(255, 0, 0), 16, 16, 0, TEXT_NAME_FULL);
    //Status: 
    al_draw_text(littleFont, al_map_rgba(0, 0, 0, 192), 20, 48, 0, TEXT_STATUS);
    al_draw_text(littleFont, al_map_rgb(255, 255, 255), 16, 44, 0, TEXT_STATUS);
    //drunk :(
    int drankval = (int)floor(drunkenness);
    char* drunkText;
    switch (drankval) {
      case 4:
        drunkText = TEXT_DRUNK4;
        break;
      case 3:
        drunkText = TEXT_DRUNK3;
        break;
      case 2:
        drunkText = TEXT_DRUNK2;
        break;
      case 1:
        drunkText = TEXT_DRUNK1;
        break;
      default:
        drunkText = TEXT_DRUNK0;
        break;
    }
    if (drunkenness >= 5) drunkText = TEXT_DRUNK5;
    al_draw_text(littleFont, al_map_rgba(0, 0, 0, 192), 80, 48, 0, drunkText);
    al_draw_text(littleFont, al_map_rgb(255, 255,   0), 76, 44, 0, drunkText);
    //Time: 
    al_draw_text(littleFont, al_map_rgba(0, 0, 0, 192), 20, 64, 0, TEXT_TIME);
    al_draw_text(littleFont, al_map_rgb(255, 255, 255), 16, 60, 0, TEXT_TIME);
    //09:43
    int hour = (int)(gametime / 3600) % 24;
    int min = ((int)gametime % 3600) / 60;
    int sec = (int)gametime % 60;
    sprintf(timeString, "%02d:%02d:%02d", hour, min, sec);
    al_draw_text(littleFont, al_map_rgba(0, 0, 0, 192), 80, 64, 0, timeString);
    al_draw_text(littleFont, al_map_rgb(255, 255,   0), 76, 60, 0, timeString);
    //map text
    al_draw_text(littleFont, al_map_rgba(0, 0, 0, 192), 620, 48,
        ALLEGRO_ALIGN_RIGHT, maps[activeMap]->name);
    al_draw_text(littleFont, al_map_rgb(255, 255, 255), 616, 44,
        ALLEGRO_ALIGN_RIGHT, maps[activeMap]->name);
    //location text
    al_draw_text(littleFont, al_map_rgba(0, 0, 0, 192), 620, 64,
        ALLEGRO_ALIGN_RIGHT, 
        maps[activeMap]->nodes[maps[activeMap]->location]->name);
    al_draw_text(littleFont, al_map_rgba(255, 255, 255, 192), 616, 60,
        ALLEGRO_ALIGN_RIGHT,
        maps[activeMap]->nodes[maps[activeMap]->location]->name);

    //test message
    if (messageAlpha > 0) {
      al_draw_text(littleFont, al_map_rgba_f(0, 0, 0, messageAlpha), 318, 242,
          ALLEGRO_ALIGN_CENTER, message);
      al_draw_text(littleFont, al_map_rgba_f(0, 0, 0, messageAlpha), 318, 238,
          ALLEGRO_ALIGN_CENTER, message);
      al_draw_text(littleFont, al_map_rgba_f(0, 0, 0, messageAlpha), 322, 242,
          ALLEGRO_ALIGN_CENTER, message);
      al_draw_text(littleFont, al_map_rgba_f(0, 0, 0, messageAlpha), 322, 238,
          ALLEGRO_ALIGN_CENTER, message);
      al_draw_text(littleFont, al_map_rgba_f(1, 1, 1, messageAlpha), 320, 240,
          ALLEGRO_ALIGN_CENTER, message);
    }

    //option text 1
    MapNode n = maps[activeMap]->nodes[maps[activeMap]->location];
    if (n->action1) {
      al_draw_text(bigFont, al_map_rgba(0, 0, 0, 192), 84, 444,
          ALLEGRO_ALIGN_CENTER, n->opt1);
      al_draw_text(bigFont, al_map_rgb(255, 0, 0), 80, 440,
          ALLEGRO_ALIGN_CENTER, n->opt1);
    }
    if (n->action2) {
      al_draw_text(bigFont, al_map_rgba(0, 0, 0, 192), 244, 444,
          ALLEGRO_ALIGN_CENTER, n->opt2);
      al_draw_text(bigFont, al_map_rgb(255, 0, 0), 240, 440,
          ALLEGRO_ALIGN_CENTER, n->opt2);
    }
    if (n->action3) {
      al_draw_text(bigFont, al_map_rgba(0, 0, 0, 192), 404, 444,
          ALLEGRO_ALIGN_CENTER, n->opt3);
      al_draw_text(bigFont, al_map_rgb(255, 0, 0), 400, 440,
          ALLEGRO_ALIGN_CENTER, n->opt3);
    }
    if (n->action4) {
      al_draw_text(bigFont, al_map_rgba(0, 0, 0, 192), 564, 434,
          ALLEGRO_ALIGN_CENTER, n->opt4);
      al_draw_text(bigFont, al_map_rgb(255, 0, 0), 560, 430,
          ALLEGRO_ALIGN_CENTER, n->opt4);
    }

    al_flip_display();
  }

  //clean up loaded resources
  al_destroy_bitmap(citymap);
  al_destroy_audio_stream(musicBird);
  al_destroy_audio_stream(musicCheap);
  al_destroy_audio_stream(musicPear);
  al_destroy_audio_stream(musicAge);
  al_destroy_mixer(mixer);
  al_destroy_voice(voice);

  al_destroy_timer(introTimer);
  al_destroy_event_queue(events);

  al_destroy_display(display);
  al_uninstall_audio();
  al_uninstall_keyboard();
  al_uninstall_mouse();

  return 0;
}

