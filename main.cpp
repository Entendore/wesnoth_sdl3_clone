#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <queue>
#include <string>
#include <fstream>

#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"

const int WINDOW_W = 900;
const int WINDOW_H = 750;
const int HEX_SIZE = 38;
const int GRID_W = 12;
const int GRID_H = 8;

// ==================== SVGs ====================
const char* grassSVG = R"SVG(<svg xmlns="http://www.w3.org/2000/svg" viewBox="5 5 90 90" width="100" height="100"><polygon points="50,5 95,27.5 95,72.5 50,95 5,72.5 5,27.5" fill="#5a9a5a" stroke="#3a7a3a" stroke-width="3"/></svg>)SVG";
const char* forestSVG = R"SVG(<svg xmlns="http://www.w3.org/2000/svg" viewBox="5 5 90 90" width="100" height="100"><polygon points="50,5 95,27.5 95,72.5 50,95 5,72.5 5,27.5" fill="#2d6a2d" stroke="#143d14" stroke-width="3"/><circle cx="35" cy="50" r="12" fill="#1a5a1a"/><circle cx="65" cy="55" r="12" fill="#1a5a1a"/></svg>)SVG";
const char* mountainSVG = R"SVG(<svg xmlns="http://www.w3.org/2000/svg" viewBox="5 5 90 90" width="100" height="100"><polygon points="50,5 95,27.5 95,72.5 50,95 5,72.5 5,27.5" fill="#9a9a9a" stroke="#555" stroke-width="3"/><polygon points="25,70 45,30 60,50 75,70" fill="#555"/></svg>)SVG";
const char* villageSVG = R"SVG(<svg xmlns="http://www.w3.org/2000/svg" viewBox="5 5 90 90" width="100" height="100"><polygon points="50,5 95,27.5 95,72.5 50,95 5,72.5 5,27.5" fill="#4a8a4a" stroke="#2b5d2b" stroke-width="3"/><polygon points="25,55 50,30 75,55 75,75 25,75" fill="#d4a04a" stroke="#8b6914" stroke-width="2"/><rect x="43" y="58" width="14" height="17" fill="#5a3a1a"/></svg>)SVG";
const char* keepSVG = R"SVG(<svg xmlns="http://www.w3.org/2000/svg" viewBox="5 5 90 90" width="100" height="100"><polygon points="50,5 95,27.5 95,72.5 50,95 5,72.5 5,27.5" fill="#7a7a7a" stroke="#444" stroke-width="3"/><rect x="30" y="40" width="40" height="35" fill="#999"/><polygon points="25,40 50,18 75,40" fill="#666"/><rect x="35" y="50" width="8" height="8" fill="#222"/><rect x="57" y="50" width="8" height="8" fill="#222"/></svg>)SVG";
const char* swampSVG = R"SVG(<svg xmlns="http://www.w3.org/2000/svg" viewBox="5 5 90 90" width="100" height="100"><polygon points="50,5 95,27.5 95,72.5 50,95 5,72.5 5,27.5" fill="#4a5a3a" stroke="#2a3a1a" stroke-width="3"/><circle cx="30" cy="50" r="5" fill="#3a4a2a"/><circle cx="70" cy="60" r="5" fill="#3a4a2a"/></svg>)SVG";
const char* sandSVG = R"SVG(<svg xmlns="http://www.w3.org/2000/svg" viewBox="5 5 90 90" width="100" height="100"><polygon points="50,5 95,27.5 95,72.5 50,95 5,72.5 5,27.5" fill="#d2b48c" stroke="#b09870" stroke-width="3"/></svg>)SVG";
const char* hillsSVG = R"SVG(<svg xmlns="http://www.w3.org/2000/svg" viewBox="5 5 90 90" width="100" height="100"><polygon points="50,5 95,27.5 95,72.5 50,95 5,72.5 5,27.5" fill="#7a9a5a" stroke="#5a7a3a" stroke-width="3"/><path d="M20,70 Q35,50 50,70 T80,70" fill="none" stroke="#5a7a3a" stroke-width="3"/></svg>)SVG";
const char* waterSVG = R"SVG(<svg xmlns="http://www.w3.org/2000/svg" viewBox="5 5 90 90" width="100" height="100"><polygon points="50,5 95,27.5 95,72.5 50,95 5,72.5 5,27.5" fill="#2a5a8a" stroke="#1a3a5a" stroke-width="3"/><path d="M20,40 Q35,30 50,40 T80,40" fill="none" stroke="#5a8aba" stroke-width="3"/><path d="M20,60 Q35,50 50,60 T80,60" fill="none" stroke="#5a8aba" stroke-width="3"/></svg>)SVG";
const char* castleSVG = R"SVG(<svg xmlns="http://www.w3.org/2000/svg" viewBox="5 5 90 90" width="100" height="100"><polygon points="50,5 95,27.5 95,72.5 50,95 5,72.5 5,27.5" fill="#8a8a8a" stroke="#555" stroke-width="3"/><rect x="30" y="30" width="40" height="50" fill="#777"/><rect x="35" y="35" width="10" height="10" fill="#666"/><rect x="55" y="35" width="10" height="10" fill="#666"/></svg>)SVG";
const char* bridgeSVG = R"SVG(<svg xmlns="http://www.w3.org/2000/svg" viewBox="5 5 90 90" width="100" height="100"><polygon points="50,5 95,27.5 95,72.5 50,95 5,72.5 5,27.5" fill="#2a5a8a" stroke="#1a3a5a" stroke-width="3"/><rect x="20" y="40" width="60" height="20" fill="#8b4513" stroke="#5a2a0a" stroke-width="2"/><line x1="30" y1="40" x2="30" y2="60" stroke="#5a2a0a" stroke-width="2"/><line x1="50" y1="40" x2="50" y2="60" stroke="#5a2a0a" stroke-width="2"/><line x1="70" y1="40" x2="70" y2="60" stroke="#5a2a0a" stroke-width="2"/></svg>)SVG";
const char* fordSVG = R"SVG(<svg xmlns="http://www.w3.org/2000/svg" viewBox="5 5 90 90" width="100" height="100"><polygon points="50,5 95,27.5 95,72.5 50,95 5,72.5 5,27.5" fill="#4a8aba" stroke="#2a6a9a" stroke-width="3"/><path d="M20,40 Q35,30 50,40 T80,40" fill="none" stroke="#8acaba" stroke-width="2"/><path d="M20,60 Q35,50 50,60 T80,60" fill="none" stroke="#8acaba" stroke-width="2"/></svg>)SVG";
const char* lavaSVG = R"SVG(<svg xmlns="http://www.w3.org/2000/svg" viewBox="5 5 90 90" width="100" height="100"><polygon points="50,5 95,27.5 95,72.5 50,95 5,72.5 5,27.5" fill="#8a2a0a" stroke="#5a1a0a" stroke-width="3"/><polygon points="20,50 35,30 50,50 65,30 80,50" fill="#ff5500"/><polygon points="20,70 40,50 60,70 80,50" fill="#ffaa00"/></svg>)SVG";
const char* oasisSVG = R"SVG(<svg xmlns="http://www.w3.org/2000/svg" viewBox="5 5 90 90" width="100" height="100"><polygon points="50,5 95,27.5 95,72.5 50,95 5,72.5 5,27.5" fill="#d2b48c" stroke="#b09870" stroke-width="3"/><circle cx="50" cy="50" r="15" fill="#4a8aba"/><circle cx="30" cy="40" r="8" fill="#2d6a2d"/><circle cx="70" cy="60" r="8" fill="#2d6a2d"/></svg>)SVG";
const char* snowSVG = R"SVG(<svg xmlns="http://www.w3.org/2000/svg" viewBox="5 5 90 90" width="100" height="100"><polygon points="50,5 95,27.5 95,72.5 50,95 5,72.5 5,27.5" fill="#f0f8ff" stroke="#c0c8d8" stroke-width="3"/><circle cx="30" cy="40" r="4" fill="#e0e0ef"/><circle cx="70" cy="60" r="5" fill="#e0e0ef"/></svg>)SVG";
const char* iceSVG = R"SVG(<svg xmlns="http://www.w3.org/2000/svg" viewBox="5 5 90 90" width="100" height="100"><polygon points="50,5 95,27.5 95,72.5 50,95 5,72.5 5,27.5" fill="#add8e6" stroke="#87ceeb" stroke-width="3"/><polygon points="20,30 40,60 70,40" fill="none" stroke="#fff" stroke-width="2"/></svg>)SVG";
const char* roadSVG = R"SVG(<svg xmlns="http://www.w3.org/2000/svg" viewBox="5 5 90 90" width="100" height="100"><polygon points="50,5 95,27.5 95,72.5 50,95 5,72.5 5,27.5" fill="#a08a60" stroke="#806a40" stroke-width="3"/><path d="M10,50 L50,20 L90,50" fill="none" stroke="#d2b48c" stroke-width="5"/></svg>)SVG";
const char* caveSVG = R"SVG(<svg xmlns="http://www.w3.org/2000/svg" viewBox="5 5 90 90" width="100" height="100"><polygon points="50,5 95,27.5 95,72.5 50,95 5,72.5 5,27.5" fill="#5a5a5a" stroke="#333" stroke-width="3"/><circle cx="50" cy="55" r="20" fill="#111"/></svg>)SVG";
const char* meadowSVG = R"SVG(<svg xmlns="http://www.w3.org/2000/svg" viewBox="5 5 90 90" width="100" height="100"><polygon points="50,5 95,27.5 95,72.5 50,95 5,72.5 5,27.5" fill="#7cba7c" stroke="#5c9a5c" stroke-width="3"/><circle cx="30" cy="40" r="4" fill="#ff69b4"/><circle cx="70" cy="50" r="5" fill="#ffff00"/><circle cx="50" cy="70" r="4" fill="#ff69b4"/></svg>)SVG";
const char* bonesSVG = R"SVG(<svg xmlns="http://www.w3.org/2000/svg" viewBox="5 5 90 90" width="100" height="100"><polygon points="50,5 95,27.5 95,72.5 50,95 5,72.5 5,27.5" fill="#c2a878" stroke="#a08a60" stroke-width="3"/><polygon points="30,60 35,50 40,60 35,70" fill="#e0d8c0"/><polygon points="60,40 65,30 70,40 65,50" fill="#e0d8c0"/></svg>)SVG";
const char* deepWaterSVG = R"SVG(<svg xmlns="http://www.w3.org/2000/svg" viewBox="5 5 90 90" width="100" height="100"><polygon points="50,5 95,27.5 95,72.5 50,95 5,72.5 5,27.5" fill="#1a3a5a" stroke="#0a2a4a" stroke-width="3"/><path d="M20,40 Q35,30 50,40 T80,40" fill="none" stroke="#3a6a9a" stroke-width="3"/><path d="M20,60 Q35,50 50,60 T80,60" fill="none" stroke="#3a6a9a" stroke-width="3"/></svg>)SVG";

const char* scoutSVG = R"SVG(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 100 100" width="100" height="100"><circle cx="50" cy="50" r="30" fill="rgba(80,160,255,0.95)" stroke="#2266aa" stroke-width="3"/><polygon points="50,30 65,55 50,65 35,55" fill="#fff"/></svg>)SVG";
const char* fighterSVG = R"SVG(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 100 100" width="100" height="100"><circle cx="50" cy="50" r="32" fill="rgba(30,80,200,0.95)" stroke="#1133aa" stroke-width="3"/><rect x="40" y="28" width="20" height="44" fill="#ccc" stroke="#888" stroke-width="2"/><polygon points="50,12 56,28 44,28" fill="#ccc"/></svg>)SVG";
const char* archerSVG = R"SVG(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 100 100" width="100" height="100"><circle cx="50" cy="50" r="30" fill="rgba(80,180,80,0.95)" stroke="#226622" stroke-width="3"/><path d="M28,50 Q50,28 72,50" fill="none" stroke="#8b4513" stroke-width="4"/><line x1="28" y1="50" x2="72" y2="50" stroke="#8b4513" stroke-width="2"/></svg>)SVG";
const char* mageSVG = R"SVG(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 100 100" width="100" height="100"><circle cx="50" cy="55" r="28" fill="rgba(150,80,200,0.95)" stroke="#6622aa" stroke-width="3"/><polygon points="50,15 60,45 40,45" fill="#aa66ff"/><circle cx="50" cy="55" r="6" fill="#fff"/></svg>)SVG";
const char* gruntSVG = R"SVG(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 100 100" width="100" height="100"><circle cx="50" cy="50" r="32" fill="rgba(200,50,50,0.95)" stroke="darkred" stroke-width="3"/><rect x="40" y="28" width="20" height="44" fill="#666" stroke="#333" stroke-width="2"/><circle cx="35" cy="45" r="4" fill="yellow"/><circle cx="65" cy="45" r="4" fill="yellow"/></svg>)SVG";
const char* goblinArcherSVG = R"SVG(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 100 100" width="100" height="100"><circle cx="50" cy="50" r="30" fill="rgba(180,80,80,0.95)" stroke="#aa2222" stroke-width="3"/><path d="M28,50 Q50,28 72,50" fill="none" stroke="#5a2a0a" stroke-width="4"/><line x1="28" y1="50" x2="72" y2="50" stroke="#5a2a0a" stroke-width="2"/></svg>)SVG";
const char* wolfRiderSVG = R"SVG(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 100 100" width="100" height="100"><circle cx="50" cy="55" r="28" fill="rgba(150,50,50,0.95)" stroke="darkred" stroke-width="3"/><ellipse cx="50" cy="70" rx="22" ry="10" fill="#555"/><polygon points="28,58 22,42 35,52" fill="#555"/><polygon points="72,58 78,42 65,52" fill="#555"/></svg>)SVG";
const char* trollSVG = R"SVG(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 100 100" width="100" height="100"><circle cx="50" cy="50" r="34" fill="rgba(100,150,50,0.95)" stroke="#446622" stroke-width="3"/><rect x="30" y="40" width="14" height="20" fill="#333"/><rect x="56" y="40" width="14" height="20" fill="#333"/><polygon points="50,65 60,75 40,75" fill="#f00"/></svg>)SVG";

