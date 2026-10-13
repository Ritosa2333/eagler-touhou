// Purple TH06 native hooks adapted to the portable title's typed owners.
// Shared algorithms are generated from THPrac (MIT), not rewritten here.
#include "PurpleRuntime.hpp"
#include "PracticeRuntime.hpp"
#include "PurpleLabels.hpp"
#include "PurpleVisuals.hpp"
#include "PurpleWall.hpp"
#include "PurpleSave.hpp"
#include "PurpleResources.hpp"
#include "PurplePlayerVisual.hpp"
#include "FileSystem.hpp"
#include "Controller.hpp"
#include "Enemy.hpp"
#include "GameManager.hpp"
#include "Rng.hpp"
#include "EclManager.hpp"
#include "GameWindow.hpp"
#include "EaglerOptions.hpp"
#include "graphics/Gles.hpp"
#include <SDL3_image/SDL_image.h>
#include "ThpracImGui.hpp"
#include <eagler/thprac/PracticeInput.hpp>
#include <eagler/thprac/PracticeSpeed.hpp>
#include <imgui_internal.h>
#include <eagler/thprac/PracticeKeyMonitor.hpp>
#include <eagler/thprac/PracticeMobilePanels.hpp>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <functional>
#include <random>
#include <vector>
#include <cfloat>
#include <SDL3/SDL.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace PracticeRuntime::Purple {
static int Locale();
bool enemyInvincible = false;
static bool disableRankDrop = false, bossMoveDown = false, fixedSeed = false;
static bool showKeys = false, showRank = false, showBulletHitbox = false, showReplayMarker = false;
static bool mapInfiniteLives = true;
static bool showLockTimer=false,showLifeThreshold=false;
static bool backgroundFix=false;
static int lockTimer=0,bossLife=0,bossMaxLife=0,bossThreshold=0;
static float lastBossX=0,lastBossY=0;
static bool showGrey=false,booksActive=false,booksDied=false;
static int grey=0,greyInBomb=0,bookMisses=0,bookBombs=0,misses=0;
static float bookPositions[6][2]{};
static int bookFrames=0;
static eagler::thprac::PracticeSpeed speed;
static bool showPlayerHitbox=false,blindView=false;
static float blindSize=150;
static ImTextureID hitboxTexture=nullptr,blindTexture=nullptr;
static ImVec2 hitboxSize{};
static bool hitboxFailed=false,blindFailed=false;
static ImTextureID LoadImage(const char* name,const void* fallback,int length,ImVec2& size) {
    auto* backend=dynamic_cast<GlesGraphics*>(g_GfxBackend);
    if(!backend)return nullptr;
    auto* image=IMG_Load(name);
    if(!image)image=IMG_Load_IO(SDL_IOFromConstMem(fallback,length),true);
    if(!image)return nullptr;
    auto* rgba=SDL_ConvertSurface(image,SDL_PIXELFORMAT_RGBA32);SDL_DestroySurface(image);
    if(!rgba)return nullptr;
    // SDL surfaces can carry row padding; upload tightly packed RGBA.
    std::vector<unsigned char> pixels(size_t(rgba->w)*rgba->h*4);
    for(int y=0;y<rgba->h;++y)std::memcpy(pixels.data()+size_t(y)*rgba->w*4,
        static_cast<const unsigned char*>(rgba->pixels)+size_t(y)*rgba->pitch,size_t(rgba->w)*4);
    const auto texture=backend->CreateImGuiTexture(rgba->w,rgba->h,pixels.data());
    size=ImVec2(float(rgba->w),float(rgba->h));SDL_DestroySurface(rgba);
    return reinterpret_cast<ImTextureID>(uintptr_t(texture));
}
static void ReleaseImage(ImTextureID& image,bool& failed) {
    if(image)if(auto* backend=dynamic_cast<GlesGraphics*>(g_GfxBackend))
        backend->ReleaseImGuiTexture(u32(uintptr_t(image)));
    image=nullptr;failed=false;
}
void ReleaseTextures() {ReleaseImage(hitboxTexture,hitboxFailed);ReleaseImage(blindTexture,blindFailed);}
static PurpleSave& Stats() {
    static PurpleSave stats;static bool loaded=false;
    if(!loaded) {
        loaded=true;
        if(auto* file=FileSystem::OpenFileStream("score06.dat","rb")) {
            int version=0;PurpleSave::Save candidate;
            if(SDL_ReadIO(file,&version,sizeof(version))==sizeof(version) && version==1 &&
                SDL_ReadIO(file,&candidate,sizeof(candidate))==sizeof(candidate))stats.total=candidate;
            SDL_CloseIO(file);
        } else if(auto* file=FileSystem::OpenFileStream("spell_capture.dat","rb")) {
            PurpleSave::Save candidate{};
            if(SDL_ReadIO(file,&candidate.SC_History,sizeof(candidate.SC_History))==sizeof(candidate.SC_History)) {
                SDL_ReadIO(file,&candidate.timePlayer,sizeof(candidate.timePlayer));
                for(auto& row:candidate.timePlayer)for(auto& value:row)
                    value=std::clamp<std::int64_t>(value,0,INT64_MAX/1000000)*1000000;
                stats.total=candidate;
            }
            SDL_CloseIO(file);
        }
    }
    return stats;
}
void SaveStatistics() {
    if(!Enabled())return;
    if(auto* file=FileSystem::OpenFileStream("score06.dat","wb")) {
        const int version=1;const auto& data=Stats().total;
        SDL_WriteIO(file,&version,sizeof(version));SDL_WriteIO(file,&data,sizeof(data));SDL_CloseIO(file);
    }
}
static void SpellRecord(int id,int state) {
    if(!Enabled() || g_GameManager.isInReplay)return;
    if(Stats().Add(id,g_GameManager.difficulty,g_GameManager.CharacterShotType(),state))SaveStatistics();
}
void SpellAttempt(int id) {SpellRecord(id,PurpleSave::Attempt);}
void SpellCapture(int id) {SpellRecord(id,PurpleSave::Capture);}
void SpellTimeout(int id) {
    if(!(id==1 && g_GameManager.currentStage!=1))SpellRecord(id,PurpleSave::Timeout);
}
void GreyItem(bool inBomb) {if(Enabled() && showGrey)(inBomb?greyInBomb:grey)++;}
static bool BooksCountable() {return !(AdvancedActive() && GetConfig().section==22 && GetConfig().phase!=0);}
void BookStart(unsigned offset,float x,float y) {
    if(!Enabled() || g_GameManager.currentStage!=4 || offset<0xcdd4 || offset>0xce60 || (offset-0xcdd4)%0x1c)return;
    const unsigned index=(offset-0xcdd4)/0x1c;
    bookPositions[index][0]=x;bookPositions[index][1]=y;
    if(index==0) {
        booksActive=true;booksDied=false;bookFrames=0;bookMisses=misses;bookBombs=g_GameManager.bombsUsed;
        if(BooksCountable())SpellAttempt(64);
    }
}
void BookEnd(unsigned offset) {
    if(!booksActive || g_GameManager.currentStage!=4 || offset!=0xce7c)return;
    booksActive=false;
    if(BooksCountable() && !booksDied && bookMisses==misses && bookBombs==g_GameManager.bombsUsed)SpellCapture(64);
    booksDied=false;
}
void BookFailed() {if(booksActive)booksDied=true;}
static std::string GetTime_HHMMSS(std::int64_t ns) {
    char text[128];std::snprintf(text,sizeof(text),"%lld:%02lld:%02lld.%03lld.%03lld.%03lld",
        (long long)(ns/3600000000000LL),(long long)(ns/60000000000LL%60),(long long)(ns/1000000000LL%60),
        (long long)(ns/1000000%1000),(long long)(ns/1000%1000),(long long)(ns%1000));return text;
}
static std::string GetTime_YYMMDD_HHMMSS(std::int64_t ns) {
    const std::int64_t values[]{ns/1000000000LL/60/60/24/30/12/100,ns/1000000000LL/60/60/24/30/12%100,
        ns/1000000000LL/60/60/24/30%12,ns/1000000000LL/60/60/24%30,ns/1000000000LL/60/60%24,
        ns/60000000000LL%60,ns/1000000000LL%60,ns/1000000%1000,ns/1000%1000,ns%1000};
    const char* units[]{purple_THPRAC_INGAMEINFO_06_GAMTIME_CENTRY[Locale()],purple_THPRAC_INGAMEINFO_06_GAMTIME_YEAR[Locale()],
        purple_THPRAC_INGAMEINFO_06_GAMTIME_MONTH[Locale()],purple_THPRAC_INGAMEINFO_06_GAMTIME_DAY[Locale()],
        purple_THPRAC_INGAMEINFO_06_GAMTIME_HOUR[Locale()],purple_THPRAC_INGAMEINFO_06_GAMTIME_MINUTE[Locale()],
        purple_THPRAC_INGAMEINFO_06_GAMTIME_SECOND[Locale()],purple_THPRAC_INGAMEINFO_06_GAMTIME_MILLISECOND[Locale()],
        purple_THPRAC_INGAMEINFO_06_GAMTIME_MICROSECOND[Locale()],purple_THPRAC_INGAMEINFO_06_GAMTIME_NANOSECOND[Locale()]};
    std::string result;bool started=false;
    for(int i=0;i<10;++i) {
        if(!started && values[i]==0 && i!=9)continue;
        char text[40];std::snprintf(text,sizeof(text),started&&i<7?"%02lld":"%lld",(long long)values[i]);
        result+=text;result+=units[i];started=true;
    }
    return result;
}
#include "PurpleDetail.inc"
static float bossRange = 0.5f;
static int seed = 0, lastReplaySeed = 0;
static eagler::thprac::PracticeInput input;
static bool PhysicalKey(SDL_Scancode key,u16 logical=0) {
    return SDL_GetKeyboardState(nullptr)[key] || (logical && (EaglerOptions::BrowserKeyboardBits() & logical));
}
static eagler::thprac::PracticeKeyMonitor monitor;
// Keep the exact native key geometry; only coordinates/size scale from the
// original 1280x960 draw space to this title's 640x480 ImGui display space.
using namespace eagler::thprac;
#define key_monitor monitor
#include <eagler/thprac/PracticeKeyHud.inc>
#undef key_monitor
static int Locale() { return static_cast<int>(ThpracImGui::GetLocale()); }
static const char* label(const char* const* values) { return values[Locale()]; }
struct PracticeCounter { std::int64_t QuadPart = 0; };
static void practice_counter_frequency(PracticeCounter* value) { value->QuadPart = 1000000000; }
static void practice_counter_now(PracticeCounter* value) { value->QuadPart = SDL_GetTicksNS(); }
static std::function<unsigned()> practice_random_generator(unsigned lo, unsigned hi) {
    // GUI-only entropy. Never touch the title RNG for reaction-test waits.
    return std::bind(std::uniform_int_distribution<unsigned>(lo,hi),std::mt19937(unsigned(SDL_GetTicksNS())));
}
#include <eagler/thprac/PracticeReaction.inc>
#include <eagler/thprac/PracticeSpeed.inc>
double SimulationInterval() {
#ifdef TH_ENABLE_NETPLAY
    return 1.0/60.0;
#else
    if(!Enabled())return 1.0/60.0;
    return speed.interval(g_GameManager.isInReplay,PhysicalKey(SDL_SCANCODE_LCTRL,TH_BUTTON_SKIP)||PhysicalKey(SDL_SCANCODE_RCTRL),
        PhysicalKey(SDL_SCANCODE_LSHIFT,TH_BUTTON_FOCUS)||PhysicalKey(SDL_SCANCODE_RSHIFT),PhysicalKey(SDL_SCANCODE_SPACE));
#endif
}
void FeedUiInput(u16 buttons) {
    // The extracted test uses native VK slots, not SDL scancodes. Bind only
    // while its containing advanced window owns input; ordinary game menus
    // retain their original D-pad-only ImGui navigation contract.
    static bool bound = false;
    auto& io = ImGui::GetIO();
    if (!AdvancedOptionsOpen()) {
        if (bound) for (int key : {37,38,39,40,90,16}) io.KeysDown[key] = false;
        bound = false;
        return;
    }
    bound = true;
    io.KeysDown[37] = (buttons & TH_BUTTON_LEFT) != 0;
    io.KeysDown[38] = (buttons & TH_BUTTON_UP) != 0;
    io.KeysDown[39] = (buttons & TH_BUTTON_RIGHT) != 0;
    io.KeysDown[40] = (buttons & TH_BUTTON_DOWN) != 0;
    io.KeysDown[90] = (buttons & TH_BUTTON_SHOOT) != 0;
    io.KeysDown[16] = (buttons & TH_BUTTON_FOCUS) != 0;
}
static bool Mutable() {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    // Purple trainer mutation has no negotiated multiplayer contract.
    return false;
#else
    return Enabled() && !g_GameManager.isInReplay;
#endif
}
bool EnemyInvincible() { return Mutable() && enemyInvincible; }
bool DisableRankDrop() { return Mutable() && disableRankDrop; }
bool BackgroundFix() { return Enabled() && backgroundFix; }
bool PreserveLifeStock(int lives) {
    return OverlayInfiniteLives() && (!mapInfiniteLives || lives == 0);
}
void OnMiss() { ++misses;booksDied=true;if (Mutable()) input.begin_retry(AdvancedActive() ? 1 : 0); }
void GuiTick() {
    input.gui_tick();
    static std::uint64_t previous=SDL_GetTicksNS(),unsaved=0;
    const auto now=SDL_GetTicksNS();const auto elapsed=now-previous;previous=now;
    // Native th06_update increments this even while the pause UI is open.
    if(booksActive)++bookFrames;
    static bool lastChord=false;
    const bool chord=(PhysicalKey(SDL_SCANCODE_LSHIFT,TH_BUTTON_FOCUS)||PhysicalKey(SDL_SCANCODE_RSHIFT))&&PhysicalKey(SDL_SCANCODE_C);
    if(Enabled() && AdvancedOptionsOpen()) {
        if(chord && !lastChord)disableRankDrop=!disableRankDrop;
        lastChord=chord;
    }
    if(Enabled() && !g_GameManager.isInReplay && g_Supervisor.curState==SUPERVISOR_STATE_GAMEMANAGER &&
        !g_GameManager.isInGameMenu && PurpleSave::Valid(0,g_GameManager.difficulty,g_GameManager.CharacterShotType(),0)) {
        auto& stats=Stats();const int diff=g_GameManager.difficulty,player=g_GameManager.CharacterShotType();
        stats.total.timePlayer[diff][player]+=elapsed;stats.current.timePlayer[diff][player]+=elapsed;
        unsaved+=elapsed;if(unsaved>=180000000000ULL){SaveStatistics();unsaved=0;}
    }
}
void ResetLockTimer() { lockTimer=0; }
void TickLockTimer() { if(Enabled()) ++lockTimer; }
void CaptureBossLife(const Enemy* enemy) {
    bossLife=enemy->life;bossMaxLife=enemy->maxLife;bossThreshold=enemy->lifeCallbackThreshold;
}
float BossThresholdRatio(float healthRatio) {
    if(!Enabled() || !showLifeThreshold || bossMaxLife<=0) return -1;
    const int life=bossThreshold>0?std::min(bossLife,bossThreshold):bossLife;
    return std::min(float(life)/float(bossMaxLife),healthRatio);
}
static bool WallEnabled() {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    return false;
#else
    // Recorded purple section parameters are part of replay simulation, not
    // live trainer toggles. Preserve the same RNG consumption on playback.
    return Enabled() && AdvancedActive() && GetConfig().purpleSections && g_GameManager.currentStage==6 &&
        GetConfig().wall_prac_st6;
#endif
}
void RememberWallBoss(const Enemy* enemy) {
    if(WallEnabled() && GetConfig().section==49) {
        lastBossX=enemy->position.x;lastBossY=enemy->position.y;
    }
}
float AdjustWallAngle(const Enemy* enemy,float original) {
    const int section=GetConfig().section;
    if(!WallEnabled() || (section!=46 && section!=49)) return original;
    return WallAngle(section,enemy->position.x,enemy->position.y,
        g_Player.positionCenter.x,g_Player.positionCenter.y,lastBossX,lastBossY,
        g_BulletManager.bullets,[](){return float(double(g_Rng.GetRandomU32())/4294967296.0);});
}
float AdjustWallSnipe(float x,float y,float original) {
    const auto& config=GetConfig();
    if(!WallEnabled() || config.section!=49 || (config.snipeN<=0 && config.snipeF<=0)) return original;
    return WallSnipe(original,x,y,g_Player.positionCenter.x,g_Player.positionCenter.y,config.snipeN,config.snipeF);
}
void SetBossBounds(Enemy* enemy) {
    if (!Mutable() || !bossMoveDown) return;
    // th06_bossmovedown runs once after ECL MOVEBOUNDSSET, not every frame.
    const float range = enemy->upperMoveLimit.y - enemy->lowerMoveLimit.y;
    enemy->lowerMoveLimit.y = enemy->upperMoveLimit.y - range * (1.0f - bossRange);
}
void PrepareSeed() {
    input.reset();
    ResetLockTimer();
    lastBossX=lastBossY=0;
    bossMaxLife=0;
    booksActive=false;booksDied=false;misses=0;
    if (g_GameManager.isInReplay) { lastReplaySeed = g_Rng.seed; return; }
    const auto& config = GetConfig();
    if (Mutable() && fixedSeed && (g_GameManager.currentStage == 1 || g_GameManager.currentStage == 7) &&
        (!AdvancedActive() || config.section == 10101 || config.section == 0))
        g_Rng.seed = static_cast<u16>(seed & 0xffff);
}
u16 FilterInput(u16 buttons) {
    if (!Mutable() || CapturesGameInput()) return buttons;
    std::uint8_t state[256]{};
    // Native auto-shoot exists only when its global trigger has been configured.
    // No made-up default binding and no additional Launcher settings page.
#ifdef __EMSCRIPTEN__
    const int trigger=EM_ASM_INT({return Number.isInteger(Module.eaglerOptions?.thpracAutoShootKey)?Module.eaglerOptions.thpracAutoShootKey:-1;});
    const int validTrigger=trigger>=0 && trigger<256?trigger:-1;
    if(input.shoot_key_DIK!=validTrigger){input.reset();input.shoot_key_DIK=validTrigger;input.enable_auto_shoot=false;}
#endif
    if(input.shoot_key_DIK>=0 && input.shoot_key_DIK<256) {
        const int key=input.shoot_key_DIK;
        const SDL_Keycode character=key>=65&&key<=90?key+32:key;
        const auto scan=SDL_GetScancodeFromKey(character,nullptr);
        state[key]=scan!=SDL_SCANCODE_UNKNOWN&&PhysicalKey(scan)?0x80:0;
    } else input.enable_auto_shoot=false;
    state[90] = buttons & TH_BUTTON_SHOOT ? 0x80 : 0;
    state[88] = buttons & TH_BUTTON_BOMB ? 0x80 : 0;
    state[160] = state[161] = buttons & TH_BUTTON_FOCUS ? 0x80 : 0;
    state[27] = buttons & TH_BUTTON_MENU ? 0x80 : 0;
    state[82] = buttons & TH_BUTTON_R ? 0x80 : 0;
    state[67]=PhysicalKey(SDL_SCANCODE_C)?0x80:0;
    state[68]=PhysicalKey(SDL_SCANCODE_D)?0x80:0;
    state[81]=PhysicalKey(SDL_SCANCODE_Q,TH_BUTTON_Q)?0x80:0;
    input.apply(state);
    return (buttons & ~u16(7 | TH_BUTTON_MENU | TH_BUTTON_R)) | (state[90] ? TH_BUTTON_SHOOT : 0) |
        (state[88] ? TH_BUTTON_BOMB : 0) | ((state[160] || state[161]) ? TH_BUTTON_FOCUS : 0) |
        (state[27] ? TH_BUTTON_MENU : 0) | (state[82] ? TH_BUTTON_R : 0);
}
void RecordPlayerInput() {
    if (Enabled() && showKeys) monitor.record(6, g_CurFrameInput);
}
void DrawAdvanced() {
    const int locale = Locale();
    if(ImGui::CollapsingHeader(purple_TH_GAME_SPEED[locale]))GameFPSOpt(speed,g_GameManager.isInReplay);
    ImGui::Checkbox(purple_TH06_RANKLOCK_DOWN[locale], &disableRankDrop);
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", purple_TH06_RANKLOCK_DOWN_DESC[locale]);
    // This group is the native DisableKeyOpt subset represented by TH06's
    // logical input mask. C is not a TH06 gameplay input, but retains its
    // source-owned linked setting for compatibility with the shared library.
    ImGui::Checkbox(purple_TH_ADV_DISABLE_X_KEY[locale], &input.disable_xkey);
    ImGui::Checkbox(purple_TH_ADV_DISABLE_C_KEY_SAMETIME[locale], &input.disable_Ckey_at_same_time);
    ImGui::Checkbox(purple_TH_ADV_DISABLE_SHIFT_KEY[locale], &input.disable_shiftkey);
    ImGui::Checkbox(purple_TH_ADV_FORCE_SHIFT_KEY[locale], &input.force_shiftkey);
    ImGui::Checkbox(purple_TH_ADV_DISABLE_Z_KEY[locale], &input.disable_zkey);
    ImGui::Checkbox(purple_THPRAC_FAST_RETRY[locale], &input.enable_fast_retry);
    if(input.shoot_key_DIK>=0){ImGui::SameLine();ImGui::Checkbox(purple_THPRAC_AUTO_SHOOT[locale],&input.enable_auto_shoot);}
    ImGui::Checkbox(purple_THPRAC_INFLIVES_MAP[locale], &mapInfiniteLives);
    ImGui::Checkbox(purple_TH_BOSS_FORCE_MOVE_DOWN[locale], &bossMoveDown);
    ImGui::SetNextItemWidth(180);
    if (ImGui::DragFloat(purple_TH_BOSS_FORCE_MOVE_DOWN_RANGE[locale], &bossRange, 0.002f, 0, 1))
        bossRange = std::clamp(bossRange, 0.0f, 1.0f);
    ImGui::Checkbox(purple_THPRAC_INGAMEINFO_TH06_SHOW_RANK[locale], &showRank);
    ImGui::Checkbox(purple_THPRAC_SHOW_BULLET_HITBOX[locale], &showBulletHitbox);
    ImGui::Checkbox(purple_THPRAC_INGAMEINFO_TH06_SHOW_HITBOX[locale],&showPlayerHitbox);
    ImGui::SameLine();
    if(ImGui::Button(purple_THPRAC_INGAMEINFO_TH06_SHOW_HITBOX_RELOAD[locale]))ReleaseImage(hitboxTexture,hitboxFailed);
    ImGui::Checkbox(purple_TH_ENABLE_LOCK_TIMER[locale], &showLockTimer);
    ImGui::Checkbox(purple_THPRAC_INGAMEINFO_TH06_SHOW_THRESHOLD[locale], &showLifeThreshold);
    if(ImGui::Checkbox(purple_THPRAC_INGAMEINFO_TH06_SHOW_POINT[locale],&showGrey))grey=greyInBomb=0;
    ImGui::Checkbox(purple_THPRAC_TH06_SHOW_REP_MARKER[locale], &showReplayMarker);
    ImGui::Checkbox(purple_THPRAC_TH06_FIX_RAND_SEED[locale], &fixedSeed);
    ImGui::SetNextItemWidth(180);
    if (ImGui::InputInt(purple_THPRAC_TH06_RAND_SEED[locale], &seed)) seed = std::clamp(seed, 0, 65535);
    ImGui::Text("%s: %d", purple_THPRAC_TH06_REP_RAND_SEED[locale], lastReplaySeed);
    ImGui::Checkbox(purple_THPRAC_KB_OPEN[locale], &showKeys);
    if(showKeys) {
    if (ImGui::Button((monitor.g_record_key_aps ? purple_THPRAC_KB_RECORD_STOP : purple_THPRAC_KB_RECORD_START)[locale])) {
        if (!monitor.g_record_key_aps) monitor.clear_record();
        monitor.g_record_key_aps = !monitor.g_record_key_aps;
    }
    if (ImGui::Button(purple_THPRAC_KB_OUTPUT[locale])) {
        const auto csv = monitor.csv();
#ifdef __EMSCRIPTEN__
        EM_ASM({
            const url = URL.createObjectURL(new Blob([UTF8ToString($0)], {type:'text/csv;charset=utf-8'}));
            const a = document.createElement('a');
            a.href = url; a.download = 'APS.csv'; a.click();
            setTimeout(() => URL.revokeObjectURL(url), 1000);
        }, csv.c_str());
#else
        const auto path = FileSystem::GetPrefPath("thprac_keys.csv");
        if (FileSystem::WriteDataToFile(path.c_str(), csv.data(), csv.size()) != 0)
            SDL_Log("Could not export THPrac key recording");
#endif
    }
    const auto& aps = monitor.g_recorded_aps;
    if (aps.size() >= 2) ImGui::PlotLines("##APS", [](void* data, int index) {
        const auto& samples = *static_cast<const std::vector<int>*>(data);
        return float(samples[(samples.size() > 600 ? samples.size()-600 : 0) + index]);
    }, const_cast<std::vector<int>*>(&aps), int(std::min<std::size_t>(600,aps.size())),
       0, nullptr, FLT_MAX, FLT_MAX, ImVec2(0,ImGui::GetFrameHeight()*3));
    }
    ImGui::Checkbox(purple_THPRAC_TH06_BACKGROUND_FIX[locale], &backgroundFix);
    if(ImGui::CollapsingHeader(purple_THPRAC_INGAMEINFO_06_SHOWDETAIL_COLLAPSE[locale]))ShowDetail();
    if(ImGui::CollapsingHeader("Super Secret Settings")) {
        ImGui::Checkbox(purple_THPRAC_BLIND[locale],&blindView);ImGui::SameLine();
        ImGui::TextDisabled("(?)");if(ImGui::IsItemHovered())ImGui::SetTooltip("%s",purple_THPRAC_BLIND_DESC[locale]);
        ImGui::SameLine();ImGui::SetNextItemWidth(75);
        ImGui::DragFloat(purple_THPRAC_BLIND_SZ[locale],&blindSize,1,20,600);
        if(ImGui::Button((std::string(purple_THPRAC_INGAMEINFO_TH06_SHOW_HITBOX_RELOAD[locale])+"##blind_reload").c_str()))ReleaseImage(blindTexture,blindFailed);
    }
    ImGui::Text("%s:(%d,%d),(%d,%d),(%d,%d),(%d,%d),(%d,%d),(%d,%d)",purple_TH_BOOK_LAST[locale],
        int(bookPositions[0][0]-192),int(bookPositions[0][1]),int(bookPositions[1][0]-192),int(bookPositions[1][1]),
        int(bookPositions[2][0]-192),int(bookPositions[2][1]),int(bookPositions[3][0]-192),int(bookPositions[3][1]),
        int(bookPositions[4][0]-192),int(bookPositions[4][1]),int(bookPositions[5][0]-192),int(bookPositions[5][1]));
    if(ImGui::Button(purple_TH_BOOK_COPY_SETTING[locale])) {
        char text[256];std::snprintf(text,sizeof(text),"(%d,%d),(%d,%d),(%d,%d),(%d,%d),(%d,%d),(%d,%d)",
            int(bookPositions[0][0]-192),int(bookPositions[0][1]),int(bookPositions[1][0]-192),int(bookPositions[1][1]),
            int(bookPositions[2][0]-192),int(bookPositions[2][1]),int(bookPositions[3][0]-192),int(bookPositions[3][1]),
            int(bookPositions[4][0]-192),int(bookPositions[4][1]),int(bookPositions[5][0]-192),int(bookPositions[5][1]));
        SDL_SetClipboardText(text);
    }
    static THGuiTestReactionTest reaction;
    if (ImGui::CollapsingHeader(purple_THPRAC_TOOLS_REACTION_TEST[locale])) {
        reaction.GuiUpdate(true);
    } else reaction.Reset();
}
void DrawTrackerExtras() {
    if(showGrey)ImGui::Text("%s: %d / %d",purple_THPRAC_INGAMEINFO_TH06_POINT[Locale()],greyInBomb,grey);
    if(showRank)ImGui::Text("%s: %6d.%02d", purple_THPRAC_INGAMEINFO_TH06_RANK[Locale()],
        g_GameManager.rank, g_GameManager.subRank);
    const int id=booksActive?64:g_EnemyManager.spellcardInfo.idx;
    if((booksActive || g_EnemyManager.spellcardInfo.isActive) && PurpleSave::Valid(id,g_GameManager.difficulty,g_GameManager.CharacterShotType(),0)) {
        const int diff=g_GameManager.difficulty,pl=g_GameManager.CharacterShotType();
        ImGui::TextUnformatted(th06_spells_str[Locale()][id]);
        const auto& data=Stats();
        for(const auto* save:{&data.total,&data.current}) {
            const auto& count=save->SC_History[id][diff][pl];
            ImGui::Text("%d/%d(%.1f%%); %d",count[0],count[1],float(count[0])/std::fmax(1.0f,float(count[1]))*100.0f,count[2]);
        }
        if(booksActive && AdvancedActive() && GetConfig().phase!=0)ImGui::Text("%.1f",float(bookFrames)/60.0f);
    }
}
void DrawIndicators() {
    if (!Enabled()) return;
    if(showPlayerHitbox && !hitboxTexture && !hitboxFailed) {
        hitboxTexture=LoadImage("hitbox.png",hitbox_file,sizeof(hitbox_file)-1,hitboxSize);hitboxFailed=!hitboxTexture;
    }
    static int lastFrame=-1;const bool advanced=lastFrame!=g_GameManager.gameFrames;lastFrame=g_GameManager.gameFrames;
    RenderPurplePlayerHitbox(ImGui::GetForegroundDrawList(),showPlayerHitbox,hitboxTexture,hitboxSize,advanced?1.0f:0.0f);
    if(blindView && g_Supervisor.curState==SUPERVISOR_STATE_GAMEMANAGER) {
        if(!blindTexture && !blindFailed) {
            ImVec2 size;blindTexture=LoadImage("blind.png",blind_file,sizeof(blind_file),size);blindFailed=!blindTexture;
        }
        if(blindTexture) {
            auto p=ImGui::GetBackgroundDrawList();p->PushClipRect(ImVec2(32,16),ImVec2(416,464));
            const float sx=blindSize/80.0f*128.0f;
            const ImVec2 p1(g_Player.positionCenter.x+32-sx,g_Player.positionCenter.y+16-sx);
            const ImVec2 p2(g_Player.positionCenter.x+32+sx,g_Player.positionCenter.y+16+sx);
            p->AddImage(blindTexture,p1,p2);
            p->AddRectFilled(ImVec2(-1000,-1000),ImVec2(1000,p1.y),0xff000000);
            p->AddRectFilled(ImVec2(-1000,-1000),ImVec2(p1.x,1000),0xff000000);
            p->AddRectFilled(ImVec2(p2.x,-1000),ImVec2(1000,1000),0xff000000);
            p->AddRectFilled(ImVec2(-1000,p2.y),ImVec2(1000,1000),0xff000000);
            p->PopClipRect();
        }
    }
    RenderPurpleHitbox(ImGui::GetForegroundDrawList(), showBulletHitbox);
    if(showLockTimer && OverlayTimeLock() && g_Supervisor.curState==SUPERVISOR_STATE_GAMEMANAGER) {
        char text[32];std::snprintf(text,sizeof(text),"%.2f",lockTimer/60.0f);
        const auto size=ImGui::CalcTextSize(text);auto draw=ImGui::GetForegroundDrawList();
        draw->AddRectFilled(ImVec2(32,0),ImVec2(110,size.y),0xffffffff);
        draw->AddText(ImVec2(110-size.x,0),0xff000000,text);
    }
    if (showReplayMarker && g_GameManager.isInReplay && g_Supervisor.curState == SUPERVISOR_STATE_GAMEMANAGER) {
        const auto font = ImGui::GetFont();
        const auto size = font->CalcTextSizeA(20,100,100,"ＲＥＰ");
        auto draw = ImGui::GetForegroundDrawList();
        draw->AddRectFilled(ImVec2(416-size.x,464-size.y),ImVec2(416,464),0x77000000);
        draw->AddText(font,20,ImVec2(416-size.x,464-size.y),0xffffffff,"ＲＥＰ");
    }
    if (showKeys && g_Supervisor.curState == SUPERVISOR_STATE_GAMEMANAGER &&
        eagler::thprac::mobile::draw_tool(eagler::thprac::mobile::Keyboard,
        "Keys###th06-purple-keys", ImVec2(416.5f,0), Locale())) {
        KeyRectStyle style; style.size = ImVec2(24,24);
        // Native GameGuiInit defaults, not the generic shared style defaults.
        style.padding=ImVec2(0,0);
        style.border_color_press=style.border_color_release=0xffffffff;
        style.fill_color_press=0xffff4444;style.fill_color_release=0xffffcccc;
        KeysHUD(6, ImVec2(640,0), ImVec2(416.5f,0), style, true, false);
    }
}
}