const char* paladinSVG = R"SVG(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 100 100" width="100" height="100"><circle cx="50" cy="50" r="32" fill="rgba(240,240,240,0.95)" stroke="#888" stroke-width="3"/><rect x="40" y="20" width="20" height="60" fill="#ccc"/><rect x="25" y="40" width="50" height="10" fill="#ccc"/><circle cx="50" cy="50" r="5" fill="#00f"/></svg>)SVG";
const char* rangerSVG = R"SVG(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 100 100" width="100" height="100"><circle cx="50" cy="50" r="30" fill="rgba(40,100,40,0.95)" stroke="#113311" stroke-width="3"/><path d="M28,50 Q50,28 72,50" fill="none" stroke="#5a2a0a" stroke-width="4"/><polygon points="50,20 60,40 40,40" fill="#2a5a2a"/></svg>)SVG";
const char* warlordSVG = R"SVG(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 100 100" width="100" height="100"><circle cx="50" cy="50" r="34" fill="rgba(150,20,20,0.95)" stroke="#500" stroke-width="3"/><rect x="35" y="30" width="30" height="40" fill="#333"/><polygon points="20,70 50,40 80,70" fill="#888"/></svg>)SVG";
const char* assassinSVG = R"SVG(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 100 100" width="100" height="100"><circle cx="50" cy="50" r="28" fill="rgba(40,20,20,0.95)" stroke="#111" stroke-width="3"/><polygon points="50,25 65,55 35,55" fill="#111"/><circle cx="50" cy="50" r="4" fill="red"/></svg>)SVG";
const char* archmageSVG = R"SVG(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 100 100" width="100" height="100"><circle cx="50" cy="50" r="30" fill="rgba(80,0,120,0.95)" stroke="#400080" stroke-width="3"/><polygon points="50,5 55,25 45,25" fill="#aa66ff"/><polygon points="50,15 60,35 40,35" fill="#aa66ff"/><circle cx="50" cy="55" r="8" fill="#fff"/></svg>)SVG";
const char* marauderSVG = R"SVG(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 100 100" width="100" height="100"><circle cx="50" cy="50" r="30" fill="rgba(80,20,20,0.95)" stroke="#400000" stroke-width="3"/><ellipse cx="50" cy="70" rx="25" ry="12" fill="#333"/><polygon points="20,60 15,40 30,50" fill="#333"/><polygon points="80,60 85,40 70,50" fill="#333"/></svg>)SVG";
const char* trollWarriorSVG = R"SVG(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 100 100" width="100" height="100"><circle cx="50" cy="50" r="36" fill="rgba(80,120,40,0.95)" stroke="#334400" stroke-width="3"/><rect x="25" y="35" width="15" height="25" fill="#222"/><rect x="60" y="35" width="15" height="25" fill="#222"/><polygon points="50,70 65,85 35,85" fill="#f00"/></svg>)SVG";

// ==================== Enums & Structs ====================
enum TerrainType { GRASS, FOREST, MOUNTAIN, VILLAGE, KEEP, SWAMP, SAND, HILLS, WATER, CASTLE, BRIDGE, FORD, LAVA, OASIS, SNOW, ICE, ROAD, CAVE, MEADOW, BONES, DEEP_WATER };
enum Alignment { LAWFUL, CHAOTIC, NEUTRAL };
enum TimeOfDay { DAWN, MORNING, NOON, AFTERNOON, DUSK, NIGHT };
const char* timeNames[] = {"Dawn", "Morning", "Noon", "Afternoon", "Dusk", "Night"};
SDL_Color timeColors[] = { {255,180,100,255}, {255,230,100,255}, {255,255,100,255}, {255,200,100,255}, {200,100,50,255}, {80,80,180,255} };
enum Trait { NONE = 0, QUICK = 1, RESILIENT = 2, STRONG = 4, INTELLIGENT = 8 };

struct UnitType {
    const char* name; int maxHp, attacks, damage, moveRange, attackRange, sight; Alignment align; int cost; const char* svg;
    bool hasPoison, hasRegen, hasHealAura, firstStrike, ignoresZoc, hasSlow, leadership, canHeal, canTeleport, stealth, magical, hasDrain, hasCharge, hasBackstab, flying, healthy;
};

struct Unit {
    int x, y, hp, maxHp, xp, level, typeIdx, owner; int hpBonus, dmgBonus, moveRange, traits;
    bool hasMoved, isLeader, isPoisoned, isSlowed; float animProgress; int prevX, prevY; bool didAttack; float lungeProgress; int lungeTargetX, lungeTargetY;
};

struct Village { int x, y, owner; };
struct FloatText { float x, y; std::string text; SDL_Color color; float life, vy; };
struct CombatAnim { float fromX, fromY, toX, toY; float life; SDL_Color color; };
struct Particle { float x, y, vx, vy; SDL_Color color; float life; float size; };
struct Projectile { float x, y, tx, ty, speed; SDL_Color color; bool active; };
enum AIState { AI_IDLE, AI_SELECTING, AI_MOVING, AI_ATTACKING, AI_NEXT_UNIT };

struct Game {
    SDL_AudioStream* audioStream = nullptr; std::string savePath = "savegame.txt";
    std::vector<Unit> units; std::vector<std::vector<TerrainType>> map; std::vector<Village> villages;
    std::vector<FloatText> floatTexts; std::vector<CombatAnim> combatAnims; std::vector<Particle> particles; std::vector<Projectile> projectiles;
    int currentPlayer = 0, selectedUnitIndex = -1, animatingUnitIndex = -1; int hoverX = -1, hoverY = -1; int gold[2] = {60, 60};
    int turnCount = 1; TimeOfDay timeOfDay = DAWN; bool reachable[GRID_W][GRID_H] = {false}; bool attackable[GRID_W][GRID_H] = {false};
    int costMap[GRID_W][GRID_H] = {0}; int cameFromX[GRID_W][GRID_H], cameFromY[GRID_W][GRID_H]; std::vector<SDL_Point> currentPath;
    bool showRecruitMenu = false; bool gameOver = false; int winner = -1; bool visible[GRID_W][GRID_H] = {false}; bool explored[GRID_W][GRID_H] = {false};
    bool showDefenseOverlay = false; float screenShake = 0.0f; float globalTime = 0.0f;
    AIState aiState = AI_IDLE; int aiCurrentUnitIdx = -1, aiTargetUnitIdx = -1;
};

std::vector<UnitType> unitTypes;
std::vector<SDL_Texture*> unitTextures(15, nullptr); 
SDL_Texture* terrainTex[21] = {nullptr}; 

struct AppState {
    SDL_Window* window = nullptr; SDL_Renderer* renderer = nullptr; Game* game = nullptr; Uint64 lastTime = 0;
    SDL_Cursor* cursorDefault = nullptr; SDL_Cursor* cursorAttack = nullptr; SDL_Cursor* cursorMove = nullptr; SDL_Cursor* cursorSelect = nullptr;
};

// ==================== Utility & Audio ====================
SDL_Texture* LoadSVG(SDL_Renderer* r, const char* svg, int w, int h) {
    NSVGimage* image = nsvgParse((char*)svg, "px", 96.0f); if (!image) return nullptr;
    NSVGrasterizer* rast = nsvgCreateRasterizer(); unsigned char* img = (unsigned char*)malloc(w * h * 4);
    nsvgRasterize(rast, image, 0, 0, (float)w / image->width, img, w, h, w * 4);
    SDL_Surface* s = SDL_CreateSurface(w, h, SDL_PIXELFORMAT_RGBA32); SDL_memcpy(s->pixels, img, w * h * 4);
    SDL_Texture* t = SDL_CreateTextureFromSurface(r, s); if (t) SDL_SetTextureScaleMode(t, SDL_SCALEMODE_LINEAR);
    SDL_DestroySurface(s); free(img); nsvgDeleteRasterizer(rast); nsvgDelete(image); return t;
}

void playSound(SDL_AudioStream* stream, float freq, float duration, float volume) {
    if (!stream) return; int num_samples = 44100 * duration; std::vector<float> samples(num_samples);
    for (int i = 0; i < num_samples; ++i) { float t = i / 44100.0f; float env = std::exp(-t * 5.0f); samples[i] = std::sin(2 * M_PI * freq * t) * env * volume; }
    SDL_PutAudioStreamData(stream, samples.data(), num_samples * sizeof(float));
}

int generateTraits() { std::vector<int> pool = {QUICK, RESILIENT, STRONG, INTELLIGENT}; for (int i = pool.size() - 1; i > 0; --i) std::swap(pool[i], pool[rand() % (i + 1)]); return pool[0] | pool[1]; }
SDL_FPoint hexToPixel(int c, int r) { return {HEX_SIZE * std::sqrt(3.0f) * (c + 0.5f * (r & 1)) + 80, HEX_SIZE * 1.5f * r + 80}; }
SDL_Point pixelToHex(float px, float py) { SDL_Point best = {0,0}; float minD = 1e9; for (int r = 0; r < GRID_H; ++r) for (int c = 0; c < GRID_W; ++c) { SDL_FPoint p = hexToPixel(c, r); float d = (px-p.x)*(px-p.x) + (py-p.y)*(py-p.y); if (d < minD) { minD = d; best = {c,r}; } } return best; }
int hexDistance(int x1, int y1, int x2, int y2) { int q1 = x1 - (y1 - (y1 & 1)) / 2, r1 = y1; int q2 = x2 - (y2 - (y2 & 1)) / 2, r2 = y2; return (abs(q1-q2) + abs(q1+r1-q2-r2) + abs(r1-r2)) / 2; }
bool isAdjacent(int x1, int y1, int x2, int y2) { return hexDistance(x1,y1,x2,y2) == 1; }
int findUnitAt(Game& g, int x, int y) { for (size_t i = 0; i < g.units.size(); ++i) if (g.units[i].x == x && g.units[i].y == y) return i; return -1; }

int getDefense(TerrainType t) { if (t == FOREST) return 40; if (t == MOUNTAIN) return 60; if (t == VILLAGE) return 50; if (t == KEEP) return 40; if (t == SWAMP) return 20; if (t == SAND) return 10; if (t == HILLS) return 30; if (t == WATER) return 10; if (t == CASTLE) return 40; if (t == BRIDGE) return 20; if (t == FORD) return 10; if (t == LAVA) return 0; if (t == OASIS) return 30; if (t == SNOW) return 20; if (t == ICE) return 10; if (t == ROAD) return 0; if (t == CAVE) return 60; if (t == MEADOW) return 10; if (t == BONES) return 10; if (t == DEEP_WATER) return 0; return 20; }
int getMoveCost(TerrainType t) { if (t == FOREST) return 2; if (t == MOUNTAIN) return 999; if (t == SWAMP) return 3; if (t == SAND) return 2; if (t == HILLS) return 2; if (t == WATER) return 999; if (t == CASTLE) return 1; if (t == BRIDGE) return 1; if (t == FORD) return 3; if (t == LAVA) return 2; if (t == OASIS) return 2; if (t == SNOW) return 2; if (t == ICE) return 999; if (t == ROAD) return 1; if (t == CAVE) return 2; if (t == MEADOW) return 1; if (t == BONES) return 2; if (t == DEEP_WATER) return 999; return 1; }
int getTerrainHeight(TerrainType t) { if (t == HILLS || t == SWAMP || t == SAND || t == SNOW || t == BONES) return 1; if (t == FOREST || t == MOUNTAIN || t == KEEP || t == CASTLE || t == CAVE) return 2; return 0; }
int getUnitDefense(Game& g, Unit& u) { if (unitTypes[u.typeIdx].flying) { TerrainType t = g.map[u.x][u.y]; if (t == VILLAGE) return 50; if (t == KEEP || t == CASTLE) return 40; return 0; } return getDefense(g.map[u.x][u.y]); }
int getDamageModifier(TimeOfDay t, Alignment a) { if (a == NEUTRAL) return 0; bool isDay = (t == MORNING || t == NOON || t == AFTERNOON); bool isNight = (t == NIGHT); if (isDay) return (a == LAWFUL) ? 25 : -25; if (isNight) return (a == CHAOTIC) ? 25 : -25; return 0; }
int getUnitBaseDamage(Unit& u) { return unitTypes[u.typeIdx].damage + u.dmgBonus + ((u.traits & STRONG) ? 1 : 0); }
int getUnitMaxHp(Unit& u) { return unitTypes[u.typeIdx].maxHp + u.hpBonus + ((u.traits & RESILIENT) ? 5 : 0); }
int getUnitMoveRange(Unit& u) { int base = unitTypes[u.typeIdx].moveRange + ((u.traits & QUICK) ? 1 : 0); if (u.isSlowed) base = std::max(1, base / 2); return base; }
int getUnitAttacks(Unit& u) { int base = unitTypes[u.typeIdx].attacks; if (u.isSlowed) base = std::max(1, base / 2); return base; }
int getXpNeeded(Unit& u) { int needed = 20 * u.level; if (u.traits & INTELLIGENT) needed -= needed * 0.25; return std::max(1, needed); }

int getUnitDamageWithAuras(Game& g, Unit& u, int targetX, int targetY) {
    int dmg = getUnitBaseDamage(u); int mod = getDamageModifier(g.timeOfDay, unitTypes[u.typeIdx].align); dmg = dmg + (dmg * mod / 100);
    int atkHeight = getTerrainHeight(g.map[u.x][u.y]); int defHeight = getTerrainHeight(g.map[targetX][targetY]);
    if (atkHeight > defHeight) dmg = dmg * 125 / 100; else if (atkHeight < defHeight) dmg = dmg * 75 / 100;
    if (u.level < 3) { for (int dy = -1; dy <= 1; ++dy) { for (int dx = -1; dx <= 1; ++dx) { if (dx == 0 && dy == 0) continue; int nx = u.x + dx, ny = u.y + dy; if (!isAdjacent(u.x, u.y, nx, ny)) continue; int allyIdx = findUnitAt(g, nx, ny); if (allyIdx != -1 && g.units[allyIdx].owner == u.owner && unitTypes[g.units[allyIdx].typeIdx].leadership) { dmg += dmg * 0.25; return dmg; } } } }
    return dmg;
}

void calculateVisibility(Game& g, int player) {
    memset(g.visible, 0, sizeof(g.visible));
    for (auto& u : g.units) { if (u.owner == player) { int sight = unitTypes[u.typeIdx].sight; for (int r = 0; r < GRID_H; ++r) { for (int c = 0; c < GRID_W; ++c) { int dist = hexDistance(u.x, u.y, c, r); if (dist <= sight) { g.explored[c][r] = true; int enemyIdx = findUnitAt(g, c, r); if (enemyIdx != -1 && g.units[enemyIdx].owner != player) { if (unitTypes[g.units[enemyIdx].typeIdx].stealth && dist > 1) continue; } if (g.map[c][r] == FOREST && dist > 1) { int eIdx = findUnitAt(g, c, r); if (eIdx == -1 || g.units[eIdx].owner != player) continue; } g.visible[c][r] = true; } } } } }
    for (auto& v : g.villages) { if (v.owner == player) { for (int r = 0; r < GRID_H; ++r) { for (int c = 0; c < GRID_W; ++c) { int dist = hexDistance(v.x, v.y, c, r); if (dist <= 2) { g.explored[c][r] = true; int enemyIdx = findUnitAt(g, c, r); if (enemyIdx != -1 && g.units[enemyIdx].owner != player) { if (unitTypes[g.units[enemyIdx].typeIdx].stealth && dist > 1) continue; } if (g.map[c][r] == FOREST && dist > 1) continue; g.visible[c][r] = true; } } } } }
}

// ==================== Save / Load ====================
void saveGame(Game& g) { std::ofstream file(g.savePath); if (!file) return; file << g.turnCount << " " << g.timeOfDay << " " << g.gold[0] << " " << g.gold[1] << "\n"; file << GRID_W << " " << GRID_H << "\n"; for(int r=0; r<GRID_H; ++r) { for(int c=0; c<GRID_W; ++c) file << g.map[c][r] << " "; file << "\n"; } file << g.units.size() << "\n"; for (auto& u : g.units) { file << u.x << " " << u.y << " " << u.hp << " " << u.maxHp << " " << u.xp << " " << u.level << " " << u.typeIdx << " " << u.owner << " " << u.hasMoved << " " << u.isPoisoned << " " << u.isSlowed << " " << u.isLeader << " " << u.hpBonus << " " << u.dmgBonus << " " << u.traits << "\n"; } file << g.villages.size() << "\n"; for (auto& v : g.villages) file << v.x << " " << v.y << " " << v.owner << "\n"; }
void loadGame(Game& g) { std::ifstream file(g.savePath); if (!file) return; g.units.clear(); g.villages.clear(); file >> g.turnCount >> g.timeOfDay >> g.gold[0] >> g.gold[1]; int mw, mh; file >> mw >> mh; for(int r=0; r<mh; ++r) { for(int c=0; c<mw; ++c) { int t; file >> t; g.map[c][r] = (TerrainType)t; } } int numUnits; file >> numUnits; for (int i = 0; i < numUnits; ++i) { Unit u; file >> u.x >> u.y >> u.hp >> u.maxHp >> u.xp >> u.level >> u.typeIdx >> u.owner >> u.hasMoved >> u.isPoisoned >> u.isSlowed >> u.isLeader >> u.hpBonus >> u.dmgBonus >> u.traits; u.animProgress = 0; u.prevX = u.x; u.prevY = u.y; u.didAttack = false; u.lungeProgress = 0; u.moveRange = getUnitMoveRange(u); g.units.push_back(u); } int numVillages; file >> numVillages; for (int i = 0; i < numVillages; ++i) { Village v; file >> v.x >> v.y >> v.owner; g.villages.push_back(v); } calculateVisibility(g, 0); }

// ==================== Particles & Projectiles ====================
void spawnDustParticles(Game& g, float x, float y, int count) { for (int i = 0; i < count; ++i) { float angle = (rand() % 360) * M_PI / 180.0f; float speed = 20 + rand() % 40; g.particles.push_back({x, y, cos(angle)*speed, sin(angle)*speed - 20, {180,160,120,255}, 0.5f, 3.0f}); } }
void spawnHitParticles(Game& g, float x, float y, SDL_Color color, int count) { for (int i = 0; i < count; ++i) { float angle = (rand() % 360) * M_PI / 180.0f; float speed = 40 + rand() % 80; g.particles.push_back({x, y, cos(angle)*speed, sin(angle)*speed, color, 0.6f, 4.0f}); } }
void updateParticles(Game& g, float dt) { for (auto& p : g.particles) { p.x += p.vx * dt; p.y += p.vy * dt; p.vy += 60 * dt; p.vx *= 0.95f; p.life -= dt; } g.particles.erase(std::remove_if(g.particles.begin(), g.particles.end(), [](const Particle& p){ return p.life <= 0; }), g.particles.end()); for (auto& p : g.projectiles) { if (!p.active) continue; float dx = p.tx - p.x, dy = p.ty - p.y; float dist = std::sqrt(dx*dx + dy*dy); float move_dist = p.speed * dt; if (dist < move_dist) { p.active = false; continue; } p.x += dx / dist * move_dist; p.y += dy / dist * move_dist; } g.projectiles.erase(std::remove_if(g.projectiles.begin(), g.projectiles.end(), [](const Projectile& p){ return !p.active; }), g.projectiles.end()); }

// ==================== Pathfinding ====================
void calculateReachable(Game& g, Unit& u) {
    memset(g.reachable, 0, sizeof(g.reachable)); memset(g.attackable, 0, sizeof(g.attackable));
    for (int i = 0; i < GRID_W; ++i) for (int j = 0; j < GRID_H; ++j) { g.costMap[i][j] = 999; g.cameFromX[i][j] = -1; g.cameFromY[i][j] = -1; }
    typedef std::pair<int, std::pair<int,int>> PQEntry; std::priority_queue<PQEntry, std::vector<PQEntry>, std::greater<PQEntry>> pq;
    pq.push({0, {u.x, u.y}}); g.costMap[u.x][u.y] = 0; g.reachable[u.x][u.y] = true;
    while (!pq.empty()) { auto [cost, pos] = pq.top(); pq.pop(); int cx = pos.first, cy = pos.second; if (cost > g.costMap[cx][cy]) continue;
        for (int dy = -1; dy <= 1; ++dy) { for (int dx = -1; dx <= 1; ++dx) { if (dx == 0 && dy == 0) continue; int nx = cx + dx, ny = cy + dy; if (!isAdjacent(cx, cy, nx, ny)) continue; if (nx < 0 || nx >= GRID_W || ny < 0 || ny >= GRID_H) continue; if (!g.visible[nx][ny] && u.owner == 0) continue;
            int mc = getMoveCost(g.map[nx][ny]); if (unitTypes[u.typeIdx].flying) mc = 1; if (mc >= 999) continue;
            int occupant = findUnitAt(g, nx, ny); if (occupant != -1 && g.units[occupant].owner != u.owner) continue;
            int newCost = cost + mc; int moveRange = getUnitMoveRange(u);
            if (!unitTypes[u.typeIdx].ignoresZoc) { bool adjEnemy = false; for (int dy2 = -1; dy2 <= 1; ++dy2) { for (int dx2 = -1; dx2 <= 1; ++dx2) { if (dx2 == 0 && dy2 == 0) continue; int ex = nx + dx2, ey = ny + dy2; if (!isAdjacent(nx, ny, ex, ey)) continue; int eIdx = findUnitAt(g, ex, ey); if (eIdx != -1 && g.units[eIdx].owner != u.owner) { adjEnemy = true; break; } } if (adjEnemy) break; } if (adjEnemy) newCost = moveRange; }
            if (newCost <= moveRange && newCost < g.costMap[nx][ny]) { g.costMap[nx][ny] = newCost; g.cameFromX[nx][ny] = cx; g.cameFromY[nx][ny] = cy; g.reachable[nx][ny] = true; pq.push({newCost, {nx, ny}}); }
        } }
    }
    int atkRange = unitTypes[u.typeIdx].attackRange;
    for (int r = 0; r < GRID_H; ++r) { for (int c = 0; c < GRID_W; ++c) { if (!g.reachable[c][r]) continue; for (int r2 = 0; r2 < GRID_H; ++r2) { for (int c2 = 0; c2 < GRID_W; ++c2) { int d = hexDistance(c, r, c2, r2); if (d >= 1 && d <= atkRange) { if (!g.visible[c2][r2] && u.owner == 0) continue; int idx = findUnitAt(g, c2, r2); if (idx != -1 && g.units[idx].owner != u.owner) g.attackable[c2][r2] = true; } } } } }
}

std::vector<SDL_Point> reconstructPath(Game& g, int tx, int ty, int sx, int sy) { std::vector<SDL_Point> path; int cx = tx, cy = ty; while (cx != sx || cy != sy) { path.push_back({cx, cy}); int nx = g.cameFromX[cx][cy], ny = g.cameFromY[cx][cy]; if (nx == -1) break; cx = nx; cy = ny; } std::reverse(path.begin(), path.end()); return path; }

// ==================== Combat & Mechanics ====================
void addFloatText(Game& g, float x, float y, const char* text, SDL_Color color) { g.floatTexts.push_back({x, y, text, color, 1.0f, -30.0f}); }

bool attackSequence(Game& g, Unit& A, Unit& D, SDL_FPoint aPos, SDL_FPoint dPos, bool isCounter, bool origAttackerHasCharge) {
    UnitType& atkType = unitTypes[A.typeIdx]; int dmg = getUnitDamageWithAuras(g, A, D.x, D.y); int defChance = getUnitDefense(g, D); int hitChance = atkType.magical ? 70 : (100 - defChance); int attacks = getUnitAttacks(A);
    bool isBackstab = false;
    if (atkType.hasBackstab && !isCounter) { SDL_FPoint aPxl = hexToPixel(A.x, A.y); SDL_FPoint dPxl = hexToPixel(D.x, D.y); SDL_FPoint oppPxl = { dPxl.x + (dPxl.x - aPxl.x), dPxl.y + (dPxl.y - aPxl.y) }; SDL_Point oppHex = pixelToHex(oppPxl.x, oppPxl.y); int allyIdx = findUnitAt(g, oppHex.x, oppHex.y); if (allyIdx != -1 && g.units[allyIdx].owner == A.owner) { isBackstab = true; dmg = dmg * 3 / 2; addFloatText(g, dPos.x, dPos.y - 45, "BACKSTAB!", {255, 50, 255, 255}); } }
    if (origAttackerHasCharge) dmg = dmg * 3 / 2; 
    g.combatAnims.push_back({aPos.x, aPos.y, dPos.x, dPos.y, 0.4f, {255,255,100,255}});
    if (atkType.attackRange > 1) { g.projectiles.push_back({aPos.x, aPos.y, dPos.x, dPos.y, 600.0f, {255, 200, 50, 255}, true}); } else { A.lungeProgress = 0.01f; A.lungeTargetX = D.x; A.lungeTargetY = D.y; }
    playSound(g.audioStream, atkType.magical ? 500 : 150, 0.1, 0.4);
    for (int i = 0; i < attacks; ++i) {
        if (rand() % 100 < hitChance) { D.hp -= dmg; addFloatText(g, dPos.x, dPos.y, std::to_string(dmg).c_str(), {255,80,80,255}); spawnHitParticles(g, dPos.x, dPos.y, {255,50,50,255}, 5); g.screenShake = 3.0f; playSound(g.audioStream, atkType.magical ? 300 : 100, 0.15, 0.5); if (atkType.hasDrain && dmg > 0) { A.hp = std::min(getUnitMaxHp(A), A.hp + dmg / 2); addFloatText(g, aPos.x, aPos.y - 15, ("+" + std::to_string(dmg/2) + " Drain").c_str(), {200, 0, 200, 255}); } if (D.hp <= 0) { A.xp += 8; return true; } }
        else { addFloatText(g, dPos.x, dPos.y, "MISS", {150,150,150,255}); }
    }
    A.xp += 1; return false;
}

void performAttack(Game& g, Unit& attacker, Unit& defender) {
    SDL_FPoint aPos = hexToPixel(attacker.x, attacker.y); SDL_FPoint dPos = hexToPixel(defender.x, defender.y);
    bool atkFirst = unitTypes[attacker.typeIdx].firstStrike; bool defFirst = unitTypes[defender.typeIdx].firstStrike; bool atkCharge = unitTypes[attacker.typeIdx].hasCharge;
    if (atkFirst && !defFirst) { if (attackSequence(g, attacker, defender, aPos, dPos, false, atkCharge)) return; }
    if (defFirst && !atkFirst) { if (attackSequence(g, defender, attacker, dPos, aPos, false, unitTypes[defender.typeIdx].hasCharge)) return; }
    if (attackSequence(g, attacker, defender, aPos, dPos, false, atkCharge)) return;
    int dist = hexDistance(attacker.x, attacker.y, defender.x, defender.y);
    if (dist == 1 && unitTypes[defender.typeIdx].attackRange >= 1 && defender.hp > 0) { if (attackSequence(g, defender, attacker, dPos, aPos, true, atkCharge)) return; }
    if (unitTypes[attacker.typeIdx].hasPoison && defender.hp > 0) { defender.isPoisoned = true; addFloatText(g, dPos.x, dPos.y - 15, "POISON!", {100,255,100,255}); spawnHitParticles(g, dPos.x, dPos.y, {50,200,50,255}, 4); }
    if (unitTypes[attacker.typeIdx].hasSlow && defender.hp > 0) { defender.isSlowed = true; addFloatText(g, dPos.x, dPos.y - 30, "SLOWED!", {50,50,255,255}); spawnHitParticles(g, dPos.x, dPos.y, {50,50,255,255}, 4); }
}

void checkLevelUp(Game& g, Unit& u) { int xpNeeded = getXpNeeded(u); if (u.xp >= xpNeeded && u.level < 3) { u.xp -= xpNeeded; u.level++; u.hpBonus += 5; u.dmgBonus += 1; if (u.level == 3) { if (u.typeIdx == 1) u.typeIdx = 8; else if (u.typeIdx == 2) u.typeIdx = 9; else if (u.typeIdx == 3) u.typeIdx = 12; else if (u.typeIdx == 4) u.typeIdx = 10; else if (u.typeIdx == 5) u.typeIdx = 11; else if (u.typeIdx == 6) u.typeIdx = 13; else if (u.typeIdx == 7) u.typeIdx = 14; } u.maxHp = getUnitMaxHp(u); u.hp = u.maxHp; u.isPoisoned = false; u.isSlowed = false; SDL_FPoint p = hexToPixel(u.x, u.y); addFloatText(g, p.x, p.y, "LEVEL UP!", {255,255,0,255}); spawnHitParticles(g, p.x, p.y, {255,255,0,255}, 8); playSound(g.audioStream, 800, 0.3, 0.5); } }
void removeDeadUnits(Game& g) { for (auto& u : g.units) { if (u.hp <= 0 && u.isLeader) { g.gameOver = true; g.winner = 1 - u.owner; } } g.units.erase(std::remove_if(g.units.begin(), g.units.end(), [](const Unit& u){ return u.hp <= 0; }), g.units.end()); }

// ==================== Enemy AI ====================
void updateAI(Game& g, float dt) {
    if (g.currentPlayer != 1 || g.gameOver || g.animatingUnitIndex != -1) return;
    if (g.aiState == AI_IDLE) { int idx = -1; for (size_t i = 0; i < g.units.size(); ++i) { if (g.units[i].owner == 1 && !g.units[i].hasMoved) { idx = i; break; } } if (idx == -1) { endTurn(g); return; } g.aiCurrentUnitIdx = idx; g.aiState = AI_SELECTING; }
    if (g.aiState == AI_SELECTING) {
        Unit& enemy = g.units[g.aiCurrentUnitIdx]; calculateReachable(g, enemy);
        if (enemy.hp <= enemy.maxHp * 0.3) { int bestVX = -1, bestVY = -1; for (auto& v : g.villages) { if (v.owner == 1 && g.reachable[v.x][v.y] && findUnitAt(g, v.x, v.y) == -1) { bestVX = v.x; bestVY = v.y; break; } } if(bestVX == -1) { for (int r=0; r<GRID_H; ++r) { for (int c=0; c<GRID_W; ++c) { if (g.map[c][r] == OASIS && g.reachable[c][r] && findUnitAt(g, c, r) == -1) { bestVX = c; bestVY = r; break; } } if(bestVX != -1) break; } } if (bestVX != -1) { g.currentPath = reconstructPath(g, bestVX, bestVY, enemy.x, enemy.y); g.animatingUnitIndex = g.aiCurrentUnitIdx; enemy.hasMoved = true; g.aiState = AI_MOVING; return; } }
        int bestVX = -1, bestVY = -1, bestVDist = 999; for (auto& v : g.villages) { if (v.owner != 1 && g.reachable[v.x][v.y] && findUnitAt(g, v.x, v.y) == -1) { int d = hexDistance(enemy.x, enemy.y, v.x, v.y); if (d < bestVDist) { bestVDist = d; bestVX = v.x; bestVY = v.y; } } }
        int targetIdx = -1; float bestTargetScore = -1e9; for (size_t i = 0; i < g.units.size(); ++i) { if (g.units[i].owner != 0) continue; float dist = hexDistance(enemy.x, enemy.y, g.units[i].x, g.units[i].y); float score = -dist + (g.units[i].isLeader ? 20 : 0) + (g.units[i].hp < enemy.hp ? 5 : 0); if (score > bestTargetScore) { bestTargetScore = score; targetIdx = i; } }
        int moveX = -1, moveY = -1, attackIdx = -1;
        if (targetIdx != -1) { Unit& target = g.units[targetIdx]; int atkRange = unitTypes[enemy.typeIdx].attackRange; int bestX = enemy.x, bestY = enemy.y, bestScore = -999; bool canAttack = false; for (int r = 0; r < GRID_H; ++r) { for (int c = 0; c < GRID_W; ++c) { if (!g.reachable[c][r]) continue; int d = hexDistance(c, r, target.x, target.y); if (d >= 1 && d <= atkRange) { int def = getUnitDefense(g, enemy); int heightAdv = getTerrainHeight(g.map[c][r]) - getTerrainHeight(g.map[target.x][target.y]); int score = def + (heightAdv * 15); if (score > bestScore) { bestScore = score; bestX = c; bestY = r; canAttack = true; } } } } if (canAttack) { moveX = bestX; moveY = bestY; attackIdx = targetIdx; } }
        if (moveX == -1 && bestVX != -1) { moveX = bestVX; moveY = bestVY; }
        if (moveX == -1 && targetIdx != -1) { float bestMoveDist = 1e9; for (int r = 0; r < GRID_H; ++r) { for (int c = 0; c < GRID_W; ++c) { if (!g.reachable[c][r]) continue; float d = hexDistance(c, r, g.units[targetIdx].x, g.units[targetIdx].y); if (d < bestMoveDist) { bestMoveDist = d; moveX = c; moveY = r; } } } }
        if (moveX != -1) { if (moveX != enemy.x || moveY != enemy.y) { g.currentPath = reconstructPath(g, moveX, moveY, enemy.x, enemy.y); g.animatingUnitIndex = g.aiCurrentUnitIdx; enemy.hasMoved = true; g.aiTargetUnitIdx = attackIdx; g.aiState = AI_MOVING; } else { if (attackIdx != -1) performAttack(g, enemy, g.units[attackIdx]); removeDeadUnits(g); g.aiState = AI_NEXT_UNIT; } } else { enemy.hasMoved = true; g.aiState = AI_NEXT_UNIT; }
    }
    if (g.aiState == AI_MOVING) { g.aiState = AI_ATTACKING; }
    if (g.aiState == AI_ATTACKING) { Unit& enemy = g.units[g.aiCurrentUnitIdx]; if (g.aiTargetUnitIdx != -1 && g.aiTargetUnitIdx < g.units.size()) { Unit& target = g.units[g.aiTargetUnitIdx]; if (target.hp > 0) { performAttack(g, enemy, target); removeDeadUnits(g); } } g.aiState = AI_NEXT_UNIT; }
    if (g.aiState == AI_NEXT_UNIT) { for (auto& u : g.units) checkLevelUp(g, u); int p0 = 0, p1 = 0; for (auto& u : g.units) { if (u.owner == 0 && u.isLeader) p0++; if (u.owner == 1 && u.isLeader) p1++; } if (p0 == 0) { g.gameOver = true; g.winner = 1; return; } if (p1 == 0) { g.gameOver = true; g.winner = 0; return; } g.aiState = AI_IDLE; g.aiCurrentUnitIdx = -1; g.aiTargetUnitIdx = -1; }
}

// ==================== Turn Management ====================
void startTurn(Game& g, int player) {
    for (auto& u : g.units) { if (u.owner == player) { u.hasMoved = false; u.didAttack = false; u.maxHp = getUnitMaxHp(u); u.moveRange = getUnitMoveRange(u); bool onVillage = false; bool onOasis = false; for (auto& v : g.villages) { if (v.x == u.x && v.y == u.y && v.owner == player) { onVillage = true; break; } } if (g.map[u.x][u.y] == OASIS) onOasis = true; SDL_FPoint p = hexToPixel(u.x, u.y); if (onVillage) { int healAmt = unitTypes[u.typeIdx].healthy ? 5 : 3; u.hp = std::min(u.maxHp, u.hp + healAmt); addFloatText(g, p.x, p.y, ("+" + std::to_string(healAmt)).c_str(), {0,255,0,255}); u.isPoisoned = false; } else if (onOasis) { u.hp = std::min(u.maxHp, u.hp + 5); addFloatText(g, p.x, p.y, "+5 Oasis", {0,255,0,255}); u.isPoisoned = false; } TerrainType t = g.map[u.x][u.y]; if (t == MEADOW && unitTypes[u.typeIdx].align != CHAOTIC) { u.hp = std::min(u.maxHp, u.hp + 2); addFloatText(g, p.x, p.y, "+2 Meadow", {100, 255, 100, 255}); } if (t == BONES && unitTypes[u.typeIdx].align == CHAOTIC) { u.hp = std::min(u.maxHp, u.hp + 2); addFloatText(g, p.x, p.y, "+2 Bones", {255, 100, 100, 255}); } if (t == SWAMP && !unitTypes[u.typeIdx].healthy && !unitTypes[u.typeIdx].hasRegen) { u.hp -= 2; addFloatText(g, p.x, p.y, "-2 Swamp", {50, 150, 50, 255}); spawnHitParticles(g, p.x, p.y, {50, 150, 50, 255}, 3); } if (t == LAVA && !unitTypes[u.typeIdx].flying) { u.hp -= 4; addFloatText(g, p.x, p.y, "-4 Lava", {255, 50, 0, 255}); spawnHitParticles(g, p.x, p.y, {255, 50, 0, 255}, 5); } if (u.isPoisoned) { u.hp -= 3; addFloatText(g, p.x, p.y, "-3 Poison", {100,255,100,255}); spawnHitParticles(g, p.x, p.y, {50,200,50,255}, 4); } if (unitTypes[u.typeIdx].hasRegen) { u.hp = std::min(u.maxHp, u.hp + 5); addFloatText(g, p.x, p.y, "+5 Regen", {100,255,100,255}); } } }
    for (auto& u : g.units) { if (u.owner == player && unitTypes[u.typeIdx].hasHealAura) { for (int dy = -1; dy <= 1; ++dy) { for (int dx = -1; dx <= 1; ++dx) { if (dx == 0 && dy == 0) continue; int nx = u.x + dx, ny = u.y + dy; if (!isAdjacent(u.x, u.y, nx, ny)) continue; int allyIdx = findUnitAt(g, nx, ny); if (allyIdx != -1 && g.units[allyIdx].owner == player && g.units[allyIdx].hp < g.units[allyIdx].maxHp) { g.units[allyIdx].hp = std::min(g.units[allyIdx].maxHp, g.units[allyIdx].hp + 5); SDL_FPoint p = hexToPixel(g.units[allyIdx].x, g.units[allyIdx].y); addFloatText(g, p.x, p.y, "+5 Heal", {255,255,100,255}); } } } } }
    int villageCount = 0; for (auto& v : g.villages) if (v.owner == player) villageCount++; g.gold[player] += villageCount * 5; removeDeadUnits(g); calculateVisibility(g, player);
}

void endTurn(Game& g) { for (auto& u : g.units) { if (u.owner == g.currentPlayer) u.isSlowed = false; } g.currentPlayer = 1 - g.currentPlayer; g.selectedUnitIndex = -1; g.showRecruitMenu = false; g.currentPath.clear(); if (g.currentPlayer == 0) { g.turnCount++; g.timeOfDay = (TimeOfDay)((g.timeOfDay + 1) % 6); } startTurn(g, g.currentPlayer); if (g.currentPlayer == 1) g.aiState = AI_IDLE; playSound(g.audioStream, 300, 0.2, 0.3); }

// ==================== SDL3 App Architecture ====================
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) return SDL_APP_FAILURE;
    AppState* state = new AppState(); state->game = new Game(); state->lastTime = SDL_GetTicksNS();
    char* prefPath = SDL_GetPrefPath("WesnothClone", "WesnothClone"); if (prefPath) { state->game->savePath = std::string(prefPath) + "savegame.txt"; SDL_free(prefPath); }
    if (!SDL_CreateWindowAndRenderer("SDL3 Wesnoth Highlander Edition", WINDOW_W, WINDOW_H, 0, &state->window, &state->renderer)) { return SDL_APP_FAILURE; }
    SDL_SetRenderVSync(state->renderer, 1);
    state->cursorDefault = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT); state->cursorAttack = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR); state->cursorMove = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZE_ALL); state->cursorSelect = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND); SDL_SetCursor(state->cursorDefault);
    SDL_AudioSpec spec = {44100, SDL_AUDIO_F32, 1}; state->game->audioStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, nullptr, nullptr); if (state->game->audioStream) SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(state->game->audioStream));
    
    terrainTex[0] = LoadSVG(state->renderer, grassSVG, 200, 200); terrainTex[1] = LoadSVG(state->renderer, forestSVG, 200, 200); terrainTex[2] = LoadSVG(state->renderer, mountainSVG, 200, 200); terrainTex[3] = LoadSVG(state->renderer, villageSVG, 200, 200); terrainTex[4] = LoadSVG(state->renderer, keepSVG, 200, 200); terrainTex[5] = LoadSVG(state->renderer, swampSVG, 200, 200); terrainTex[6] = LoadSVG(state->renderer, sandSVG, 200, 200); terrainTex[7] = LoadSVG(state->renderer, hillsSVG, 200, 200); terrainTex[8] = LoadSVG(state->renderer, waterSVG, 200, 200); terrainTex[9] = LoadSVG(state->renderer, castleSVG, 200, 200); terrainTex[10] = LoadSVG(state->renderer, bridgeSVG, 200, 200); terrainTex[11] = LoadSVG(state->renderer, fordSVG, 200, 200); terrainTex[12] = LoadSVG(state->renderer, lavaSVG, 200, 200); terrainTex[13] = LoadSVG(state->renderer, oasisSVG, 200, 200); terrainTex[14] = LoadSVG(state->renderer, snowSVG, 200, 200); terrainTex[15] = LoadSVG(state->renderer, iceSVG, 200, 200); terrainTex[16] = LoadSVG(state->renderer, roadSVG, 200, 200); terrainTex[17] = LoadSVG(state->renderer, caveSVG, 200, 200); terrainTex[18] = LoadSVG(state->renderer, meadowSVG, 200, 200); terrainTex[19] = LoadSVG(state->renderer, bonesSVG, 200, 200); terrainTex[20] = LoadSVG(state->renderer, deepWaterSVG, 200, 200);
    
    unitTypes = {
        {"Scout", 20, 3, 3, 5, 1, 6, NEUTRAL, 15, scoutSVG, false, false, false, false, true, false, false, false, false, false, false, false, false, false, false, true}, 
        {"Fighter", 35, 2, 5, 3, 1, 4, LAWFUL, 25, fighterSVG, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false},
        {"Archer", 25, 3, 4, 3, 2, 4, LAWFUL, 30, archerSVG, false, false, false, true, false, false, false, false, false, false, false, false, false, false, false, false},
        {"Mage", 18, 2, 4, 3, 2, 4, LAWFUL, 35, mageSVG, false, false, true, true, false, true, false, true, true, false, true, false, false, false, false, false},
        {"Grunt", 35, 2, 6, 3, 1, 4, CHAOTIC, 25, gruntSVG, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false},
        {"Goblin Archer", 25, 3, 4, 3, 2, 4, CHAOTIC, 30, goblinArcherSVG, true, false, false, true, false, true, false, false, false, false, false, false, false, false, false, false},
        {"Wolf Rider", 25, 2, 4, 5, 1, 4, CHAOTIC, 20, wolfRiderSVG, false, false, false, false, true, false, false, false, false, false, false, true, false, false, false, true}, 
        {"Troll", 45, 2, 8, 3, 1, 4, CHAOTIC, 30, trollSVG, false, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false},
        
        {"Paladin", 50, 3, 6, 3, 1, 4, LAWFUL, 0, paladinSVG, false, false, true, false, false, false, true, false, false, false, false, false, false, false, false, false},
        {"Ranger", 35, 4, 5, 4, 2, 6, LAWFUL, 0, rangerSVG, false, false, false, true, true, false, false, false, false, false, false, false, false, false, false, true}, 
        {"Warlord", 55, 3, 7, 3, 1, 4, CHAOTIC, 0, warlordSVG, false, false, false, false, false, false, true, false, false, false, false, false, false, false, false, false},
        {"Assassin", 30, 4, 5, 4, 2, 4, CHAOTIC, 0, assassinSVG, true, false, false, true, true, false, false, false, false, false, false, false, false, true, false, false},
        
        {"Archmage", 30, 3, 6, 3, 2, 5, LAWFUL, 0, archmageSVG, false, false, true, true, false, true, false, true, true, false, true, true, false, false, true, false}, 
        {"Marauder", 40, 3, 6, 6, 1, 4, CHAOTIC, 0, marauderSVG, false, false, false, false, true, false, false, false, false, false, false, true, false, false, false, true}, 
        {"Troll Warrior", 65, 3, 10, 3, 1, 4, CHAOTIC, 0, trollWarriorSVG, false, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false}
    };
    for (int i = 0; i < 15; ++i) unitTextures[i] = LoadSVG(state->renderer, unitTypes[i].svg, 100, 100);

    Game& g = *state->game; g.map.resize(GRID_W, std::vector<TerrainType>(GRID_H, GRASS));
    for(int r=0; r<GRID_H; r++) { for(int c=0; c<GRID_W; c++) { if (r <= 1) g.map[c][r] = SNOW; else if (r >= GRID_H - 2) g.map[c][r] = SAND; else g.map[c][r] = GRASS; } }
    g.map[0][0] = KEEP; g.map[GRID_W-1][GRID_H-1] = KEEP; g.map[1][0] = CASTLE; g.map[0][1] = CASTLE; g.map[GRID_W-2][GRID_H-1] = CASTLE; g.map[GRID_W-1][GRID_H-2] = CASTLE;
    for(int r=2; r<GRID_H-2; r++) { int riverC = GRID_W/2 + (rand()%3 - 1); g.map[riverC][r] = DEEP_WATER; if (rand() % 3 == 0 && riverC > 0) g.map[riverC-1][r] = WATER; if (rand() % 3 == 0 && riverC < GRID_W-1) g.map[riverC+1][r] = WATER; }
    for(int r=2; r<GRID_H-2; r++) { int c = GRID_W/2; if (g.map[c][r] == DEEP_WATER || g.map[c][r] == WATER) { if (rand() % 2 == 0) g.map[c][r] = BRIDGE; else g.map[c][r] = FORD; } }
    for(int c=1; c<GRID_W/2; c++) { if(g.map[c][0] == GRASS || g.map[c][0] == SNOW) g.map[c][0] = ROAD; if(g.map[c][GRID_H-1] == GRASS || g.map[c][GRID_H-1] == SAND) g.map[c][GRID_H-1] = ROAD; }
    for (int i = 0; i < 4; ++i) { int x = rand()%GRID_W, y = rand()%2; if (g.map[x][y] == SNOW) g.map[x][y] = ICE; }
    for (int i = 0; i < 3; ++i) { int x = rand()%GRID_W, y = rand()%2; if (g.map[x][y] == SNOW) g.map[x][y] = FOREST; }
    for (int i = 0; i < 4; ++i) { int x = rand()%GRID_W, y = 2 + rand()%(GRID_H-4); if (g.map[x][y] == GRASS) g.map[x][y] = MEADOW; }
    for (int i = 0; i < 8; ++i) { int x = rand()%GRID_W, y = 2 + rand()%(GRID_H-4); if (g.map[x][y] == GRASS) g.map[x][y] = FOREST; }
    for (int i = 0; i < 5; ++i) { int x = rand()%GRID_W, y = 2 + rand()%(GRID_H-4); if (g.map[x][y] == GRASS) g.map[x][y] = HILLS; }
    for (int i = 0; i < 3; ++i) { int x = rand()%GRID_W, y = 2 + rand()%(GRID_H-4); if (g.map[x][y] == HILLS) g.map[x][y] = MOUNTAIN; }
    for (int i = 0; i < 2; ++i) { int x = rand()%GRID_W, y = 2 + rand()%(GRID_H-4); if (g.map[x][y] == MOUNTAIN) g.map[x][y] = CAVE; }
    for (int i = 0; i < 3; ++i) { int x = rand()%GRID_W, y = GRID_H-2 + rand()%2; if (g.map[x][y] == SAND) g.map[x][y] = LAVA; }
    for (int i = 0; i < 3; ++i) { int x = rand()%GRID_W, y = GRID_H-2 + rand()%2; if (g.map[x][y] == SAND) g.map[x][y] = OASIS; }
    for (int i = 0; i < 4; ++i) { int x = rand()%GRID_W, y = GRID_H-2 + rand()%2; if (g.map[x][y] == SAND) g.map[x][y] = BONES; }
    for (int i = 0; i < 8; ++i) { int x = rand()%GRID_W, y = rand()%GRID_H; if (g.map[x][y] == GRASS || g.map[x][y] == SAND || g.map[x][y] == HILLS || g.map[x][y] == SNOW || g.map[x][y] == MEADOW) { g.map[x][y] = VILLAGE; g.villages.push_back({x, y, -1}); } }
    
    g.units.push_back({0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, false, true, false, false, 0.0f, 0, 0, false, 0.0f, 0, 0});
    g.units.push_back({1, 0, 0, 0, 0, 1, 2, 0, 0, 0, 0, false, false, false, false, 0.0f, 0, 0, false, 0.0f, 0, 0});
    g.units.push_back({0, 1, 0, 0, 0, 1, 3, 0, 0, 0, 0, false, false, false, false, 0.0f, 0, 0, false, 0.0f, 0, 0}); 
    g.units.push_back({GRID_W-1, GRID_H-1, 0, 0, 0, 1, 4, 1, 0, 0, 0, false, true, false, false, 0.0f, 0, 0, false, 0.0f, 0, 0});
    g.units.push_back({GRID_W-2, GRID_H-1, 0, 0, 0, 1, 5, 1, 0, 0, 0, false, false, false, false, 0.0f, 0, 0, false, 0.0f, 0, 0});
    g.units.push_back({GRID_W-1, GRID_H-2, 0, 0, 0, 1, 7, 1, 0, 0, 0, false, false, false, false, 0.0f, 0, 0, false, 0.0f, 0, 0}); 
    
    for (auto& u : g.units) { u.traits = generateTraits(); u.maxHp = getUnitMaxHp(u); u.hp = u.maxHp; u.moveRange = getUnitMoveRange(u); }
    calculateVisibility(g, 0);
    *appstate = state; return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    AppState* state = (AppState*)appstate; Game& g = *state->game;
    if (event->type == SDL_EVENT_QUIT) return SDL_APP_SUCCESS;
    else if (event->type == SDL_EVENT_KEY_DOWN) {
        if (event->key.key == SDLK_ESCAPE) { g.selectedUnitIndex = -1; g.showRecruitMenu = false; g.currentPath.clear(); }
        else if (event->key.key == SDLK_SPACE && g.currentPlayer == 0 && g.animatingUnitIndex == -1) endTurn(g);
        else if (event->key.key == SDLK_D) g.showDefenseOverlay = !g.showDefenseOverlay;
        else if (event->key.key == SDLK_F5) { saveGame(g); addFloatText(g, WINDOW_W/2, 100, "Game Saved", {255,255,255,255}); }
        else if (event->key.key == SDLK_F9) { loadGame(g); addFloatText(g, WINDOW_W/2, 100, "Game Loaded", {255,255,255,255}); }
        else if (event->key.key == SDLK_U && g.selectedUnitIndex != -1 && g.currentPlayer == 0) { Unit& u = g.units[g.selectedUnitIndex]; if (u.hasMoved && !u.didAttack) { u.x = u.prevX; u.y = u.prevY; u.hasMoved = false; g.selectedUnitIndex = -1; g.currentPath.clear(); calculateVisibility(g, 0); } }
    }
    else if (event->type == SDL_EVENT_MOUSE_MOTION) {
        SDL_Point hex = pixelToHex(event->motion.x, event->motion.y); g.hoverX = hex.x; g.hoverY = hex.y;
        if (g.selectedUnitIndex != -1 && g.reachable[hex.x][hex.y]) { Unit& sel = g.units[g.selectedUnitIndex]; g.currentPath = reconstructPath(g, hex.x, hex.y, sel.x, sel.y); } else { g.currentPath.clear(); }
        if (g.currentPlayer == 0 && !g.gameOver) { if (g.selectedUnitIndex != -1) { if (g.attackable[hex.x][hex.y]) SDL_SetCursor(state->cursorAttack); else if (g.reachable[hex.x][hex.y]) SDL_SetCursor(state->cursorMove); else SDL_SetCursor(state->cursorDefault); } else { int hoverIdx = findUnitAt(g, hex.x, hex.y); if (hoverIdx != -1 && g.units[hoverIdx].owner == 0 && !g.units[hoverIdx].hasMoved) SDL_SetCursor(state->cursorSelect); else SDL_SetCursor(state->cursorDefault); } }
    }
    else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        if (event->button.button == SDL_BUTTON_RIGHT) { g.selectedUnitIndex = -1; g.showRecruitMenu = false; g.currentPath.clear(); return SDL_APP_CONTINUE; }
        if (event->button.button != SDL_BUTTON_LEFT) return SDL_APP_CONTINUE;
        if (g.gameOver) return SDL_APP_SUCCESS;
        if (g.animatingUnitIndex != -1 || g.currentPlayer == 1) return SDL_APP_CONTINUE;
        float mx = event->button.x, my = event->button.y;
        if (mx > WINDOW_W - 120 && mx < WINDOW_W - 20 && my > WINDOW_H - 40 && my < WINDOW_H - 10) { endTurn(g); return SDL_APP_CONTINUE; }
        if (g.showRecruitMenu) {
            for (int i = 0; i < 4; ++i) { SDL_FRect r = {WINDOW_W - 220.0f, 150.0f + i * 60, 200.0f, 50.0f}; if (mx > r.x && mx < r.x + r.w && my > r.y && my < r.y + r.h) { if (g.gold[0] >= unitTypes[i].cost) { for (int dy = -1; dy <= 1; ++dy) { for (int dx = -1; dx <= 1; ++dx) { int nx = 0 + dx, ny = 0 + dy; if (!isAdjacent(0, 0, nx, ny)) continue; if (nx < 0 || nx >= GRID_W || ny < 0 || ny >= GRID_H) continue; if (findUnitAt(g, nx, ny) == -1 && g.map[nx][ny] != MOUNTAIN && g.map[nx][ny] != WATER && g.map[nx][ny] != LAVA && g.map[nx][ny] != ICE && g.map[nx][ny] != DEEP_WATER) { g.gold[0] -= unitTypes[i].cost; Unit newU = {nx, ny, 0, 0, 0, 1, i, 0, 0, 0, generateTraits(), false, false, false, false, 0.0f, nx, ny, false, 0.0f, 0, 0}; newU.maxHp = getUnitMaxHp(newU); newU.hp = newU.maxHp; newU.moveRange = getUnitMoveRange(newU); g.units.push_back(newU); calculateVisibility(g, 0); g.showRecruitMenu = false; playSound(g.audioStream, 400, 0.1, 0.4); return SDL_APP_CONTINUE; } } } } } }
            g.showRecruitMenu = false; return SDL_APP_CONTINUE;
        }
        SDL_Point hex = pixelToHex(mx, my);
        if (hex.x < 0 || hex.x >= GRID_W || hex.y < 0 || hex.y >= GRID_H) return SDL_APP_CONTINUE;
        int clickedIdx = findUnitAt(g, hex.x, hex.y);
        if (g.selectedUnitIndex != -1) {
            Unit& sel = g.units[g.selectedUnitIndex];
            if (!sel.hasMoved) {
                if (unitTypes[sel.typeIdx].canTeleport) { bool isFriendlyVillage = false; for (auto& v : g.villages) { if (v.x == hex.x && v.y == hex.y && v.owner == sel.owner && findUnitAt(g, hex.x, hex.y) == -1) { isFriendlyVillage = true; break; } } if (isFriendlyVillage) { SDL_FPoint p1 = hexToPixel(sel.x, sel.y); spawnHitParticles(g, p1.x, p1.y, {150, 80, 200, 255}, 10); sel.x = hex.x; sel.y = hex.y; SDL_FPoint p2 = hexToPixel(sel.x, sel.y); spawnHitParticles(g, p2.x, p2.y, {150, 80, 200, 255}, 10); sel.hasMoved = true; playSound(g.audioStream, 600, 0.2, 0.5); g.selectedUnitIndex = -1; calculateVisibility(g, 0); return SDL_APP_CONTINUE; } }
                if (unitTypes[sel.typeIdx].canHeal && clickedIdx != -1) { Unit& target = g.units[clickedIdx]; if (target.owner == sel.owner && isAdjacent(sel.x, sel.y, target.x, target.y) && target.hp < target.maxHp) { target.hp = std::min(target.maxHp, target.hp + 8); SDL_FPoint p = hexToPixel(target.x, target.y); addFloatText(g, p.x, p.y, "+8 Heal", {100, 255, 100, 255}); spawnHitParticles(g, p.x, p.y, {100, 255, 100, 255}, 8); sel.hasMoved = true; playSound(g.audioStream, 700, 0.2, 0.5); g.selectedUnitIndex = -1; return SDL_APP_CONTINUE; } }
                if (g.attackable[hex.x][hex.y] && clickedIdx != -1 && g.units[clickedIdx].owner != sel.owner) { int atkRange = unitTypes[sel.typeIdx].attackRange; int dist = hexDistance(sel.x, sel.y, hex.x, hex.y); if (dist <= atkRange) { sel.didAttack = true; performAttack(g, sel, g.units[clickedIdx]); sel.hasMoved = true; removeDeadUnits(g); for (auto& u : g.units) checkLevelUp(g, u); calculateVisibility(g, 0); g.selectedUnitIndex = -1; g.currentPath.clear(); } }
                else if (g.reachable[hex.x][hex.y] && clickedIdx == -1) { sel.prevX = sel.x; sel.prevY = sel.y; g.currentPath = reconstructPath(g, hex.x, hex.y, sel.x, sel.y); g.animatingUnitIndex = g.selectedUnitIndex; sel.hasMoved = true; g.selectedUnitIndex = -1; playSound(g.audioStream, 250, 0.05, 0.3); }
            }
        } else {
            if (clickedIdx != -1 && g.visible[hex.x][hex.y]) { Unit& u = g.units[clickedIdx]; if (u.owner == g.currentPlayer && !u.hasMoved) { g.selectedUnitIndex = clickedIdx; calculateReachable(g, u); playSound(g.audioStream, 500, 0.05, 0.3); } } else if (g.map[hex.x][hex.y] == KEEP && g.currentPlayer == 0) { int idx = findUnitAt(g, hex.x, hex.y); if (idx != -1 && g.units[idx].owner == 0 && g.units[idx].isLeader) g.showRecruitMenu = true; }
        }
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    AppState* state = (AppState*)appstate; Game& g = *state->game; Uint64 now = SDL_GetTicksNS(); float dt = (now - state->lastTime) / 1000000000.0f; state->lastTime = now; g.globalTime += dt;
    if (g.animatingUnitIndex != -1 && !g.currentPath.empty()) { if (g.animatingUnitIndex >= g.units.size()) { g.animatingUnitIndex = -1; g.currentPath.clear(); } else { Unit& u = g.units[g.animatingUnitIndex]; u.animProgress += dt * 6.0f; if (u.animProgress >= 1.0f) { u.x = g.currentPath[0].x; u.y = g.currentPath[0].y; u.animProgress = 0.0f; SDL_FPoint p = hexToPixel(u.x, u.y); spawnDustParticles(g, p.x, p.y + 15, 3); g.currentPath.erase(g.currentPath.begin()); if (g.currentPath.empty()) { for (auto& v : g.villages) { if (v.x == u.x && v.y == u.y && v.owner != u.owner) { v.owner = u.owner; SDL_FPoint p = hexToPixel(u.x, u.y); addFloatText(g, p.x, p.y, "CAPTURED", {255,200,0,255}); } } if (u.owner == 0) calculateVisibility(g, 0); g.animatingUnitIndex = -1; } } } }
    for (auto& u : g.units) { if (u.lungeProgress > 0.0f) { u.lungeProgress += dt * 4.0f; if (u.lungeProgress >= 1.0f) u.lungeProgress = 0.0f; } }
    updateAI(g, dt); updateParticles(g, dt);
    for (auto& ft : g.floatTexts) { ft.y += ft.vy * dt; ft.life -= dt; } g.floatTexts.erase(std::remove_if(g.floatTexts.begin(), g.floatTexts.end(), [](const FloatText& f){ return f.life <= 0; }), g.floatTexts.end());
    for (auto& ca : g.combatAnims) ca.life -= dt; g.combatAnims.erase(std::remove_if(g.combatAnims.begin(), g.combatAnims.end(), [](const CombatAnim& c){ return c.life <= 0; }), g.combatAnims.end());
    if (g.screenShake > 0.0f) g.screenShake -= dt * 15.0f; if (g.screenShake < 0.0f) g.screenShake = 0.0f;
    
    SDL_SetRenderDrawColor(state->renderer, 5, 5, 10, 255); SDL_RenderClear(state->renderer);
    float shakeX = (rand() % 100 - 50) * g.screenShake * 0.1f; float shakeY = (rand() % 100 - 50) * g.screenShake * 0.1f;
    for (int r = 0; r < GRID_H; ++r) { for (int c = 0; c < GRID_W; ++c) { if (!g.explored[c][r]) continue; SDL_FPoint p = hexToPixel(c, r); float w = std::sqrt(3.0f) * HEX_SIZE, h = 2.0f * HEX_SIZE; SDL_FRect dest = {p.x - w/2 + shakeX, p.y - h/2 + shakeY, w, h}; TerrainType t = g.map[c][r]; SDL_Texture* tex = terrainTex[t]; if (!g.visible[c][r]) { SDL_SetTextureColorMod(tex, 70, 70, 70); } else { if (g.selectedUnitIndex != -1) { if (g.attackable[c][r]) SDL_SetTextureColorMod(tex, 255, 100, 100); else if (g.reachable[c][r]) SDL_SetTextureColorMod(tex, 130, 180, 255); else SDL_SetTextureColorMod(tex, 255, 255, 255); } else { if (g.hoverX == c && g.hoverY == r) SDL_SetTextureColorMod(tex, 220, 220, 160); else SDL_SetTextureColorMod(tex, 255, 255, 255); } } SDL_RenderTexture(state->renderer, tex, nullptr, &dest);
        if (g.visible[c][r]) { if (t == LAVA) { float pulse = (std::sin(g.globalTime * 3.0f) + 1.0f) / 2.0f; Uint8 alpha = 100 + (Uint8)(100 * pulse); SDL_SetRenderDrawColor(state->renderer, 255, 100, 0, alpha); SDL_SetRenderDrawBlendMode(state->renderer, SDL_BLENDMODE_ADD); SDL_RenderFillRect(state->renderer, &dest); SDL_SetRenderDrawBlendMode(state->renderer, SDL_BLENDMODE_BLEND); } else if (t == WATER || t == DEEP_WATER) { float wave = std::sin(g.globalTime * 2.0f + c * 0.5f + r * 0.5f); if (wave > 0.7f) { SDL_SetRenderDrawColor(state->renderer, 255, 255, 255, 50); SDL_SetRenderDrawBlendMode(state->renderer, SDL_BLENDMODE_ADD); SDL_FRect waveRect = {p.x - 10 + shakeX, p.y + wave * 5 - 10 + shakeY, 20, 4}; SDL_RenderFillRect(state->renderer, &waveRect); SDL_SetRenderDrawBlendMode(state->renderer, SDL_BLENDMODE_BLEND); } } }
        if (t == VILLAGE && g.visible[c][r]) { for (auto& v : g.villages) { if (v.x == c && v.y == r && v.owner != -1) { SDL_FRect flag = {p.x - 5 + shakeX, p.y - 25 + shakeY, 10, 10}; SDL_SetRenderDrawColor(state->renderer, v.owner == 0 ? 50 : 255, v.owner == 0 ? 100 : 50, v.owner == 0 ? 255 : 50, 255); SDL_RenderFillRect(state->renderer, &flag); break; } } }
        if (g.showDefenseOverlay && g.visible[c][r]) { int def = getDefense(t); SDL_Color defColor; if (def <= 10) defColor = {200, 50, 50, 255}; else if (def <= 30) defColor = {200, 150, 50, 255}; else if (def <= 50) defColor = {100, 200, 50, 255}; else defColor = {50, 200, 200, 255}; SDL_FRect bgRect = {p.x - 12 + shakeX, p.y - 8 + shakeY, 24, 16}; SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, 220); SDL_RenderFillRect(state->renderer, &bgRect); SDL_FRect innerRect = {p.x - 10 + shakeX, p.y - 6 + shakeY, 20, 12}; SDL_SetRenderDrawColor(state->renderer, defColor.r, defColor.g, defColor.b, defColor.a); SDL_RenderFillRect(state->renderer, &innerRect); std::string defStr = std::to_string(def); SDL_SetRenderDrawColor(state->renderer, 255, 255, 255, 255); SDL_RenderDebugText(state->renderer, p.x - (def >= 10 ? 6 : 3) + shakeX, p.y - 5 + shakeY, defStr.c_str()); }
    } }
    if (g.currentPath.size() > 0 && g.animatingUnitIndex == -1) { for (size_t i = 0; i < g.currentPath.size(); ++i) { if (!g.visible[g.currentPath[i].x][g.currentPath[i].y]) continue; SDL_FPoint p = hexToPixel(g.currentPath[i].x, g.currentPath[i].y); SDL_FRect dot = {p.x - 4 + shakeX, p.y - 4 + shakeY, 8, 8}; SDL_SetRenderDrawColor(state->renderer, 100, 200, 255, 200); SDL_RenderFillRect(state->renderer, &dot); } }
    for (auto& p : g.particles) { Uint8 alpha = (int)(255 * std::min(1.0f, p.life)); SDL_SetRenderDrawColor(state->renderer, p.color.r, p.color.g, p.color.b, alpha); SDL_FRect pr = {p.x - p.size/2 + shakeX, p.y - p.size/2 + shakeY, p.size, p.size}; SDL_RenderFillRect(state->renderer, &pr); }
    for (size_t i = 0; i < g.units.size(); ++i) {
        auto& u = g.units[i]; if (u.owner != 0 && !g.visible[u.x][u.y]) continue; SDL_FPoint drawPos;
        if ((int)i == g.animatingUnitIndex && !g.currentPath.empty()) { SDL_FPoint p1 = hexToPixel(u.x, u.y); SDL_FPoint p2 = hexToPixel(g.currentPath[0].x, g.currentPath[0].y); drawPos.x = p1.x + (p2.x - p1.x) * u.animProgress; drawPos.y = p1.y + (p2.y - p1.y) * u.animProgress; } else { drawPos = hexToPixel(u.x, u.y); }
        if (u.lungeProgress > 0.0f) { SDL_FPoint targetPos = hexToPixel(u.lungeTargetX, u.lungeTargetY); float lungeAmt = std::sin(u.lungeProgress * M_PI) * 0.3f; drawPos.x += (targetPos.x - drawPos.x) * lungeAmt; drawPos.y += (targetPos.y - drawPos.y) * lungeAmt; }
        drawPos.x += shakeX; drawPos.y += shakeY; SDL_FRect dest = {drawPos.x - 22, drawPos.y - 22, 44, 44};
        if (u.hasMoved) SDL_SetTextureAlphaMod(unitTextures[u.typeIdx], 140); else SDL_SetTextureAlphaMod(unitTextures[u.typeIdx], 255);
        if (u.isSlowed) SDL_SetTextureColorMod(unitTextures[u.typeIdx], 150, 150, 255); else if (u.isPoisoned) SDL_SetTextureColorMod(unitTextures[u.typeIdx], 150, 255, 150); else SDL_SetTextureColorMod(unitTextures[u.typeIdx], 255, 255, 255);
        SDL_RenderTexture(state->renderer, unitTextures[u.typeIdx], nullptr, &dest);
        if (u.isLeader) { SDL_FRect crown = {drawPos.x - 8, drawPos.y - 35, 16, 5}; SDL_FRect tip1 = {drawPos.x - 8, drawPos.y - 40, 5, 5}; SDL_FRect tip2 = {drawPos.x + 3, drawPos.y - 40, 5, 5}; SDL_FRect tip3 = {drawPos.x - 2, drawPos.y - 42, 4, 7}; SDL_SetRenderDrawColor(state->renderer, 255, 215, 0, 255); SDL_RenderFillRect(state->renderer, &crown); SDL_RenderFillRect(state->renderer, &tip1); SDL_RenderFillRect(state->renderer, &tip2); SDL_RenderFillRect(state->renderer, &tip3); }
        if ((int)i == g.selectedUnitIndex) { SDL_FRect ring = {drawPos.x - 24, drawPos.y - 24, 48, 48}; SDL_SetRenderDrawColor(state->renderer, 255, 255, 100, 255); SDL_RenderRect(state->renderer, &ring); }
        SDL_FRect bg = {drawPos.x - 15, drawPos.y + 16, 30, 6}; SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, 220); SDL_RenderFillRect(state->renderer, &bg); float hpPct = std::max(0.0f, (float)u.hp / (float)u.maxHp); SDL_FRect hp = {drawPos.x - 14, drawPos.y + 17, 28 * hpPct, 4}; SDL_SetRenderDrawColor(state->renderer, u.owner == 0 ? 50 : 200, u.owner == 0 ? 200 : 50, 50, 255); SDL_RenderFillRect(state->renderer, &hp);
        std::string hpStr = std::to_string(u.hp); SDL_SetRenderDrawColor(state->renderer, 255, 255, 255, 255); SDL_RenderDebugText(state->renderer, drawPos.x - 5, drawPos.y + 24, hpStr.c_str());
        if (u.level < 3) { SDL_FRect xp_bg = {drawPos.x - 15, drawPos.y + 31, 30, 2}; SDL_SetRenderDrawColor(state->renderer, 0, 0, 50, 200); SDL_RenderFillRect(state->renderer, &xp_bg); float xpPct = (float)u.xp / (float)getXpNeeded(u); SDL_FRect xp_fg = {drawPos.x - 14, drawPos.y + 31, 28 * xpPct, 2}; SDL_SetRenderDrawColor(state->renderer, 50, 100, 255, 255); SDL_RenderFillRect(state->renderer, &xp_fg); }
        if (u.isPoisoned) { SDL_FRect skull = {drawPos.x + 12, drawPos.y - 15, 6, 6}; SDL_SetRenderDrawColor(state->renderer, 50, 200, 50, 255); SDL_RenderFillRect(state->renderer, &skull); } if (u.isSlowed) { SDL_FRect snow = {drawPos.x - 18, drawPos.y - 15, 6, 6}; SDL_SetRenderDrawColor(state->renderer, 50, 50, 255, 255); SDL_RenderFillRect(state->renderer, &snow); } if (unitTypes[u.typeIdx].hasRegen) { SDL_FRect cross = {drawPos.x + 12, drawPos.y - 8, 6, 6}; SDL_SetRenderDrawColor(state->renderer, 255, 100, 100, 255); SDL_RenderFillRect(state->renderer, &cross); }
        if (u.level > 1) { SDL_SetRenderDrawColor(state->renderer, 255, 255, 0, 255); SDL_RenderDebugText(state->renderer, drawPos.x + 10, drawPos.y + 10, std::to_string(u.level).c_str()); }
    }
    for (auto& p : g.projectiles) { if (!p.active) continue; SDL_SetRenderDrawColor(state->renderer, p.color.r, p.color.g, p.color.b, 255); float angle = atan2(p.ty - p.y, p.tx - p.x); SDL_RenderLine(state->renderer, p.x, p.y, p.x - cos(angle)*12, p.y - sin(angle)*12); }
    for (auto& ca : g.combatAnims) { SDL_SetRenderDrawColor(state->renderer, ca.color.r, ca.color.g, ca.color.b, (int)(255 * ca.life / 0.4f)); SDL_RenderLine(state->renderer, ca.fromX + shakeX, ca.fromY + shakeY, ca.toX + shakeX, ca.toY + shakeY); }
    for (auto& ft : g.floatTexts) { Uint8 alpha = (int)(255 * std::min(1.0f, ft.life)); SDL_SetRenderDrawColor(state->renderer, ft.color.r, ft.color.g, ft.color.b, alpha); SDL_RenderDebugText(state->renderer, ft.x - 10 + shakeX, ft.y + shakeY, ft.text.c_str()); }
    
    SDL_SetRenderDrawBlendMode(state->renderer, SDL_BLENDMODE_MUL); SDL_Color tint = {255, 255, 255, 255}; if (g.timeOfDay == NIGHT) tint = {70, 70, 130, 255}; else if (g.timeOfDay == DAWN) tint = {255, 200, 130, 255}; else if (g.timeOfDay == DUSK) tint = {255, 150, 100, 255}; else if (g.timeOfDay == MORNING) tint = {255, 255, 230, 255}; SDL_SetRenderDrawColor(state->renderer, tint.r, tint.g, tint.b, 255); SDL_FRect fullScreen = {0, 0, (float)WINDOW_W, (float)WINDOW_H}; SDL_RenderFillRect(state->renderer, &fullScreen); SDL_SetRenderDrawBlendMode(state->renderer, SDL_BLENDMODE_BLEND);

    int mmSize = 120, mmX = WINDOW_W - mmSize - 10, mmY = 10; SDL_FRect mmBg = {(float)mmX, (float)mmY, (float)mmSize, (float)(mmSize * GRID_H / GRID_W)}; SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, 200); SDL_RenderFillRect(state->renderer, &mmBg); float hexW = (float)mmSize / GRID_W, hexH = (float)(mmSize * GRID_H / GRID_W) / GRID_H;
    for (int r = 0; r < GRID_H; ++r) { for (int c = 0; c < GRID_W; ++c) { if (!g.explored[c][r]) continue; SDL_FRect hex = {mmX + c * hexW, mmY + r * hexH, hexW, hexH}; TerrainType t = g.map[c][r]; SDL_Color c2; if (!g.visible[c][r]) c2 = {40,40,40,255}; else if (t == FOREST) c2 = {45,106,45,255}; else if (t == MOUNTAIN) c2 = {154,154,154,255}; else if (t == VILLAGE) c2 = {74,138,74,255}; else if (t == KEEP) c2 = {122,122,122,255}; else if (t == SWAMP) c2 = {74,90,58,255}; else if (t == SAND) c2 = {210,180,140,255}; else if (t == HILLS) c2 = {122,154,90,255}; else if (t == WATER) c2 = {42,90,138,255}; else if (t == CASTLE) c2 = {138,138,138,255}; else if (t == BRIDGE) c2 = {139,69,19,255}; else if (t == FORD) c2 = {74,138,186,255}; else if (t == LAVA) c2 = {255,85,0,255}; else if (t == OASIS) c2 = {45,106,45,255}; else if (t == SNOW) c2 = {240,248,255,255}; else if (t == ICE) c2 = {173,216,230,255}; else if (t == ROAD) c2 = {160,138,96,255}; else if (t == CAVE) c2 = {90,90,90,255}; else if (t == MEADOW) c2 = {124,186,124,255}; else if (t == BONES) c2 = {194,168,120,255}; else if (t == DEEP_WATER) c2 = {26,58,90,255}; else c2 = {90,154,90,255}; SDL_SetRenderDrawColor(state->renderer, c2.r, c2.g, c2.b, c2.a); SDL_RenderFillRect(state->renderer, &hex); } }
    for (auto& u : g.units) { if (u.owner == 0 || g.visible[u.x][u.y]) { SDL_FRect dot = {mmX + u.x * hexW, mmY + u.y * hexH, hexW, hexH}; SDL_SetRenderDrawColor(state->renderer, u.owner == 0 ? 50 : 255, u.owner == 0 ? 100 : 50, u.owner == 0 ? 255 : 50, 255); SDL_RenderFillRect(state->renderer, &dot); } }
    SDL_SetRenderDrawColor(state->renderer, 200, 200, 200, 255); SDL_RenderRect(state->renderer, &mmBg);
    
    SDL_FRect panel = {0, WINDOW_H - 60, WINDOW_W, 60}; SDL_SetRenderDrawColor(state->renderer, 10, 10, 10, 255); SDL_RenderFillRect(state->renderer, &panel); SDL_SetRenderDrawColor(state->renderer, 60, 60, 60, 255); SDL_RenderRect(state->renderer, &panel);
    SDL_SetRenderDrawColor(state->renderer, 255, 255, 255, 255); std::string turnStr = "Turn " + std::to_string(g.turnCount) + " | Player " + std::to_string(g.currentPlayer + 1); SDL_RenderDebugText(state->renderer, 10, WINDOW_H - 50, turnStr.c_str()); std::string goldStr = "Gold: " + std::to_string(g.gold[g.currentPlayer]); SDL_RenderDebugText(state->renderer, 10, WINDOW_H - 30, goldStr.c_str());
    SDL_FRect todRect = {160, WINDOW_H - 50, 20, 20}; SDL_SetRenderDrawColor(state->renderer, timeColors[g.timeOfDay].r, timeColors[g.timeOfDay].g, timeColors[g.timeOfDay].b, 255); SDL_RenderFillRect(state->renderer, &todRect); SDL_SetRenderDrawColor(state->renderer, 255, 255, 255, 255); SDL_RenderDebugText(state->renderer, 190, WINDOW_H - 45, timeNames[g.timeOfDay]); int mod = getDamageModifier(g.timeOfDay, LAWFUL); std::string modStr = mod > 0 ? "Lawful +" + std::to_string(mod) + "%" : mod < 0 ? "Lawful " + std::to_string(mod) + "%" : "Neutral"; SDL_RenderDebugText(state->renderer, 190, WINDOW_H - 28, modStr.c_str());
    
    auto drawUnitInfo = [&](Unit& u, SDL_Color color) { UnitType& ut = unitTypes[u.typeIdx]; int def = getUnitDefense(g, u); std::string info = std::string(ut.name) + " L" + std::to_string(u.level) + " | HP " + std::to_string(u.hp) + "/" + std::to_string(u.maxHp) + " | ATK " + std::to_string(getUnitAttacks(u)) + "x" + std::to_string(getUnitBaseDamage(u)) + " | DEF " + std::to_string(def) + "% | XP " + std::to_string(u.xp) + "/" + std::to_string(getXpNeeded(u)); std::string traits = " | Traits: "; if (u.traits & QUICK) traits += "Q"; if (u.traits & RESILIENT) traits += "R"; if (u.traits & STRONG) traits += "S"; if (u.traits & INTELLIGENT) traits += "I"; if (u.traits == 0) traits = ""; info += traits; if (u.isPoisoned) info += " | POISONED"; if (u.isSlowed) info += " | SLOWED"; if (u.isLeader) info += " | LEADER"; SDL_SetRenderDrawColor(state->renderer, color.r, color.g, color.b, color.a); SDL_RenderDebugText(state->renderer, 220, WINDOW_H - 45, info.c_str()); std::string abilities = "Abilities: "; if (ut.hasHealAura) abilities += "HealAura "; if (ut.firstStrike) abilities += "1stStrike "; if (ut.ignoresZoc) abilities += "Skirmish "; if (ut.hasPoison) abilities += "Poison "; if (ut.hasSlow) abilities += "Slow "; if (ut.hasRegen) abilities += "Regen "; if (ut.leadership) abilities += "Lead "; if (ut.canHeal) abilities += "Heal "; if (ut.canTeleport) abilities += "Teleport "; if (ut.stealth) abilities += "Stealth "; if (ut.magical) abilities += "Magical "; if (ut.hasDrain) abilities += "Drain "; if (ut.hasCharge) abilities += "Charge "; if (ut.hasBackstab) abilities += "Backstab "; if (ut.flying) abilities += "Flying "; if (ut.healthy) abilities += "Healthy "; if (abilities == "Abilities: ") abilities = "U=Undo  Esc=Deselect  Space=EndTurn  D=Defense  F5=Save  F9=Load"; SDL_SetRenderDrawColor(state->renderer, 150, 150, 250, 255); SDL_RenderDebugText(state->renderer, 220, WINDOW_H - 28, abilities.c_str()); };

    int hoverIdx = findUnitAt(g, g.hoverX, g.hoverY);
    if (g.selectedUnitIndex != -1 && g.selectedUnitIndex < (int)g.units.size()) { drawUnitInfo(g.units[g.selectedUnitIndex], {255, 255, 255, 255}); if (g.hoverX >= 0 && g.hoverX < GRID_W && g.hoverY >= 0 && g.hoverY < GRID_H && g.attackable[g.hoverX][g.hoverY]) { int tIdx = findUnitAt(g, g.hoverX, g.hoverY); if (tIdx != -1) { Unit& sel = g.units[g.selectedUnitIndex]; Unit& target = g.units[tIdx]; int dmg = getUnitDamageWithAuras(g, sel, target.x, target.y); int defChance = getUnitDefense(g, target); int hitChance = unitTypes[sel.typeIdx].magical ? 70 : (100 - defChance); int avgDmg = dmg * getUnitAttacks(sel) * hitChance / 100; std::string pred = "PRED: ~" + std::to_string(avgDmg) + " dmg"; int atkH = getTerrainHeight(g.map[sel.x][sel.y]); int defH = getTerrainHeight(g.map[target.x][target.y]); if (atkH > defH) pred += " | Height Adv!"; else if (atkH < defH) pred += " | Height Disadv!"; int dist = hexDistance(sel.x, sel.y, target.x, target.y); if (dist == 1 && unitTypes[target.typeIdx].attackRange >= 1) { int ctrDmg = getUnitDamageWithAuras(g, target, sel.x, sel.y); int atkChance = getUnitDefense(g, sel); int avgCtr = ctrDmg * getUnitAttacks(target) * (unitTypes[target.typeIdx].magical ? 70 : (100 - atkChance)) / 100; pred += " | Ctr: ~" + std::to_string(avgCtr); } else { pred += " | No counter"; } SDL_SetRenderDrawColor(state->renderer, 255, 200, 100, 255); SDL_RenderDebugText(state->renderer, 220, WINDOW_H - 12, pred.c_str()); } } }
    else if (hoverIdx != -1 && g.visible[g.hoverX][g.hoverY]) { drawUnitInfo(g.units[hoverIdx], {200, 200, 200, 255}); }
    else if (g.hoverX >= 0 && g.hoverX < GRID_W && g.hoverY >= 0 && g.hoverY < GRID_H && g.explored[g.hoverX][g.hoverY]) { TerrainType t = g.map[g.hoverX][g.hoverY]; std::string tName = t == GRASS ? "Grass" : t == FOREST ? "Forest" : t == MOUNTAIN ? "Mountain" : t == VILLAGE ? "Village" : t == KEEP ? "Keep" : t == SWAMP ? "Swamp" : t == SAND ? "Sand" : t == HILLS ? "Hills" : t == WATER ? "Water" : t == CASTLE ? "Castle" : t == BRIDGE ? "Bridge" : t == FORD ? "Ford" : t == LAVA ? "Lava" : t == OASIS ? "Oasis" : t == SNOW ? "Snow" : t == ICE ? "Ice" : t == ROAD ? "Road" : t == CAVE ? "Cave" : t == MEADOW ? "Meadow" : t == BONES ? "Bones" : "Deep Water"; std::string info = tName + " | H:" + std::to_string(getTerrainHeight(t)) + " | Def: " + std::to_string(getDefense(t)) + "% | Cost: " + (getMoveCost(t) >= 999 ? "N/A" : std::to_string(getMoveCost(t))); if (t == FOREST) info += " | Conceals"; if (t == SWAMP) info += " | Hazard: -2 HP"; if (t == LAVA) info += " | Hazard: -4 HP"; if (t == OASIS) info += " | Heal: +5 HP"; if (t == MEADOW) info += " | Heal Law/Neu: +2 HP"; if (t == BONES) info += " | Heal Chaotic: +2 HP"; if (t == WATER || t == ICE || t == DEEP_WATER) info += " | Impassable"; if (t == BRIDGE || t == FORD) info += " | Crosses Water"; if (t == ROAD) info += " | Fast Travel"; if (t == CAVE) info += " | High Defense"; SDL_SetRenderDrawColor(state->renderer, 200, 200, 200, 255); SDL_RenderDebugText(state->renderer, 220, WINDOW_H - 45, info.c_str()); std::string hint = "U=Undo  Esc=Deselect  Space=EndTurn  D=Defense  F5=Save  F9=Load"; SDL_SetRenderDrawColor(state->renderer, 120, 120, 120, 255); SDL_RenderDebugText(state->renderer, 220, WINDOW_H - 28, hint.c_str()); }
    
    SDL_FRect btnRect = {WINDOW_W - 120.0f, WINDOW_H - 40.0f, 100.0f, 30.0f}; SDL_SetRenderDrawColor(state->renderer, 100, 100, 150, 255); SDL_RenderFillRect(state->renderer, &btnRect); SDL_SetRenderDrawColor(state->renderer, 255, 255, 255, 255); SDL_RenderDebugText(state->renderer, WINDOW_W - 105, WINDOW_H - 28, "End Turn");
    if (g.showRecruitMenu) { SDL_FRect bg = {WINDOW_W - 230, 140, 210, 250}; SDL_SetRenderDrawColor(state->renderer, 20, 20, 20, 230); SDL_RenderFillRect(state->renderer, &bg); SDL_SetRenderDrawColor(state->renderer, 200, 200, 200, 255); SDL_RenderRect(state->renderer, &bg); SDL_RenderDebugText(state->renderer, WINDOW_W - 220, 145, "RECRUIT (Leader on Keep)"); for (int i = 0; i < 4; ++i) { SDL_FRect r = {WINDOW_W - 220.0f, 150.0f + i * 60, 200.0f, 50.0f}; SDL_SetRenderDrawColor(state->renderer, 60, 60, 80, 255); SDL_RenderFillRect(state->renderer, &r); SDL_SetRenderDrawColor(state->renderer, 150, 150, 180, 255); SDL_RenderRect(state->renderer, &r); SDL_RenderTexture(state->renderer, unitTextures[i], nullptr, &SDL_FRect{r.x + 2, r.y + 2, 46, 46}); SDL_SetRenderDrawColor(state->renderer, 255, 255, 255, 255); std::string label = std::string(unitTypes[i].name) + " (" + std::to_string(unitTypes[i].cost) + "g)"; SDL_RenderDebugText(state->renderer, r.x + 52, r.y + 8, label.c_str()); std::string stats = "HP:" + std::to_string(unitTypes[i].maxHp) + " ATK:" + std::to_string(unitTypes[i].attacks) + "x" + std::to_string(unitTypes[i].damage); if (unitTypes[i].canHeal) stats += " HEAL"; if (unitTypes[i].canTeleport) stats += " TELE"; if (unitTypes[i].magical) stats += " MAGC"; SDL_RenderDebugText(state->renderer, r.x + 52, r.y + 24, stats.c_str()); } }
    if (g.gameOver) { SDL_FRect goRect = {WINDOW_W/2 - 150, WINDOW_H/2 - 40, 300, 80}; SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, 230); SDL_RenderFillRect(state->renderer, &goRect); SDL_SetRenderDrawColor(state->renderer, 255, 255, 255, 255); std::string goText = "Player " + std::to_string(g.winner + 1) + " Wins! (Leader Killed)"; SDL_RenderDebugText(state->renderer, WINDOW_W/2 - 100, WINDOW_H/2 - 10, goText.c_str()); SDL_RenderDebugText(state->renderer, WINDOW_W/2 - 60, WINDOW_H/2 + 10, "Click to quit"); }
    SDL_RenderPresent(state->renderer);
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate) {
    AppState* state = (AppState*)appstate;
    if (state) { for (int i = 0; i < 21; ++i) SDL_DestroyTexture(terrainTex[i]); for (int i = 0; i < 15; ++i) SDL_DestroyTexture(unitTextures[i]); if (state->game && state->game->audioStream) SDL_DestroyAudioStream(state->game->audioStream); if (state->cursorDefault) SDL_DestroyCursor(state->cursorDefault); if (state->cursorAttack) SDL_DestroyCursor(state->cursorAttack); if (state->cursorMove) SDL_DestroyCursor(state->cursorMove); if (state->cursorSelect) SDL_DestroyCursor(state->cursorSelect); if (state->renderer) SDL_DestroyRenderer(state->renderer); if (state->window) SDL_DestroyWindow(state->window); delete state->game; delete state; }
    SDL_Quit();
}
