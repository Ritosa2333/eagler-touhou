#include "animation/AnmScene.hpp"
#include "engine/Random.hpp"
#include "game/NativeNormalMainPermit.hpp"
#include "game/FreezeRules.hpp"
#include "game/OriginalHelpHost.hpp"
#include "game/OriginalMenuFont.hpp"
#include "game/NativeReplaySlotPreview.hpp"
#include "game/OriginalMenuInputFilter.hpp"
#include "runtime/Memory.hpp"
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace {
using namespace th128::anm;
std::unique_ptr<Scene> titleSceneOwner=std::make_unique<Scene>();
Scene& scene(){return *titleSceneOwner;}
th128::Random gameRandom, displayRandom;
std::string errorText;
int frameNumber = 0;
int selection = 0;
bool titleActive = false;
unsigned ordinaryMainGeneration=0,ordinaryMainSerial=0,ordinaryMainProfileDigest=0;
bool ordinaryMainEntering=false;bool postrunExtraMain=false;
int titlePage = 0;
std::array<int, 9> titleSelection{};
int titleBGMVolume = 100;
int titleSEVolume = 80;
int titleRapid = 0;
th128::pause::OriginalMenuInputFilter titleOptionsInputFilter;
unsigned titleOptionsHeld = 0;
unsigned titleOptionsPressed = 0;
int titleOptionsCloseFrame = 0;
int titleOptionsNextAction = 0;
std::vector<int> titleOptionsSoundRequests;
unsigned titleSoundEpoch = 1;
unsigned titleSoundNextSerial = 0;
unsigned titleSoundWindowSerial = 0;
unsigned titleSoundWindowEpoch = 0;
unsigned titleSoundWindowCount = 0;
std::uint64_t titleSoundBase = 0;
std::uint64_t titleSoundWindowBase = 0;
bool titleSoundSealed = false;
std::string titleSoundWindowText;
bool titleKeyPrepared = false;
std::array<int, 5> titleKeyCommitted{};
std::array<int, 5> titleKeyPending{};
unsigned titleKeyButtons = 0;
int titleKeyCloseFrame = 0;
bool titleMusicPrepared = false;
unsigned titleMusicSeenMask = 0;
int titleMusicAge = 0;
int titleMusicBornRows = 0;
int titleMusicCloseFrame = 0;
bool titleMusicCatalogLoaded = false;
unsigned titleMusicCatalogVariant = 0;
unsigned titleMusicTextEpoch = 0;
std::array<std::string, 10> titleMusicRawTitles;
std::vector<std::string> titleMusicTextRequests;
std::string titleMusicTextReadback;
std::array<std::array<std::string,8>,10> titleMusicComments;
bool titleMusicWarning = false;
int titleMusicPreviewRow = -1;
bool titleReplayDetails = false;
unsigned titleReplaySlot = 0;
unsigned titleExtraClearMask = 0;
struct TitleReplayStage {unsigned available=0,stage=0,score=0,frames=0;};
std::array<TitleReplayStage,3> titleReplayStages;
std::string titleReplayDetailHeading;
unsigned titleMusicBgmSerial = 0;
struct TitleBgmRequest {unsigned epoch,serial;int operation,row;};
std::vector<TitleBgmRequest> titleMusicBgmRequests;
std::string titleMusicBgmReadback;
th128::pause::OriginalHelp titleHelp;
unsigned titleHelpSoundCursor = 0;
int titleSimpleCloseFrame = 0;
unsigned titleMainActionSerial = 0;
int titleMainAction = 0;
std::array<th128::record::NativeReplaySlotPreview,25> titleReplaySlots;
bool titleReplayPrepared = false;
std::vector<std::string> titleOwnedResultRows;
struct TitleResultSpell {std::string text;unsigned colorRef=0x808080u;};
std::array<std::string,350> titleResultScores;
std::array<std::vector<TitleResultSpell>,5> titleResultSpells;
std::array<std::string,3> titleResultTotals;
int titleResultRoute=0,titleResultRank=1,titleResultPage=0,titleResultCloseFrame=0;
unsigned titleResultTextEpoch=0;
std::vector<std::string> titleResultTextRequests;
void appendOriginalResultDraws(int& order);
bool enterOrdinaryMain();

struct TitleRouteHighScore {unsigned raw=100000,continues=0,clearCount=0;};
std::array<std::array<TitleRouteHighScore,5>,7> titleRouteHighScores{};
void queueMusicCommentText();
bool titleReady = false;
bool titleTransitioning = false;
std::string titleError;
int titleReadyFrame = 0;
int titleRemoveFrame = 0;
std::vector<int> titleRemoveOwners;
std::array<int, 2> titleHeaderRemoveFrame{0, 0};
std::array<int, 2> titleHeaderRemoveOwner{0, 0};
std::string titleStateText;

struct DrawRecord {
    std::uint32_t id, parent;
    std::int32_t bank, sprite, entry, layer, mode, anchorX, anchorY, blend;
    std::uint32_t flags, flags2, primary, secondary;
    float width, height, x, y, z, rotation, sx, sy, u, v, uvScaleX, uvScaleY;
    float u0, v0, u1, v1, rx, ry, radius, geometryWidth;
    std::int32_t geometryCount, script, clock, order;
    std::uint32_t reserved[2];
};
static_assert(sizeof(DrawRecord) == 160);
std::vector<DrawRecord> draws;

void fault(const std::string& message) {
    if (errorText.empty()) errorText = message;
}

void enqueueTitleSound(int identifier) {
    if (titleSoundSealed || identifier < 0 || identifier >= 76 || titleOptionsSoundRequests.size() >= 65536) {
        fault("original Title sound journal capacity or identity is unavailable");
        return;
    }
    titleOptionsSoundRequests.push_back(identifier);
}

void configureRuntime() {
    scene().runtime.gameRandom32 = [] { return gameRandom.next32(); };
    scene().runtime.displayRandom32 = [] { return displayRandom.next32(); };
    scene().runtime.nativeDisplayRandomSelector = false;
    scene().runtime.whiteSpriteBank = 11;
    scene().runtime.whiteSprite = -1;
    scene().runtime.unsupported = [](int bank, int script, int opcode, const char* reason) {
        std::ostringstream message;
        message << "ANM bank=" << bank << " script=" << script << " opcode=" << opcode << ": " << reason;
        fault(message.str());
    };
}

struct Pose {
    float x, y, z, sx, sy, rotation;
};

Pose worldPose(const Node& node, int depth = 0) {
    const auto& state = node.vm.state();
    Pose pose{state.x + state.engineX + state.offsetX, state.y + state.engineY + state.offsetY,
              state.z + state.engineZ + state.offsetZ, state.sx, state.sy, state.rotation};
    if (node.parent) {
        if (depth >= 16) {
            fault("ANM ancestry exceeds 16 levels");
            return pose;
        }
        const auto parent = scene().nodes().find(node.parent);
        if (parent == scene().nodes().end()) {
            fault("ANM parent is missing");
            return pose;
        }
        const auto inherited = worldPose(*parent->second, depth + 1);
        pose.x += inherited.x;
        pose.y += inherited.y;
        pose.z += inherited.z;
        pose.sx *= parent->second->vm.state().sx;
        pose.sy *= parent->second->vm.state().sy;
        if (state.mode == 1 || state.mode == 3 || state.mode >= 14)
            pose.rotation += parent->second->vm.state().rotation;
    }
    return pose;
}

void capture() {
    draws.clear();
    std::vector<const Node*> ordered;
    for (const auto membership : {Membership::Primary, Membership::Secondary, Membership::Embedded})
        scene().appendChain(membership, ordered);
    std::stable_sort(ordered.begin(), ordered.end(), [](const Node* first, const Node* second) {
        return first->vm.state().layer < second->vm.state().layer;
    });
    int order = 0;
    for (const auto* node : ordered) {
        const auto& state = node->vm.state();
        if (!node->alive || !state.visible || !(state.flags & 2u) || state.deleteRequested) continue;
        if (state.sprite < 0) {
            if (state.spriteBank < 0 && state.geometryCount == 0) continue;
            fault("Visible negative/solid sprite is not configured for this TH128 bank");
            return;
        }
        const auto bank = scene().registry.bank(state.spriteBank);
        if (!bank || state.sprite >= int(bank->sprites.size())) {
            fault("ANM draw refers to an unavailable sprite");
            return;
        }
        const auto& sprite = bank->sprites[state.sprite];
        const auto pose = worldPose(*node);
        DrawRecord record{};
        record.id = node->id;
        record.parent = node->parent;
        record.bank = state.spriteBank;
        record.sprite = state.sprite;
        record.entry = sprite.textureEntry;
        record.layer = state.layer;
        record.mode = state.mode;
        record.anchorX = state.anchorX;
        record.anchorY = state.anchorY;
        record.blend = state.blend;
        record.flags = state.flags;
        record.flags2 = state.flags2;
        record.primary = state.primary;
        record.secondary = state.secondary;
        record.width = state.spriteWidth < 0 ? sprite.width : state.spriteWidth;
        record.height = state.spriteHeight < 0 ? sprite.height : state.spriteHeight;
        record.x = pose.x;
        record.y = pose.y;
        record.z = pose.z;
        record.rotation = pose.rotation;
        record.sx = pose.sx;
        record.sy = pose.sy;
        record.u = state.u;
        record.v = state.v;
        record.uvScaleX = state.uvScaleX;
        record.uvScaleY = state.uvScaleY;
        record.u0 = sprite.u0;
        record.v0 = sprite.v0;
        record.u1 = sprite.u1;
        record.v1 = sprite.v1;
        record.rx = state.rx;
        record.ry = state.ry;
        record.radius = state.geometryRadius;
        record.geometryWidth = state.geometryWidth;
        record.geometryCount = state.geometryCount;
        record.script = state.script;
        record.clock = state.clock;
        record.order = order++;
        draws.push_back(record);
    }
    if (titleActive && titlePage == 8 && titleHelp.active()) {
        for (const auto& row : titleHelp.draws()) {
            const auto& state = row.state;
            DrawRecord record{};
            record.id = 0x02000000u + row.id;
            record.parent = row.parent ? 0x02000000u + row.parent : 0;
            record.bank = row.bank; record.sprite = state.sprite; record.entry = row.sprite.textureEntry;
            record.layer = 31; record.mode = state.mode; record.anchorX = state.anchorX; record.anchorY = state.anchorY;
            record.blend = state.blend; record.flags = state.flags; record.flags2 = state.flags2;
            record.primary = state.primary; record.secondary = state.secondary;
            record.width = state.spriteWidth < 0 ? row.sprite.width : state.spriteWidth;
            record.height = state.spriteHeight < 0 ? row.sprite.height : state.spriteHeight;
            record.x = row.x; record.y = row.y; record.z = row.z; record.rotation = state.rotation;
            record.sx = state.sx; record.sy = state.sy; record.u = state.u; record.v = state.v;
            record.uvScaleX = state.uvScaleX; record.uvScaleY = state.uvScaleY;
            record.u0 = row.sprite.u0; record.v0 = row.sprite.v0; record.u1 = row.sprite.u1; record.v1 = row.sprite.v1;
            record.script = state.script; record.clock = state.clock; record.order = order++;
            draws.push_back(record);
        }
    }
    if (titleActive && titlePage == 7) appendOriginalResultDraws(order);
    if (titleActive && titlePage == 6) {
        const auto bank = scene().registry.bank(11);
        if (!bank) return;
        std::vector<std::string> lines;
        if(titlePage==6&&titleReplayDetails){
            lines.push_back(titleReplayDetailHeading);
            for(unsigned row=0;row<3;++row){
                const auto& stage=titleReplayStages[row];char buffer[112]{};
                constexpr std::array<const char*,17> names{"","A1-1","A1-2","A1-3","A2-2","A2-3","B1-1","B1-2","B1-3","B2-2","B2-3","C1-1","C1-2","C1-3","C2-2","C2-3"};
                if(stage.available)std::snprintf(buffer,sizeof(buffer),"Stage %-5s   %9u",names[stage.stage],stage.score*10u);
                else std::snprintf(buffer,sizeof(buffer),"Stage %-5s   ---------",names[stage.stage]);
                lines.emplace_back(buffer);
            }
        } else if (titlePage == 6) {
            for (unsigned index=0;index<25;++index) lines.push_back(titleReplaySlots[index].line(index));
            lines.push_back("Z: WATCH REPLAY   D: EXPORT FILE   X: RETURN");
        } else {
            lines = titleOwnedResultRows;
            if (lines.empty()) lines.push_back("NO RECORDED WEB RESULTS");
            lines.push_back("OWNED WEB RECORDS ONLY   X: RETURN");
        }
        for (std::size_t index=0;index<lines.size();++index) {
            const bool detail=titlePage==6&&titleReplayDetails;
            const auto color=titlePage==6&&int(index)==selection+(detail?1:0)?0xffffff00u:detail&&index&& !titleReplayStages[index-1].available?0xff606060u:0xffc0c0c0u;
            const float xpos=detail?(index?224.f:60.f):96.f,ypos=detail?(index?164.f+float(index-1)*18.f:86.f):64.f+float(index)*15.f;
            for (const auto& glyph:th128::pause::originalMenuGlyphs(lines[index],xpos,ypos,color)) {
                if(glyph.sprite>=bank->sprites.size()) {fault("Original main ASCII sprite missing");return;}
                const auto& sprite=bank->sprites[glyph.sprite];DrawRecord record{};
                record.id=0x03000000u+unsigned(order);record.bank=11;record.sprite=int(glyph.sprite);
                record.entry=sprite.textureEntry;record.layer=31;record.mode=1;record.flags=2;
                record.anchorX=1;record.anchorY=1;record.primary=glyph.color;record.secondary=glyph.color;
                record.width=sprite.width;record.height=sprite.height;record.x=glyph.x;record.y=glyph.y;
                record.sx=record.sy=record.uvScaleX=record.uvScaleY=1;
                record.u0=sprite.u0;record.v0=sprite.v0;record.u1=sprite.u1;record.v1=sprite.v1;record.order=order++;
                draws.push_back(record);
            }
        }
    }
}

bool bindRoot(std::uint64_t key, int bank, int script) {
    auto* node = scene().bind(key, std::uint32_t(key), bank, script, Membership::Primary,
                            0, 0, {}, false, nullptr, 0, 0, 23);
    if (!node) fault("Unable to bind ANM root bank=" + std::to_string(bank) + " script=" + std::to_string(script));
    return node && errorText.empty();
}

void signal(std::uint64_t key, int code, bool immediate = false) {
    scene().interrupt(key, code, immediate);
}

Node* titleNode(int owner, int script);

void setSelection(int choice) {
    if (choice < 0 || choice > 6) {
        fault("Invalid native title selection");
        return;
    }
    selection = choice;
    if (titleNode(3, 0)) {
        signal(3, 3, true);
        signal(3, selection + 17);
    } else {
        for (const auto& [identity, node] : scene().nodes()) {
            if (node->owner != 3) continue;
            const int script = node->vm.state().script;
            if (script == selection + 3 || script == selection + 10)
                scene().interruptNode(identity, selection + 17, true);
        }
    }
    for (const auto& [identity, node] : scene().nodes()) {
        if (node->owner != 3) continue;
        const int script = node->vm.state().script;
        for (int item = 0; item < 7; ++item)
            if (script == item + 3 || script == item + 10)
                if (item != selection) scene().interruptNode(identity, item < selection ? 30 : 31, true);
    }
    titleSelection[0] = selection;
}

bool titleScriptExists(int bank, int script) {
    const auto data = scene().registry.bank(bank);
    return data && script >= 0 && std::size_t(script) < data->scripts.size();
}

Node* titleNode(int owner, int script) {
    for (const auto& [identity, node] : scene().nodes())
        if (int(node->owner) == owner && node->alive && node->vm.state().script == script) return node.get();
    return nullptr;
}

bool titleInterrupt(int owner, int script, int code, bool immediate) {
    auto* node = titleNode(owner, script);
    if (!node) return false;
    scene().interruptNode(node->id, code, immediate);
    return true;
}

constexpr std::array<int,250> originalResultSpellRanks{0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,4,4,4,4,4,4,4,4,4,4};
void resetOriginalResultData() {
    for(unsigned route=0;route<7;++route)for(unsigned rank=0;rank<5;++rank)for(unsigned row=0;row<10;++row){
        char line[160]{};std::snprintf(line,sizeof(line),"%2u  --------  %9u0  ----/--/-- --:--  Stage ----  ---%%",row+1,100000-row*10000);
        titleResultScores[(route*5+rank)*10+row]=line;
    }
    for(auto& rows:titleResultSpells)rows.clear();
    std::string unknown;for(int i=0;i<21;++i)unknown+="\xa3\xbf";
    for(unsigned index=0;index<250;++index){char line[128]{};std::snprintf(line,sizeof(line),"No.%3u %s %3d/%4d/%4d",index+1,unknown.c_str(),0,0,0);titleResultSpells[originalResultSpellRanks[index]].push_back({line,0x808080});}
    titleResultTotals={"        0","  0:00:00","        0"};titleResultRoute=0;titleResultRank=1;titleResultPage=0;titleResultCloseFrame=0;titleResultTextRequests.clear();
}
int originalResultEffectiveRank(){return titleResultRoute==6?4:titleResultRank;}
int originalResultPageCount(){return 1+int((titleResultSpells[originalResultEffectiveRank()].size()+9)/10);}
std::uint64_t originalResultWidgetKey(int script){return 0x700u+unsigned(script);}
bool originalResultWidgetsReady(){
    for(const int script:{132+titleResultRoute,141+originalResultEffectiveRank(),146,147,148,149,150,151,152,153})if(!titleNode(int(originalResultWidgetKey(script)),script))return false;
    return true;
}
void appendOriginalResultDraws(int& order){
    const auto bank=scene().registry.bank(11);if(!bank)return;
    const auto emit=[&](const std::string& text,float x,float y,std::uint32_t color){
        for(const auto& glyph:th128::pause::originalMenuGlyphs(text,x,y,color)){
            if(glyph.sprite>=bank->sprites.size()){fault("Original RESULT ASCII glyph missing");return;}
            const auto& sprite=bank->sprites[glyph.sprite];DrawRecord r{};r.id=0x03000000u+unsigned(order);r.bank=11;r.sprite=int(glyph.sprite);r.entry=sprite.textureEntry;r.layer=31;r.mode=1;r.flags=2;r.anchorX=r.anchorY=1;r.primary=r.secondary=glyph.color;r.width=sprite.width;r.height=sprite.height;r.x=glyph.x;r.y=glyph.y;r.sx=r.sy=r.uvScaleX=r.uvScaleY=1;r.u0=sprite.u0;r.v0=sprite.v0;r.u1=sprite.u1;r.v1=sprite.v1;r.order=order++;draws.push_back(r);
        }
    };
    if(titleResultPage==0)for(unsigned row=0;row<10;++row){const unsigned shade=255-row*16;emit(titleResultScores[(titleResultRoute*5+originalResultEffectiveRank())*10+row],48.f,160.f+float(row)*18.f,0xff0000ffu|(shade<<16)|(shade<<8));}
    for(unsigned row=0;row<3;++row)emit(titleResultTotals[row],328.f,378.f+float(row)*18.f,0xffffffffu);
}
void queueOriginalResultSpellText(){
    titleResultTextRequests.clear();if(titleResultPage==0)return;
    const auto& rows=titleResultSpells[originalResultEffectiveRank()];
    for(unsigned row=0;row<10;++row){const int script=25+int(row),owner=0x900+int(row);if(!titleNode(owner,script)&&!bindRoot(owner,6,script))return;const auto* node=titleNode(owner,script);const unsigned index=unsigned(titleResultPage-1)*10+row;const TitleResultSpell item=index<rows.size()?rows[index]:TitleResultSpell{" ",0xffffff};std::string hex;constexpr char digits[]="0123456789abcdef";for(const unsigned char byte:item.text){hex+=digits[byte>>4];hex+=digits[byte&15];}
        std::ostringstream q;q<<"{\"epoch\":"<<titleResultTextEpoch<<",\"serial\":"<<row+1<<",\"frame\":"<<frameNumber<<",\"vm\":"<<node->id<<",\"bank\":6,\"entry\":0,\"sprite\":"<<30+row<<",\"texture\":[512,512],\"format\":5,\"rect\":[0,"<<36+18*row<<",512,17],\"encoding\":\"gbk\",\"textHex\":\""<<hex<<"\",\"requestedFontSize\":17,\"fontSelector\":0,\"foreground\":"<<item.colorRef<<",\"shadow\":0,\"rasterScale\":2,\"copyFilter\":4,\"deviceAccepted\":false}";titleResultTextRequests.push_back(q.str());
    }
}
void selectOriginalResultRouteRank(int route,int rank){
    const int oldRoute=titleResultRoute,oldRank=originalResultEffectiveRank();titleResultRoute=route;titleResultRank=rank;const int newRank=originalResultEffectiveRank();
    if(oldRoute!=route){signal(originalResultWidgetKey(132+oldRoute),1,false);bindRoot(originalResultWidgetKey(132+route),24,132+route);}
    if(oldRank!=newRank){signal(originalResultWidgetKey(141+oldRank),1,false);bindRoot(originalResultWidgetKey(141+newRank),24,141+newRank);}
    if(titleResultPage>=originalResultPageCount())titleResultPage=originalResultPageCount()-1;queueOriginalResultSpellText();
}
void advanceOriginalResultInput(unsigned keys,unsigned pressed){
    int route=titleResultRoute,rank=titleResultRank;
    if(route!=6){if(keys&0x10u){rank=(rank+3)%4;signal(originalResultWidgetKey(151),2,false);}if(keys&0x20u){rank=(rank+1)%4;signal(originalResultWidgetKey(152),2,false);}}
    if(keys&0x40u){route=(route+6)%7;signal(originalResultWidgetKey(149),2,false);}if(keys&0x80u){route=(route+1)%7;signal(originalResultWidgetKey(150),2,false);}
    if(route!=titleResultRoute||rank!=titleResultRank){selectOriginalResultRouteRank(route,rank);enqueueTitleSound(10);}
    if(pressed&0x102u){enqueueTitleSound(9);for(int script=132;script<=153;++script)signal(originalResultWidgetKey(script),1,false);for(int row=0;row<10;++row)signal(0x900+row,1,false);titleResultCloseFrame=frameNumber+6;titleReady=false;titleTransitioning=true;}
    else if(pressed&0x80201u){titleResultPage=(titleResultPage+1)%originalResultPageCount();if(titleResultPage==0){for(int row=0;row<10;++row)signal(0x900+row,1,false);titleResultTextRequests.clear();}else queueOriginalResultSpellText();enqueueTitleSound(7);}
}

int titleChoiceCount(int page) {
    return page == 0 ? 7 : page == 1 ? 4 : page == 2 ? 4 : page == 3 ? 5 : page == 4 ? 7 : page == 5 ? 10 : page == 6 ? (titleReplayDetails?3:25) : page == 7 ? 1 : page == 8 ? 9 : -1;
}

int selectDifficulty(int choice) {
    if (choice < 0 || choice > 3) {
        titleError = "difficulty choice must be 0..3";
        return 0;
    }
    auto* root = titleNode(10, 129);
    if (!root) {
        titleError = "difficulty root script129 is not live";
        return 0;
    }
    for (int index = 0; index < 4; ++index)
        if (!titleNode(10, 116 + index)) {
            titleError = "difficulty card 116..119 is not ready";
            return 0;
        }
    // The VM keeps one pendingInterrupt_. Send the real root IRQ first so every
    // child receives 7+choice, then put 3 on the non-selected cards last; those
    // cards must not have their hide overwritten by root propagation.
    scene().interruptNode(root->id, 7 + choice, false);
    for (int index = 0; index < 4; ++index)
        if (index != choice && !titleInterrupt(10, 116 + index, 3, false)) {
            titleError = "difficulty card 116..119 is not ready";
            return 0;
        }
    selection = choice;
    titleSelection[1] = choice;
    titleError.clear();
    return 1;
}

bool routeInformationReady() {
    if (!titleNode(0x610, 210)) return false;
    for (int script=176;script<=209;++script)
        if (!titleNode(0x610, script)) return false;
    return true;
}

bool updateRouteInformation(int choice) {
    const auto bank=scene().registry.bank(24);
    if (!bank || bank->sprites.size()<=167 || !routeInformationReady()) {
        titleError="original route information210/176..209 or stageft atlas is unavailable";
        return false;
    }
    const auto visible=[&](int script,bool show) {
        auto& state=titleNode(0x610,script)->vm.control();
        state.visible=true;
        if(show)state.flags|=2u;else state.flags&=~2u;
    };
    const bool locked=choice==3&&titleExtraClearMask!=63;
    for(int script=176;script<=209;++script)visible(script,!locked);
    if(locked)return true;
    const bool extra=choice==3;
    const int rank=extra?4:titleSelection[1],first=choice*2;
    visible(176,!extra);
    for(int index=0;index<4;++index)visible(177+index,!extra&&index==rank);
    for(int index=0;index<7;++index)visible(181+index,index==first||(!extra&&index==first+1));
    const auto digits=[&](int script,const TitleRouteHighScore& score) {
        bool leading=false;unsigned divisor=10000000;
        for(int place=0;place<9;++place) {
            const unsigned digit=place==8?score.continues:(score.raw/divisor)%10u;
            const bool show=place==8||leading||score.raw>=divisor;
            if(place<8){leading=show;divisor/=10;}
            auto& state=titleNode(0x610,script+place)->vm.control();
            state.spriteBank=24;state.sprite=153+int(digit);
            state.spriteWidth=bank->sprites[state.sprite].width;
            state.spriteHeight=bank->sprites[state.sprite].height;
            visible(script+place,show);
        }
    };
    const auto& top=titleRouteHighScores[first][rank];
    visible(189,top.clearCount!=0);digits(192,top);
    if(extra){for(int script=190;script<=191;++script)visible(script,false);for(int script=201;script<=209;++script)visible(script,false);}
    else {const auto& bottom=titleRouteHighScores[first+1][rank];visible(191,bottom.clearCount!=0);digits(201,bottom);}
    return true;
}

int selectRoute(int choice) {
    if (choice < 0 || choice > 3) {
        titleError = "route choice must be 0..3";
        return 0;
    }
    auto* root = titleNode(11, 130);
    if (!root) {
        titleError = "map root script130 is not live";
        return 0;
    }
    for (int index = 0; index < 5; ++index)
        if (!titleNode(11, 124 + index)) {
            titleError = "route card 124..128 is not ready";
            return 0;
        }
    if (!updateRouteInformation(choice)) return 0;
    const int selectedScript = choice == 3 ? (titleExtraClearMask==63?127:128) : 124 + choice;
    // Root first, then hide non-selected cards; VM pendingInterrupt_ is single.
    scene().interruptNode(root->id, 7 + choice, false);
    for (int index = 0; index < 5; ++index)
        if (124 + index != selectedScript && !titleInterrupt(11, 124 + index, 3, false)) {
            titleError = "route card 124..128 is not ready";
            return 0;
        }
    selection = choice;
    titleSelection[2] = choice;
    titleError.clear();
    return 1;
}

bool optionsSubtreeReady() {
    if (!titleNode(14, 1)) return false;
    for (int script = 17; script <= 42; ++script)
        if (!titleNode(14, script)) return false;
    return true;
}

void updateOptionsDigits() {
    const auto bank = scene().registry.bank(24);
    for (int row = 0; row < 2; ++row) {
        const int volume = row == 0 ? titleBGMVolume : titleSEVolume;
        const std::array<int, 3> digits{volume / 100, (volume / 10) % 10, volume % 10};
        for (int style = 0; style < 2; ++style) {
            for (int place = 0; place < 3; ++place) {
                auto& state = titleNode(14, 27 + row * 8 + style * 4 + place)->vm.control();
                state.spriteBank = 24;
                state.sprite = (style == 0 ? 42 : 53) + digits[place];
                state.spriteWidth = bank->sprites[state.sprite].width;
                state.spriteHeight = bank->sprites[state.sprite].height;
                state.visible = true;
                state.flags |= 1u;
                if ((place == 0 && volume < 100) || (place == 1 && volume < 10)) state.flags &= ~2u;
                else state.flags |= 2u;
            }
        }
    }
}

int selectOptions(int choice, bool entering = false) {
    if (choice < 0 || choice > 4 || !optionsSubtreeReady()) {
        titleError = "original Options script1 subtree or choice is unavailable";
        return 0;
    }
    auto* root = titleNode(14, 1);
    scene().interruptNode(root->id, 3, true);
    scene().interruptNode(root->id, (entering ? 17 : 7) + choice, false);
    for (int row = 0; row < 5; ++row) {
        if (row == choice) continue;
        const int code = row < choice ? 30 : 31;
        titleInterrupt(14, 17 + row, code, true);
        titleInterrupt(14, 22 + row, code, true);
        if (row < 2)
            for (int digit = 0; digit < 8; ++digit)
                titleInterrupt(14, 27 + row * 8 + digit, code, true);
    }
    selection = choice;
    titleSelection[3] = choice;
    titleError.clear();
    return 1;
}

void beginOptionsClose(int action) {
    if (action == 4 && !titleKeyPrepared) {
        titleError = "original KeyConfig requires its prepared committed controller mappings";
        return;
    }
    signal(14, 6, false);
    titleOptionsCloseFrame = frameNumber + 10;
    titleOptionsNextAction = action;
    titleTransitioning = true;
    titleReady = false;
    enqueueTitleSound(action == 1 ? 9 : 7);
}

void advanceOptionsInput() {
    const auto repeated = titleOptionsInputFilter.advance(titleOptionsHeld);
    const auto pressed = titleOptionsPressed;
    titleOptionsPressed = 0;
    if (!titleReady || titleTransitioning || titleOptionsCloseFrame) return;
    const auto keys = repeated | pressed;
    int choice = selection;
    if (keys & 0x10u) choice = (choice + 4) % 5;
    if (keys & 0x20u) choice = (choice + 1) % 5;
    if (choice != selection) {
        if (!selectOptions(choice)) return;
        enqueueTitleSound(10);
    }
    if (pressed & 0x102u) {
        if (selection == 4) beginOptionsClose(1);
        else {
            selectOptions(4);
            enqueueTitleSound(9);
        }
        return;
    }
    if (selection < 2) {
        auto& volume = selection == 0 ? titleBGMVolume : titleSEVolume;
        if (keys & 0x40u) volume = std::max(0, volume - 5);
        if (keys & 0x80u) volume = std::min(100, volume + 5);
        if (keys & 0xc0u) updateOptionsDigits();
    }
    if (pressed & 0x80201u) {
        if (selection == 2) beginOptionsClose(4);
        else if (selection == 4) beginOptionsClose(1);
        else if (selection == 3) {
            titleBGMVolume = 100;
            titleSEVolume = 80;
            titleRapid = 0;
            updateOptionsDigits();
            enqueueTitleSound(7);
        }
    }
}

bool keyConfigSubtreeReady() {
    if (!titleNode(15, 2)) return false;
    for (int script = 43; script <= 76; ++script)
        if (!titleNode(15, script)) return false;
    return true;
}

void updateKeyConfigDigits() {
    const auto bank = scene().registry.bank(24);
    for (int row = 0; row < 5; ++row) {
        const std::array<int, 2> digits{titleKeyPending[row] / 10, titleKeyPending[row] % 10};
        for (int style = 0; style < 2; ++style)
            for (int place = 0; place < 2; ++place) {
                auto& state = titleNode(15, 57 + row * 2 + style * 10 + place)->vm.control();
                state.spriteBank = 24;
                state.sprite = 42 + digits[place];
                state.spriteWidth = bank->sprites[state.sprite].width;
                state.spriteHeight = bank->sprites[state.sprite].height;
                state.visible = true;
                state.flags |= 3u;
            }
    }
}

int selectKeyConfig(int choice, bool entering = false) {
    if (choice < 0 || choice > 6 || !keyConfigSubtreeReady()) {
        titleError = "original KeyConfig script2 subtree or choice is unavailable";
        return 0;
    }
    signal(15, 3, true);
    signal(15, (entering ? 17 : 7) + choice, false);
    for (int row = 0; row < 7; ++row) {
        if (row == choice) continue;
        const int code = row < choice ? 30 : 31;
        titleInterrupt(15, 43 + row, code, true);
        titleInterrupt(15, 50 + row, code, true);
        if (row < 5)
            for (int style = 0; style < 2; ++style)
                for (int place = 0; place < 2; ++place)
                    titleInterrupt(15, 57 + row * 2 + style * 10 + place, code, true);
    }
    selection = choice;
    titleSelection[4] = choice;
    titleError.clear();
    return 1;
}

void beginKeyConfigClose() {
    signal(15, 6, false);
    titleKeyCloseFrame = frameNumber + 10;
    titleTransitioning = true;
    titleReady = false;
    enqueueTitleSound(9);
}

void advanceKeyConfigInput() {
    const auto repeated = titleOptionsInputFilter.advance(titleOptionsHeld);
    const auto pressed = titleOptionsPressed;
    titleOptionsPressed = 0;
    if (!titleReady || titleTransitioning || titleKeyCloseFrame) return;
    const auto keys = repeated | pressed;
    int choice = selection;
    if (keys & 0x10u) choice = (choice + 6) % 7;
    if (keys & 0x20u) choice = (choice + 1) % 7;
    if (choice != selection) {
        if (!selectKeyConfig(choice)) return;
        enqueueTitleSound(10);
    }
    if (selection < 5) {
        int button = 0;
        while (button < 31 && !(titleKeyButtons & (1u << button))) ++button;
        if (button < 31 && titleKeyPending[selection] != button) {
            const int previous = titleKeyPending[selection];
            for (int row = 0; row < 5; ++row)
                if (row != selection && titleKeyPending[row] == button) titleKeyPending[row] = previous;
            titleKeyPending[selection] = button;
            updateKeyConfigDigits();
            enqueueTitleSound(7);
        }
    }
    if (selection == 6 && (pressed & 0x102u)) {
        titleKeyPending = titleKeyCommitted;
        updateKeyConfigDigits();
        beginKeyConfigClose();
    } else if (pressed & 0x80201u) {
        if (selection == 6) {
            titleKeyCommitted = titleKeyPending;
            beginKeyConfigClose();
        } else if (selection == 5) {
            titleKeyPending = titleKeyCommitted;
            updateKeyConfigDigits();
            enqueueTitleSound(7);
        }
    }
}

bool musicSubtreeReady() {
    if (!titleNode(16, 110) || titleMusicBornRows != 10) return false;
    for (int row = 0; row < 10; ++row)
        if (!titleNode(160 + row, 154 + row)) return false;
    return true;
}

int selectMusic(int choice) {
    if (choice < 0 || choice > 9 || !musicSubtreeReady()) {
        titleError = "original MusicRoom header110 or row154..163 is unavailable";
        return 0;
    }
    for (int row = 0; row < 10; ++row)
        signal(160 + row, row == choice ? 2 : 3, false);
    selection = choice;
    titleSelection[5] = choice;
    titleMusicWarning = false;
    queueMusicCommentText();
    titleError.clear();
    return 1;
}

std::string musicTextHex(const std::string& text) {
    static constexpr char digits[] = "0123456789abcdef";
    std::string result;
    result.reserve(text.size() * 2);
    for (const unsigned char byte : text) {
        result += digits[byte >> 4];
        result += digits[byte & 15];
    }
    return result;
}

void queueMusicTitleText(int row) {
    const auto* node = titleNode(160 + row, 154 + row);
    if (!titleMusicCatalogLoaded || !node) {
        fault("original MusicRoom text requires its prepared catalog and actual row VM");
        return;
    }
    std::string label = titleMusicRawTitles[row];
    if (!(titleMusicSeenMask & (1u << row))) {
        char unknown[66]{};
        std::snprintf(unknown, sizeof(unknown), titleMusicCatalogVariant == 0 ? "\x4e\x6f\x2e\x25\x32\x64\x20\xa3\xbf\xa3\xbf\xa3\xbf\xa3\xbf\xa3\xbf\xa3\xbf\xa3\xbf\xa3\xbf\xa3\xbf\xa3\xbf\xa3\xbf" : "\x4e\x6f\x2e\x25\x32\x64\x20\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48", row + 1);
        label = unknown;
    }
    std::ostringstream request;
    request << "{\"epoch\":" << titleMusicTextEpoch << ",\"serial\":" << row + 1
            << ",\"frame\":" << frameNumber << ",\"vm\":" << node->id
            << ",\"bank\":24,\"entry\":22,\"sprite\":" << 115 + row
            << ",\"texture\":[512,512],\"format\":5,\"rect\":[0," << row * 18
            << ",512,18],\"encoding\":\"" << (titleMusicCatalogVariant == 0 ? "gbk" : "shift_jis")
            << "\",\"textHex\":\"" << musicTextHex(label)
            << "\",\"requestedFontSize\":17,\"fontSelector\":0,\"foreground\":16777215,\"shadow\":0"
            << ",\"rasterScale\":2,\"copyFilter\":4,\"deviceAccepted\":false}";
    if(titleMusicTextRequests.size()<18)titleMusicTextRequests.resize(18);
    titleMusicTextRequests[row]=request.str();
}

void queueTitleBgm(int operation,int row) {
    if(titleMusicBgmSerial==std::numeric_limits<unsigned>::max()||titleMusicBgmRequests.size()>=65536) {
        fault("Original Title BGM request capacity exhausted");return;
    }
    titleMusicBgmRequests.push_back({titleSoundEpoch,++titleMusicBgmSerial,operation,row});
}

void queueMusicCommentText() {
    if(titlePage!=5||!titleMusicCatalogLoaded)return;
    static const std::array<std::array<std::string,8>,2> warnings{{{"\x20\x20","\x20\x20\x20\x20\xa3\xaa\xa3\xaa\x20\xd1\xa1\x20\xd4\xf1\x20\xb5\xc4\x20\xc7\xfa\x20\xc4\xbf\x20\xc9\xd0\x20\xce\xb4\x20\xd4\xda\x20\xd3\xce\x20\xcf\xb7\x20\xd6\xd0\x20\xb2\xa5\x20\xb7\xc5\x20\xb9\xfd\x20\xa3\xaa\xa3\xaa\x20","\x20","\x20\x20\x20\x20\x20\x20\x20\x20\xc7\xfa\xd7\xd3\xb5\xc4\xbd\xe9\xc9\xdc\xc4\xda\xc8\xdd\xbf\xc9\xc4\xdc\xbb\xe1\xd4\xec\xb3\xc9\xbe\xe7\xcd\xb8\xa1\xa3\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20","\x20\x20\x20\x20\x20\x20\x20\x20\xbc\xb4\xb1\xe3\xc8\xe7\xb4\xcb\xbb\xb9\xca\xc7\xd2\xaa\xb2\xa5\xb7\xc5\xc2\xf0\xa3\xbf\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20","\x20\x20","\x20\x20\x20\x20\x20\x20\xd2\xaa\xb2\xa5\xb7\xc5\xb5\xc4\xbb\xb0\xa3\xac\xc7\xeb\xd4\xd9\xb4\xce\xb0\xb4\xcf\xc2\xbe\xf6\xb6\xa8\xbc\xfc\xa1\xa3\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20","\x20\x20\x20\x20\x20\x20\xb2\xbb\xb2\xa5\xb7\xc5\xb5\xc4\xbb\xb0\xa3\xac\xc7\xeb\xbd\xab\xb9\xe2\xb1\xea\xd1\xa1\xd4\xf1\xb5\xbd\xc6\xe4\xcb\xfc\xc7\xfa\xc4\xbf\xc9\xcf\xbd\xf8\xd0\xd0\xb2\xa5\xb7\xc5\xa1\xa3\x20\x20"},
{"\x81\x40","\x81\x40\x81\x40\x81\x96\x81\x96\x91\x49\x91\xf0\x82\xb5\x82\xbd\x8b\xc8\x82\xcd\x82\xdc\x82\xbe\x83\x51\x81\x5b\x83\x80\x92\x86\x82\xc5\x8d\xc4\x90\xb6\x82\xb3\x82\xea\x82\xc4\x82\xa2\x82\xdc\x82\xb9\x82\xf1\x81\x96\x81\x96","\x20","\x81\x40\x81\x40\x81\x40\x81\x40\x8b\xc8\x82\xcc\x83\x52\x83\x81\x83\x93\x83\x67\x82\xaa\x83\x6c\x83\x5e\x83\x6f\x83\x8c\x82\xc9\x82\xc8\x82\xe9\x8b\xb0\x82\xea\x82\xaa\x82\xa0\x82\xe8\x82\xdc\x82\xb7\x81\x42","\x81\x40\x81\x40\x81\x40\x81\x40\x81\x40\x81\x40\x81\x40\x81\x40\x81\x40\x82\xbb\x82\xea\x82\xc5\x82\xe0\x8d\xc4\x90\xb6\x82\xb5\x82\xdc\x82\xb7\x82\xa9\x81\x48","\x81\x40","\x81\x40\x81\x40\x81\x40\x8d\xc4\x90\xb6\x82\xb5\x82\xbd\x82\xa2\x95\xfb\x82\xcd\x81\x41\x82\xe0\x82\xa4\x88\xea\x93\x78\x8c\x88\x92\xe8\x83\x7b\x83\x5e\x83\x93\x82\xf0\x89\x9f\x82\xb5\x82\xc4\x82\xad\x82\xbe\x82\xb3\x82\xa2\x81\x42","\x81\x40\x81\x40\x81\x40\x8d\xc4\x90\xb6\x82\xb5\x82\xbd\x82\xad\x82\xc8\x82\xa2\x95\xfb\x82\xcd\x81\x41\x83\x4a\x81\x5b\x83\x5c\x83\x8b\x82\xf0\x88\xda\x93\xae\x82\xb5\x82\xc4\x82\xad\x82\xbe\x82\xb3\x82\xa2\x81\x42"}}};
    for(int line=0;line<8;++line) {
        const auto* node=titleNode(200+line,41+line);
        if(!node)continue;
        const auto raw=titleMusicWarning?warnings[titleMusicCatalogVariant][line]:
            ((titleMusicSeenMask&(1u<<selection))||titleMusicPreviewRow==selection)?titleMusicComments[selection][line]:std::string{};
        std::ostringstream request;
        request<<"{\"epoch\":"<<titleMusicTextEpoch<<",\"serial\":"<<titleMusicTextRequests.size()+1
            <<",\"frame\":"<<frameNumber<<",\"vm\":"<<node->id<<",\"bank\":6,\"entry\":0,\"sprite\":"<<28+line
            <<",\"texture\":[512,512],\"format\":5,\"rect\":[0,"<<18*line<<",512,17],\"encoding\":\""
            <<(titleMusicCatalogVariant?"shift_jis":"gbk")<<"\",\"textHex\":\""<<musicTextHex(raw)
            <<"\",\"requestedFontSize\":17,\"fontSelector\":0,\"deviceAccepted\":false}";
        if(titleMusicTextRequests.size()<18)titleMusicTextRequests.resize(18);
        titleMusicTextRequests[10+line]=request.str();
        titleNode(200+line,41+line)->vm.control().primary=titleMusicWarning?0xffff8080u:0xffffffffu;
        signal(200+line,2,false);
    }
}

void advanceMusicRoom() {
    const auto repeated = titleOptionsInputFilter.advance(titleOptionsHeld);
    const auto pressed = titleOptionsPressed;
    titleOptionsPressed = 0;
    if (!titleMusicCloseFrame && titleMusicAge < 10) {
        ++titleMusicAge;
        const int target = std::min(10, titleMusicAge * 2);
        while (titleMusicBornRows < target) {
            const int row = titleMusicBornRows;
            if (!bindRoot(160 + row, 24, 154 + row)) return;
            queueMusicTitleText(row);
            if (!errorText.empty()) return;
            signal(160 + row, row == selection ? 2 : 3, false);
            ++titleMusicBornRows;
        }
    }
    if (!titleReady || titleTransitioning || titleMusicCloseFrame) return;
    const auto keys = repeated | pressed;
    int choice = selection;
    if (keys & 0x10u) choice = (choice + 9) % 10;
    if (keys & 0x20u) choice = (choice + 1) % 10;
    if (choice != selection) {
        if (!selectMusic(choice)) return;
        enqueueTitleSound(10);
    }
    if ((pressed & 0x80201u) && !(pressed & 0x102u)) {
        enqueueTitleSound(7);
        if(!(titleMusicSeenMask&(1u<<selection))&&!titleMusicWarning) {
            titleMusicWarning=true;queueTitleBgm(0,selection);queueMusicCommentText();
        } else {
            titleMusicWarning=false;queueTitleBgm(1,selection);queueMusicCommentText();
        }
    }
    if (pressed & 0x102u) {
        for (int line=0;line<8;++line)signal(200+line,1,false);
        queueTitleBgm(2,0);
        for (int row = 0; row < titleMusicBornRows; ++row) signal(160 + row, 1, false);
        enqueueTitleSound(9);
        titleMusicCloseFrame = frameNumber + 10;
        titleReady = false;
        titleTransitioning = true;
    }
}

bool mainSubtreeReady() {
    for (int script = 3; script <= 16; ++script)
        if (!titleNode(3, script)) return false;
    return true;
}

bool titlePagesReady() {
    if (titlePage == 0) return mainSubtreeReady();
    if (titlePage >= 6) return titleNode(100+titlePage,titlePage==6?108:titlePage==7?109:107) && (titlePage!=8||titleHelp.active()) && (titlePage!=7||originalResultWidgetsReady());
    if (titlePage == 3) return optionsSubtreeReady();
    if (titlePage == 4) return keyConfigSubtreeReady();
    if (titlePage == 5) return musicSubtreeReady();
    if (titlePage == 1) {
        if (!titleNode(10, 129)) return false;
        if (!titleNode(12, 105)) return false;
        for (int script = 116; script <= 119; ++script)
            if (!titleNode(10, script)) return false;
        return true;
    }
    if (!titleNode(11, 130) || !routeInformationReady()) return false;
    if (!titleNode(13, 113)) return false;
    for (int script = 124; script <= 128; ++script)
        if (!titleNode(11, script)) return false;
    return true;
}

std::string titleJsonString(const std::string& text) {
    std::string result;
    for (const char value : text) {
        switch (value) {
        case '\\': result += "\\\\"; break;
        case '"': result += "\\\""; break;
        case '\n': result += "\\n"; break;
        case '\r': result += "\\r"; break;
        case '\t': result += "\\t"; break;
        default: result += value; break;
        }
    }
    return result;
}
}

namespace {
bool enterOrdinaryMain(){
    if(!ordinaryMainEntering)return true;
    // Original return path445f76..445f83 sets60 before its first Main update.
    frameNumber=60;
    if(!scene().find(0x620)&&!bindRoot(0x620,11,24))return false;
    if(!titleNode(1,97)&&!bindRoot(1,24,97))return false;
    if(!titleInterrupt(1,97,3,true))return false;
    if(!titleNode(2,101)&&!titleNode(2,102)&&!bindRoot(2,24,102))return false;
    ordinaryMainEntering=false;return errorText.empty();
}
}

namespace th128::postrun {
bool adoptOrdinaryMain(std::unique_ptr<NativeNormalMainPermit>& permit,std::string& error){
    error.clear();if(!permit||!permit->ready()||titleSoundSealed||titleSoundEpoch==UINT32_MAX
        ||!titleOptionsSoundRequests.empty()||titleSoundWindowSerial||!titleMusicBgmRequests.empty()){
        error="full original Main cannot adopt an unsealed owner or discard pending Title media";return false;}
    const auto ascii=permit->scene_->registry.bank(11);
    if(!permit->scene_->registry.bank(24)||!permit->scene_->registry.bank(25)||!ascii||ascii->scripts.size()<=24||ascii->sprites.size()<=316){
        error="sealed ordinary Main lacks its original four-resource registry";return false;}
    const auto generation=permit->random_.generation,serial=permit->serial_,digest=permit->digest_;postrunExtraMain=permit->extra_;
    const auto retainedGame=permit->random_.game,retainedDisplay=permit->random_.display;
    titleSceneOwner=std::move(permit->scene_);gameRandom=retainedGame;displayRandom=retainedDisplay;
    errorText.clear();draws.clear();frameNumber=0;selection=0;titlePage=0;titleSelection.fill(0);
    titleActive=true;titleReady=false;titleTransitioning=true;titleReadyFrame=0;titleError.clear();
    titleRemoveFrame=0;titleRemoveOwners.clear();titleHeaderRemoveFrame={0,0};titleHeaderRemoveOwner={0,0};
    titleOptionsInputFilter.reset();titleOptionsHeld=titleOptionsPressed=0;titleOptionsCloseFrame=titleOptionsNextAction=0;
    titleKeyCloseFrame=0;titleKeyButtons=0;titleMusicPrepared=false;titleMusicAge=titleMusicBornRows=titleMusicCloseFrame=0;
    titleMusicWarning=false;titleMusicPreviewRow=-1;titleReplayPrepared=false;titleReplayDetails=false;titleReplaySlots={};titleReplayStages={};
    titleSimpleCloseFrame=0;titleMainAction=0;titleMainActionSerial=0;titleResultCloseFrame=0;
    ++titleSoundEpoch;titleSoundNextSerial=titleSoundWindowSerial=titleSoundWindowEpoch=titleSoundWindowCount=0;
    titleSoundBase=titleSoundWindowBase=0;titleSoundWindowText.clear();
    titleMusicSeenMask=0;for(unsigned i=0;i<10;++i)if(permit->profile_[13152+0x26+i])titleMusicSeenMask|=1u<<i;
    ordinaryMainGeneration=generation;ordinaryMainSerial=serial;ordinaryMainProfileDigest=digest;ordinaryMainEntering=true;
    configureRuntime();scene().runtime.nativeDisplayRandomSelector=true;
    permit->consumed_=true;permit.reset();return true;
}
}
extern "C" {
int th128_freeze_can_release(int power, int held, int perfectState) {
    return th128::freeze::canRelease(power, held, perfectState);
}
int th128_freeze_duration(int power) { return th128::freeze::durationCount(power); }
int th128_freeze_add_power(int power, int delta) { return th128::freeze::addPowerRaw(power, delta); }
const char* th128_error() { return errorText.c_str(); }
int th128_frame() { return frameNumber; }
int th128_selection() { return selection; }
int th128_nodes() { return int(scene().nodes().size()); }
std::uint32_t th128_rng_calls() { return gameRandom.calls; }
int th128_draw_stride() { return sizeof(DrawRecord); }
void th128_draw_enabled(unsigned key, int enabled) { scene().drawEnabled(key, enabled != 0); }
const DrawRecord* th128_draw_ptr() { return draws.data(); }
int th128_draw_count() { capture(); return errorText.empty() ? int(draws.size()) : 0; }

void th128_reset() {
    if (titleActive && (!titleOptionsSoundRequests.empty() || titleSoundWindowSerial || !titleMusicBgmRequests.empty())) {
        fault("Title reset requires its real sound requests to be consumed and acknowledged");
        return;
    }
    if (titleSoundSealed || titleSoundEpoch == std::numeric_limits<unsigned>::max()) {
        titleSoundSealed = true;
        fault("Title sound epoch is exhausted");
        return;
    }
    ++titleSoundEpoch;
    titleSoundNextSerial = 0;
    titleSoundWindowSerial = titleSoundWindowEpoch = titleSoundWindowCount = 0;
    titleSoundBase = titleSoundWindowBase = 0;
    titleSoundWindowText.clear();
    scene().reset();
    errorText.clear();
    draws.clear();
    frameNumber = 0;
    selection = 0;
    titleActive = false;
    ordinaryMainGeneration=ordinaryMainSerial=ordinaryMainProfileDigest=0;ordinaryMainEntering=false;postrunExtraMain=false;
    titlePage = 0;
    titleSelection.fill(0);
    titleBGMVolume = 100;
    titleSEVolume = 80;
    titleRapid = 0;
    titleOptionsInputFilter.reset();
    titleOptionsHeld = titleOptionsPressed = 0;
    titleOptionsCloseFrame = titleOptionsNextAction = 0;
    titleOptionsSoundRequests.clear();
    titleKeyPrepared = false;
    titleKeyCommitted.fill(0);
    titleKeyPending.fill(0);
    titleKeyButtons = 0;
    titleKeyCloseFrame = 0;
    titleMusicPrepared = false;
    titleMusicSeenMask = 0;
    titleMusicWarning=false;titleMusicPreviewRow=-1;titleMusicBgmRequests.clear();titleMusicBgmSerial=0;
    titleReplayDetails=false;titleReplayStages={};titleReplayDetailHeading.clear();titleExtraClearMask=0;
    titleHelpSoundCursor=0;titleSimpleCloseFrame=0;titleMainAction=0;titleMainActionSerial=0;resetOriginalResultData();
    titleReplayPrepared=false;titleReplaySlots={};titleOwnedResultRows.clear();titleRouteHighScores={};
    titleMusicAge = titleMusicBornRows = titleMusicCloseFrame = 0;
    titleMusicCatalogLoaded = false;
    titleMusicRawTitles.fill({});
    titleMusicTextRequests.clear();
    titleMusicTextReadback.clear();
    titleReady = false;
    titleTransitioning = false;
    titleError.clear();
    titleReadyFrame = 0;
    titleRemoveFrame = 0;
    titleRemoveOwners.clear();
    titleHeaderRemoveFrame = {0, 0};
    titleHeaderRemoveOwner = {0, 0};
    titleStateText.clear();
    gameRandom = th128::Random{};
    displayRandom = th128::Random{};
    configureRuntime();
}

int th128_load_anm(int bank, const std::uint8_t* data, std::uint32_t length) {
    if (bank < 0 || bank > 255 || !th128::runtime::inputRange(data,length)) {
        fault("Invalid ANM input memory range");
        return 0;
    }
    std::string error;
    if (!scene().registry.loadBank(bank, data, length, error)) {
        fault(error);
        return 0;
    }
    return 1;
}

int th128_boot_title(unsigned seed) {
    th128_reset();
    const auto ascii = scene().registry.bank(11);
    if (!scene().registry.bank(24) || !scene().registry.bank(25) || !ascii
        || ascii->scripts.size() <= 24 || ascii->sprites.size() <= 316) {
        fault("Load original title.anm, title_v.anm and ascii.anm first");
        return 0;
    }
    displayRandom.seed = std::uint16_t(seed);
    titleActive = true;
    scene().runtime.nativeDisplayRandomSelector = true;
    titleReady = false;
    titleTransitioning = true;
    titleReadyFrame = 0;
    // Original main entry445e88..445ea8 retains ASCII S24 for this title
    // lifetime. Its S23 children own the authored cadence, motion and fade.
    return bindRoot(0x620, 11, 24) && bindRoot(1, 24, 97) && bindRoot(2, 24, 101);
}

int th128_bind(int bank, int script, unsigned key) {
    if (!errorText.empty()) return 0;
    return bindRoot(key, bank, script);
}

int th128_title_menu_select(int choice);

int th128_title_menu_page(int page, int choice) {
    titleError.clear();
    if (!titleActive) {
        titleError = "title menu is not booted";
        return 0;
    }
    if (!errorText.empty()) {
        titleError = errorText;
        return 0;
    }
    const int count = titleChoiceCount(page);
    if (count < 0) {
        titleError = "unsupported title menu page (only 0..8)";
        return 0;
    }
    if (choice < 0 || choice >= count) {
        titleError = "title menu choice is outside the page range";
        return 0;
    }
    if (page == titlePage) {
        return th128_title_menu_select(choice);
    }
    if (titleTransitioning || !titleReady) {
        titleError = "title menu transition is still in progress";
        return 0;
    }
    if (page == 0) {
        if (!titleScriptExists(24, 0) || !titleScriptExists(25, 0)) {
            titleError = "main title scripts are not loaded";
            return 0;
        }
        if (!titleScriptExists(24, 97) || !titleScriptExists(24, 101)) {
            titleError = "main title background scripts are not loaded";
            return 0;
        }
    } else if (page >= 6) {
        if(page==7&&(!titleScriptExists(24,153)||(!scene().registry.bank(6)||scene().registry.bank(6)->scripts.size()<=34||scene().registry.bank(6)->sprites.size()<=39)||titleResultTextEpoch==std::numeric_limits<unsigned>::max())){titleError="original RESULT widgets/text bank or epoch unavailable";return 0;}
        if(!titleScriptExists(24,page==6?108:page==7?109:107)||(page==8&&!titleHelp.loaded())) {
            titleError="original main panel header or Help resource is unavailable";return 0;
        }
    } else if (page == 5) {
        if (!titleMusicPrepared || !titleMusicCatalogLoaded || titleMusicTextEpoch == std::numeric_limits<unsigned>::max()
            || !titleScriptExists(24, 110) || !titleScriptExists(24, 163)) {
            titleError = "original MusicRoom requires its header110/rows154..163 and observed owned seen-mask";
            return 0;
        }
    } else if (page == 4) {
        const auto bank = scene().registry.bank(24);
        if (!titleKeyPrepared || !titleScriptExists(24, 2) || !bank || bank->sprites.size() <= 51) {
            titleError = "original KeyConfig requires script2, digit atlas and committed controller mappings";
            return 0;
        }
    } else if (page == 3) {
        const auto bank = scene().registry.bank(24);
        if (!titleScriptExists(24, 1) || !bank || bank->sprites.size() <= 63) {
            titleError = "original Options script1 or digit sprites are not loaded";
            return 0;
        }
    } else {
        if(page==2) {
            const auto bank=scene().registry.bank(24);
            if(!titleScriptExists(24,210)||!bank||bank->sprites.size()<=167) {
                titleError="original map information210 or stageft glyphs are not loaded";return 0;
            }
        }
        const int rootScript = page == 1 ? 129 : 130;
        const int headerScript = page == 1 ? 105 : 113;
        if (!titleScriptExists(24, rootScript)) {
            titleError = page == 1 ? "difficulty root script129 is not loaded"
                                   : "map root script130 is not loaded";
            return 0;
        }
        if (!titleScriptExists(24, headerScript)) {
            titleError = page == 1 ? "difficulty header script105 is not loaded"
                                   : "map header script113 is not loaded";
            return 0;
        }
    }
    const int oldPage = titlePage;
    const int oldSelection = selection;
    const auto oldSelections = titleSelection;
    std::vector<int> oldOwners = oldPage == 0 ? std::vector<int>{3, 4}
        : std::vector<int>{oldPage == 1 ? 10 : oldPage == 2 ? 11 : oldPage == 3 ? 14 : 15};
    if (oldPage == 0 && page == 8) oldOwners = {3};
    if (oldPage == 2) oldOwners.push_back(0x610);
    if(oldPage>=6)oldOwners={100+oldPage};
    if(oldPage==7){for(int script=132;script<=153;++script)oldOwners.push_back(int(originalResultWidgetKey(script)));for(int row=0;row<10;++row)oldOwners.push_back(0x900+row);}
    if (oldPage == 5) {
        oldOwners.clear();
        for(int line=0;line<8;++line)oldOwners.push_back(200+line);
        oldOwners.push_back(16);
        for (int row = 0; row < 10; ++row) oldOwners.push_back(160 + row);
    }
    const bool oldTransitioning = titleTransitioning;
    const bool oldReady = titleReady;
    const int oldReadyFrame = titleReadyFrame;
    const int oldRemoveFrame = titleRemoveFrame;
    const auto oldRemoveOwners = titleRemoveOwners;
    const auto oldHeaderFrames = titleHeaderRemoveFrame;
    const auto oldHeaderOwners = titleHeaderRemoveOwner;
    const std::string previousError = errorText;
    std::vector<int> candidates;
    auto fail = [&](const std::string& cause) {
        for (const int owner : candidates) scene().removeOwner(owner);
        scene().collect();
        selection = oldSelection;
        titleSelection = oldSelections;
        titleTransitioning = oldTransitioning;
        titleReady = oldReady;
        titleReadyFrame = oldReadyFrame;
        titleRemoveFrame = oldRemoveFrame;
        titleRemoveOwners = oldRemoveOwners;
        titleHeaderRemoveFrame = oldHeaderFrames;
        titleHeaderRemoveOwner = oldHeaderOwners;
        errorText = previousError;
        titleError = cause.empty()
            ? "unable to bind the requested original title page" : cause;
        return 0;
    };
    auto bindCandidate = [&](int owner, int bank, int script) {
        candidates.push_back(owner);
        return bindRoot(owner, bank, script);
    };
    bool root1Fresh = false;
    if (page == 0) {
        if (!bindCandidate(3, 24, 0)
            || (!titleNode(4, 0) && !bindCandidate(4, 25, 0)))
            return fail(errorText);
        if (!mainSubtreeReady())
            return fail("main title subtree is incomplete");
        if (!titleNode(1, 97)) {
            if (!bindCandidate(1, 24, 97)) return fail(errorText);
            root1Fresh = true;
        }
        scene().removeOwner(21);
        scene().collect();
        if (!bindCandidate(21, 24, 101)) return fail(errorText);
        if (!titleNode(21, 101))
            return fail("main title background subtree is incomplete");
        if (!bindCandidate(2, 24, 101)) return fail(errorText);
        if (!titleNode(2, 101))
            return fail("main title background subtree is incomplete");
        scene().removeOwner(21);
        scene().collect();
    } else if (page >= 6) {
        if(!bindCandidate(100+page,24,page==6?108:page==7?109:107))return fail(errorText);
        if(page==8) {
            try {titleHelp.open(128.f);titleHelpSoundCursor=0;}catch(const std::exception& exception){return fail(exception.what());}
        } else if(page==7){
            titleResultRoute=0;titleResultRank=1;titleResultPage=0;titleResultCloseFrame=0;titleResultTextRequests.clear();++titleResultTextEpoch;
            for(const int script:{132,142,146,147,148,149,150,151,152,153})if(!bindCandidate(int(originalResultWidgetKey(script)),24,script))return fail(errorText);
        }
        titleSimpleCloseFrame=0;titleOptionsInputFilter.reset();titleOptionsHeld=titleOptionsPressed=0;
    } else if (page == 5) {
        if (!bindCandidate(16, 24, 110)) return fail(errorText);
        titleMusicAge = titleMusicBornRows = titleMusicCloseFrame = 0;
        ++titleMusicTextEpoch;
        titleMusicTextRequests.clear();
        titleMusicTextReadback.clear();
        titleMusicWarning=false;titleMusicPreviewRow=-1;
        for(int line=0;line<8;++line)if(!bindCandidate(200+line,6,41+line))return fail(errorText);
        titleOptionsInputFilter.reset();
        titleOptionsHeld = titleOptionsPressed = 0;
    } else if (page == 4) {
        if (!bindCandidate(15, 24, 2)) return fail(errorText);
        if (!keyConfigSubtreeReady()) return fail("original KeyConfig script2 subtree is incomplete");
        titleKeyPending = titleKeyCommitted;
        updateKeyConfigDigits();
        titleKeyButtons = 0;
        titleKeyCloseFrame = 0;
        titleOptionsInputFilter.reset();
        titleOptionsHeld = titleOptionsPressed = 0;
    } else if (page == 3) {
        if (!bindCandidate(14, 24, 1)) return fail(errorText);
        if (!optionsSubtreeReady()) return fail("original Options script1 subtree is incomplete");
        updateOptionsDigits();
        if (!selectOptions(choice, true)) return fail(titleError);
        titleOptionsInputFilter.reset();
        titleOptionsHeld = titleOptionsPressed = 0;
        titleOptionsCloseFrame = titleOptionsNextAction = 0;
    } else {
        const int owner = page == 1 ? 10 : 11;
        const int rootScript = page == 1 ? 129 : 130;
        const int headerOwner = page == 1 ? 12 : 13;
        const int headerScript = page == 1 ? 105 : 113;
        const int firstCard = page == 1 ? 116 : 124;
        const int cardCount = page == 1 ? 4 : 5;
        scene().removeOwner(20);
        scene().collect();
        if (!bindCandidate(20, 24, headerScript)) return fail(errorText);
        if (!titleNode(20, headerScript))
            return fail("original title header subtree is incomplete");
        if (!bindCandidate(owner, 24, rootScript)) return fail(errorText);
        if (page == 2 && !bindCandidate(0x610, 24, 210)) return fail(errorText);
        bool complete = titleNode(owner, rootScript) != nullptr;
        for (int index = 0; complete && index < cardCount; ++index)
            complete = titleNode(owner, firstCard + index) != nullptr;
        if (!complete)
            return fail("original title page subtree is incomplete");
        const int selected = page == 1 ? selectDifficulty(choice) : selectRoute(choice);
        if (!selected) return fail(titleError);
        if (!bindCandidate(headerOwner, 24, headerScript)) return fail(errorText);
        if (!titleNode(headerOwner, headerScript))
            return fail("original title header subtree is incomplete");
        scene().removeOwner(20);
        scene().collect();
    }
    titleSelection[oldPage] = oldSelection;
    titlePage = page;
    selection = choice;
    titleSelection[page] = choice;
    if(page==5)queueMusicCommentText();
    titleTransitioning = true;
    titleReady = false;
    titleReadyFrame = frameNumber + (page == 0 ? 11 : page == 8 ? 20 : page == 5 ? 10 : page >= 3 ? 7 : 20);
    titleRemoveFrame = frameNumber + 10;
    titleRemoveOwners = oldOwners;
    for (const int owner : oldOwners) signal(owner, 1, true);
    if (oldPage == 0 && page != 0) {
        if (page != 8) signal(1, 3, false);
        signal(2, 1, false);
    } else if (oldPage != 0 && page == 0 && !root1Fresh) {
        signal(1, 2, false);
    }
    if (page == 1) {
        titleHeaderRemoveFrame[0] = 0;
        titleHeaderRemoveOwner[0] = 0;
    } else if (page == 2) {
        titleHeaderRemoveFrame[1] = 0;
        titleHeaderRemoveOwner[1] = 0;
    }
    if (oldPage == 1) {
        signal(12, 1, false);
        titleHeaderRemoveOwner[0] = 12;
        titleHeaderRemoveFrame[0] = frameNumber + 42;
    } else if (oldPage == 2) {
        signal(13, 1, false);
        titleHeaderRemoveOwner[1] = 13;
        titleHeaderRemoveFrame[1] = frameNumber + 42;
    }
    titleError.clear();
    return 1;
}

int th128_title_menu_select(int choice) {
    titleError.clear();
    if (!titleActive) {
        titleError = "title menu is not booted";
        return 0;
    }
    if (!errorText.empty()) {
        titleError = errorText;
        return 0;
    }
    if (!titleReady || titleTransitioning) {
        titleError = "title menu page is not ready";
        return 0;
    }
    const int count = titleChoiceCount(titlePage);
    if (choice < 0 || choice >= count) {
        titleError = "title menu choice is outside the page range";
        return 0;
    }
    if (titlePage == 0) {
        if (!titleNode(3, 0)) {
            titleError = "main title root is not live";
            return 0;
        }
        setSelection(choice);
        return errorText.empty() ? 1 : 0;
    }
    if(titlePage>=6) {if(titlePage==6&&titleReplayDetails&&!titleReplayStages[choice].available)return 0;selection=choice;titleSelection[titlePage]=choice;return 1;}
    if (titlePage == 3) return selectOptions(choice);
    if (titlePage == 4) return selectKeyConfig(choice);
    if (titlePage == 5) return selectMusic(choice);
    return titlePage == 1 ? selectDifficulty(choice) : selectRoute(choice);
}

int th128_title_music_catalog(const std::uint8_t* data, unsigned length, unsigned variant) {
    titleError.clear();
    if (!titleActive || titlePage != 0 || !titleReady || titleTransitioning || !errorText.empty()
        || variant > 1 || !length || length > 1048576 || !th128::runtime::inputRange(data, length)) {
        titleError = "original Music catalog requires its ready main owner, actual bytes and known encoding";
        return 0;
    }
    const std::string raw(reinterpret_cast<const char*>(data), length);
    if (raw.find('\0') != std::string::npos) {
        titleError = "original Music catalog contains an embedded NUL";
        return 0;
    }
    static const std::array<std::string, 10> identities{
        "07", "00", "02", "01", "05", "06", "10", "11", "09", "08"};
    std::array<std::string, 10> titles;
    std::array<std::array<std::string,8>,10> comments{};
    std::array<int,10> commentCount{};
    std::istringstream input(raw);
    std::string line;
    int current = -1;
    while (std::getline(input, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty() || line[0] == '#') continue;
        if (line[0] == '@') {
            if ((current >= 0 && titles[current].empty()) || current == 9) {
                titleError = "original Music catalog has a missing or extra title";
                return 0;
            }
            ++current;
            if (line != "@bgm/th128_" + identities[current] + ".wav") {
                titleError = "original Music catalog path or actual comment order differs";
                return 0;
            }
        } else if (current >= 0 && titles[current].empty()) {
            std::size_t cursor = 3;
            if (line.rfind("No.", 0) != 0 || line.size() > 65) {
                titleError = "original Music title prefix or native66byte budget differs";
                return 0;
            }
            while (cursor < line.size() && (line[cursor] == ' ' || line[cursor] == '\t')) ++cursor;
            int number = 0;
            const auto first = cursor;
            while (cursor < line.size() && line[cursor] >= '0' && line[cursor] <= '9') {
                number = number * 10 + line[cursor] - '0';
                ++cursor;
            }
            if (cursor == first || number != current + 1 || cursor >= line.size() || line[cursor] != ' ') {
                titleError = "original Music title No.N does not match its actual row";
                return 0;
            }
            titles[current] = line;
        } else if(current>=0 && commentCount[current]<8) {
            comments[current][commentCount[current]++]=line;
        }
    }
    if (current != 9 || titles[9].empty()) {
        titleError = "original Music catalog requires all ten original title rows";
        return 0;
    }
    titleMusicRawTitles = std::move(titles);
    titleMusicComments=std::move(comments);
    titleMusicCatalogVariant = variant;
    titleMusicCatalogLoaded = true;
    return 1;
}

unsigned th128_title_music_text_epoch() { return titlePage==7?titleResultTextEpoch:titleMusicTextEpoch; }
unsigned th128_title_music_text_count() { return unsigned(titlePage==7?titleResultTextRequests.size():titleMusicTextRequests.size()); }
const char* th128_title_music_text_request(unsigned index) {
    titleMusicTextReadback.clear();
    if(titlePage==7){if(index<titleResultTextRequests.size())titleMusicTextReadback=titleResultTextRequests[index];return titleMusicTextReadback.c_str();}
    if (titlePage != 5 || index >= titleMusicTextRequests.size()) return titleMusicTextReadback.c_str();
    titleMusicTextReadback = titleMusicTextRequests[index];
    return titleMusicTextReadback.c_str();
}

int th128_title_music_prepare(unsigned seenMask) {
    titleError.clear();
    if (!titleActive || titlePage != 0 || !titleReady || titleTransitioning || !errorText.empty() || seenMask > 1023) {
        titleError = "MusicRoom seen-mask preparation requires its ready main owner and ten observed owned flags";
        return 0;
    }
    titleMusicSeenMask = seenMask;
    titleMusicPrepared = true;
    return 1;
}

int th128_title_music_input(unsigned held, unsigned pressed) {
    titleError.clear();
    if (!titleActive || titlePage != 5 || !errorText.empty() || ((held | pressed) & ~0x803ffu)
        || (pressed & 0x80201u)) {
        titleError = "MusicRoom navigation requires its live owner; playback still requires the actual BGM consumer";
        return 0;
    }
    titleOptionsHeld = held;
    titleOptionsPressed |= pressed;
    return 1;
}

unsigned th128_title_sound_epoch() { return titleSoundSealed ? 0 : titleSoundEpoch; }
unsigned th128_title_sound_count() { return unsigned(titleOptionsSoundRequests.size()); }

int th128_title_sound_begin_window() {
    titleError.clear();
    if (!titleActive || titleSoundSealed || !titleSoundEpoch) {
        titleError = "original Title sound journal owner is unavailable";
        return 0;
    }
    if (titleSoundWindowSerial) return int(titleSoundWindowSerial);
    if (titleOptionsSoundRequests.empty()) return 0;
    if (titleSoundNextSerial >= unsigned(std::numeric_limits<int>::max())) {
        titleSoundSealed = true;
        titleError = "original Title sound window serial is exhausted";
        return 0;
    }
    titleSoundWindowSerial = ++titleSoundNextSerial;
    titleSoundWindowEpoch = titleSoundEpoch;
    titleSoundWindowBase = titleSoundBase;
    titleSoundWindowCount = unsigned(titleOptionsSoundRequests.size());
    return int(titleSoundWindowSerial);
}

const char* th128_title_sound_window_state() {
    std::ostringstream result;
    result << "{\"serial\":" << titleSoundWindowSerial << ",\"epoch\":" << titleSoundWindowEpoch
           << ",\"baseLow\":" << unsigned(titleSoundWindowBase & 0xffffffffu)
           << ",\"baseHigh\":" << unsigned(titleSoundWindowBase >> 32)
           << ",\"count\":" << titleSoundWindowCount << '}';
    titleSoundWindowText = result.str();
    return titleSoundWindowText.c_str();
}

int th128_title_sound_id(unsigned index) {
    return titleSoundWindowSerial && index < titleSoundWindowCount ? titleOptionsSoundRequests[index] : -2;
}
float th128_title_sound_x(unsigned index) {
    return titleSoundWindowSerial && index < titleSoundWindowCount ? 0.f : std::numeric_limits<float>::quiet_NaN();
}
int th128_title_sound_operation(unsigned index) {
    return titleSoundWindowSerial && index < titleSoundWindowCount ? 0 : -1;
}

int th128_title_sound_ack_window(unsigned serial, unsigned count, unsigned baseLow, unsigned baseHigh, unsigned epoch) {
    titleError.clear();
    const std::uint64_t base = (std::uint64_t(baseHigh) << 32) | baseLow;
    if (titleSoundSealed || !epoch || epoch != titleSoundEpoch || epoch != titleSoundWindowEpoch
        || !serial || serial != titleSoundWindowSerial || count != titleSoundWindowCount
        || base != titleSoundBase || base != titleSoundWindowBase || count > titleOptionsSoundRequests.size()) {
        titleError = "Title sound acknowledgement does not own the exact epoch, serial, count and base";
        return -1;
    }
    titleOptionsSoundRequests.erase(titleOptionsSoundRequests.begin(), titleOptionsSoundRequests.begin() + count);
    titleSoundBase += count;
    titleSoundWindowBase = titleSoundBase;
    titleSoundWindowSerial = titleSoundWindowEpoch = titleSoundWindowCount = 0;
    return int(titleOptionsSoundRequests.size());
}

int th128_title_options_input(unsigned held, unsigned pressed) {
    titleError.clear();
    if (!titleActive || titlePage != 3 || !errorText.empty()
        || ((held | pressed) & ~0x803ffu) || titleOptionsNextAction == 4) {
        titleError = "native Options input requires its live owner and original key bits";
        return 0;
    }
    titleOptionsHeld = held;
    titleOptionsPressed |= pressed;
    return 1;
}

int th128_title_keyconfig_prepare(int shot, int bomb, int rapid, int slow, int pause) {
    titleError.clear();
    const std::array<int, 5> values{shot, bomb, rapid, slow, pause};
    if (!titleActive || (titlePage != 0 && titlePage != 3) || !titleReady || titleTransitioning
        || !errorText.empty()) {
        titleError = "controller mapping preparation requires a ready main or Options owner";
        return 0;
    }
    for (int row = 0; row < 5; ++row) {
        if (values[row] < 0 || values[row] > 30) {
            titleError = "original controller button mapping must be 0..30";
            return 0;
        }
        for (int previous = 0; previous < row; ++previous)
            if (values[row] == values[previous]) {
                titleError = "original controller button mappings must be distinct";
                return 0;
            }
    }
    titleKeyCommitted = values;
    titleKeyPending = values;
    titleKeyPrepared = true;
    return 1;
}

int th128_title_keyconfig_input(unsigned held, unsigned pressed, unsigned buttons) {
    titleError.clear();
    if (!titleActive || titlePage != 4 || !errorText.empty() || !keyConfigSubtreeReady()
        || ((held | pressed) & ~0x803ffu)) {
        titleError = "native KeyConfig input requires its original live owner and key bits";
        return 0;
    }
    titleOptionsHeld = held;
    titleOptionsPressed |= pressed;
    titleKeyButtons = buttons;
    return 1;
}

const char* th128_title_menu_state() {
    const bool ready = titleActive && titleReady && !titleTransitioning && errorText.empty()
                       && titlePagesReady();
    std::ostringstream result;
    result << "{\"page\":" << titlePage << ",\"selection\":" << selection
           << ",\"ready\":" << (ready ? 1 : 0) << ",\"supportedPages\":[0,1,2,3,4,5,6,7,8]"
           << ",\"transitioning\":" << (titleTransitioning ? 1 : 0)
           << ",\"bgmVolume\":" << titleBGMVolume << ",\"seVolume\":" << titleSEVolume
           << ",\"rapid\":" << titleRapid << ",\"optionsClosing\":" << (titleOptionsCloseFrame ? 1 : 0)
           << ",\"optionsAction\":" << titleOptionsNextAction
           << ",\"optionsUnconsumedSoundRequests\":" << titleOptionsSoundRequests.size() << ",\"error\":\""
           << titleJsonString(titleError.empty() ? errorText : titleError) << "\"}";
    const auto prefix = result.str();
    result.str({});
    result.clear();
    result << prefix.substr(0, prefix.size() - 1) << ",\"keyCommitted\":[";
    for (int row = 0; row < 5; ++row) {
        if (row) result << ',';
        result << titleKeyCommitted[row];
    }
    result << "],\"keyPending\":[";
    for (int row = 0; row < 5; ++row) {
        if (row) result << ',';
        result << titleKeyPending[row];
    }
    result << "],\"keyPrepared\":" << (titleKeyPrepared ? 1 : 0)
           << ",\"keyClosing\":" << (titleKeyCloseFrame ? 1 : 0)
           << ",\"musicSeenMask\":" << titleMusicSeenMask << ",\"musicAge\":" << titleMusicAge
           << ",\"musicBornRows\":" << titleMusicBornRows << ",\"musicClosing\":" << (titleMusicCloseFrame ? 1 : 0)
           << ",\"musicWarning\":"<<(titleMusicWarning?1:0)
           << ",\"helpPage\":"<<titleHelp.pageIndex()<<",\"helpSelection\":"<<titleHelp.selection()
           << ",\"resultRoute\":"<<titleResultRoute<<",\"resultRank\":"<<originalResultEffectiveRank()<<",\"resultPage\":"<<titleResultPage<<",\"resultPageCount\":"<<originalResultPageCount()<<",\"resultScoreRowCount\":10"
           << ",\"replayDetails\":"<<(titleReplayDetails?1:0)<<",\"replaySlot\":"<<titleReplaySlot<<",\"extraDebugClearMask\":"<<titleExtraClearMask
           << ",\"postrunOwnerKind\":\""<<(postrunExtraMain?"extra":"ordinary")<<"\""
           << ",\"ordinaryReturnGeneration\":"<<ordinaryMainGeneration<<",\"ordinaryReturnSerial\":"<<ordinaryMainSerial
           << ",\"ordinaryProfileDigest\":"<<ordinaryMainProfileDigest<<",\"normalClear\":false"
           << ",\"ordinaryGameSeed\":"<<gameRandom.seed<<",\"ordinaryGameCalls\":"<<gameRandom.calls
           << ",\"ordinaryDisplaySeed\":"<<displayRandom.seed<<",\"ordinaryDisplayCalls\":"<<displayRandom.calls
           << ",\"mainAction\":"<<titleMainAction<<",\"mainActionSerial\":"<<titleMainActionSerial
           << ",\"musicTextRasterAccepted\":false,\"musicPlaybackAccepted\":false}";
    titleStateText = result.str();
    return titleStateText.c_str();
}

void th128_select(int choice) {
    if (titleActive && errorText.empty()) {
        if (titlePage == 0 && frameNumber >= 71) {
            setSelection(choice);
            return;
        }
        th128_title_menu_select(choice);
    }
}

void th128_irq(unsigned key, int code) {
    if (errorText.empty()) signal(key, code);
}

int th128_title_help_load(const void* data,unsigned length) {
    if(!th128::runtime::inputRange(data,length)||titleHelp.active())return 0;
    try {titleHelp.load({static_cast<const std::uint8_t*>(data),length});return 1;}
    catch(const std::exception& exception){titleError=exception.what();return 0;}
}

int th128_title_settings_restore(int bgm,int se,int rapid,const int* mapping,unsigned count) {
    if(!titleActive||titlePage!=0||!titleReady||titleTransitioning||bgm<0||bgm>100||se<0||se>100||rapid<0||rapid>1
       ||count!=5||!th128::runtime::inputRange(mapping,count*sizeof(int)))return 0;
    std::array<int,5> candidate;
    for(unsigned index=0;index<5;++index) {
        candidate[index]=mapping[index];
        if(candidate[index]<0||candidate[index]>30)return 0;
        for(unsigned previous=0;previous<index;++previous)if(candidate[index]==candidate[previous])return 0;
    }
    titleBGMVolume=bgm;titleSEVolume=se;titleRapid=rapid;
    titleKeyCommitted=titleKeyPending=candidate;titleKeyPrepared=true;return 1;
}

int th128_title_replay_prepare(const std::uint8_t* data,unsigned length) {
    if(!titleActive||titlePage!=0||!titleReady||!th128::runtime::inputRange(data,length))return 0;
    try {auto candidate=th128::record::NativeReplaySlotPreview::decode({data,length});titleReplaySlots=candidate;titleReplayPrepared=true;return 1;}
    catch(const std::exception& exception){titleError=exception.what();return 0;}
}

const char* th128_title_music_bgm_request() {
    titleMusicBgmReadback="{}";
    if(!titleMusicBgmRequests.empty()) {
        const auto& request=titleMusicBgmRequests.front();std::ostringstream result;
        result<<"{\"epoch\":"<<request.epoch<<",\"serial\":"<<request.serial<<",\"operation\":"<<request.operation<<",\"row\":"<<request.row<<"}";
        titleMusicBgmReadback=result.str();
    }
    return titleMusicBgmReadback.c_str();
}

int th128_title_music_bgm_ack(unsigned epoch,unsigned serial,int operation,int row) {
    if(titleMusicBgmRequests.empty())return -1;
    const auto& expected=titleMusicBgmRequests.front();
    if(epoch!=expected.epoch||serial!=expected.serial||operation!=expected.operation||row!=expected.row)return -1;
    if(operation==1) {
        titleMusicPreviewRow=row;
        if(titlePage==5) {queueMusicTitleText(row);queueMusicCommentText();}
    }
    titleMusicBgmRequests.erase(titleMusicBgmRequests.begin());return int(titleMusicBgmRequests.size());
}

int th128_title_results_prepare(const std::uint8_t* data,unsigned length) {
    if(!titleActive||titlePage!=0||!titleReady||!th128::runtime::inputRange(data,length)||length<8)return 0;
    const auto bytes=std::span<const std::uint8_t>(data,length);
    const auto word=[&](unsigned offset){return th128::record::NativeReplaySlotPreview::word(bytes,offset);};
    if(word(0)==0x314c5352u){
        if(word(4)!=1||length>100000)return 0;
        try{
            unsigned offset=8;const auto text=[&](bool ascii,unsigned cap){if(offset+4>length)throw std::runtime_error("RESULT text length missing");const unsigned size=word(offset);offset+=4;if(size>cap||offset+size>length)throw std::runtime_error("RESULT text extent rejected");std::string value(reinterpret_cast<const char*>(bytes.data()+offset),size);offset+=size;for(const unsigned char c:value)if(!c||(ascii&&(c<32||c>129)))throw std::runtime_error("RESULT text scalar rejected");return value;};
            std::array<std::string,350> scores;for(auto& row:scores)row=text(true,160);
            std::array<std::vector<TitleResultSpell>,5> spells;
            for(unsigned index=0;index<250;++index){if(offset+4>length)throw std::runtime_error("RESULT spell color missing");const unsigned color=word(offset);offset+=4;if(color!=0x808080u&&color!=0xdfdfdfu&&color!=0xffffa0u&&color!=0xffff80u)throw std::runtime_error("RESULT spell color rejected");auto value=text(false,127);spells[originalResultSpellRanks[index]].push_back({std::move(value),color});}
            std::array<std::string,3> totals;for(auto& row:totals)row=text(true,64);if(offset!=length)return 0;
            titleResultScores=std::move(scores);titleResultSpells=std::move(spells);titleResultTotals=std::move(totals);return 1;
        }catch(const std::exception&){return 0;}
    }
    if(word(0)==0x31485254u) {
        if(word(4)!=35||length!=428)return 0;
        std::array<std::array<TitleRouteHighScore,5>,7> candidate{};
        for(unsigned route=0;route<7;++route)for(unsigned rank=0;rank<5;++rank) {
            const unsigned base=8+(route*5+rank)*12;
            const auto raw=word(base),continues=word(base+4),clearCount=word(base+8);
            if(raw>999999999u||continues>9)return 0;
            candidate[route][rank]={raw,continues,clearCount};
        }
        titleRouteHighScores=candidate;return 1;
    }
    if(word(0)!=0x5352454du||word(4)>25||length!=8+word(4)*16)return 0;
    std::vector<std::string> rows;
    for(unsigned index=0;index<word(4);++index) {
        const unsigned base=8+index*16,stage=word(base),rank=word(base+4),score=word(base+8),complete=word(base+12);
        if(stage<1||stage>23||rank>4||score>999999999||complete>1)return 0;
        char buffer[112]{};
        constexpr std::array<const char*,5> ranks{"EASY","NORMAL","HARD","LUNATIC","EXTRA"};
        std::snprintf(buffer,sizeof(buffer),"STAGE %2u  %-7s %9u  %s",stage,ranks[rank],score,complete?"FINISHED":"SESSION");
        rows.emplace_back(buffer);
    }
    titleOwnedResultRows=std::move(rows);return 1;
}

int th128_title_extra_save(unsigned mask){
    if(!titleActive||titlePage!=0||!titleReady||titleTransitioning||mask>63)return 0;
    titleExtraClearMask=mask;return 1;
}
int th128_title_replay_stage_prepare(const std::uint8_t* data,unsigned length){
    if(!titleActive||titlePage!=6||!titleReady||titleTransitioning||titleReplayDetails
        ||!th128::runtime::inputRange(data,length)||length!=72)return 0;
    const auto bytes=std::span<const std::uint8_t>(data,length);
    const auto word=[&](unsigned offset){return th128::record::NativeReplaySlotPreview::word(bytes,offset);};
    const unsigned slot=word(4),route=word(8),rank=word(12);
    if(word(0)!=0x31545352u||slot>=25||route>5||rank>3||!titleReplaySlots[slot].known)return 0;
    constexpr std::array<std::array<unsigned,3>,6> paths{{{1,2,3},{1,4,5},{6,7,8},{6,9,10},{11,12,13},{11,14,15}}};
    const auto& saved=titleReplaySlots[slot];const auto metadata=std::span<const std::uint8_t>(saved.metadata);
    if(th128::record::NativeReplaySlotPreview::word(metadata,100)!=rank)return 0;
    std::array<TitleReplayStage,3> candidate{};unsigned available=0;
    for(unsigned index=0;index<3;++index){
        const unsigned base=24+index*16;
        candidate[index]={word(base),word(base+4),word(base+8),word(base+12)};
        if(candidate[index].available>1||candidate[index].stage!=paths[route][index]||candidate[index].score>999999999
            ||candidate[index].frames>1000000||(!candidate[index].available&&candidate[index].frames))return 0;
        available+=candidate[index].available;
    }
    if(!available||!candidate[0].available)return 0;
    char name[9]{};for(unsigned index=0;index<8;++index)name[index]=char(saved.metadata[index]);
    constexpr std::array<const char*,4> ranks{"Easy","Normal","Hard","Lunatic"};
    char heading[196]{};
    const auto slowdown=std::bit_cast<float>(th128::record::NativeReplaySlotPreview::word(metadata,84));
    std::snprintf(heading,sizeof(heading),"No.%02u %-8s %02u/%02u/%02u %02u:%02u  %u %s  %4.1f%%",slot+1,name,
        saved.calendar[0]%100,saved.calendar[1],saved.calendar[2],saved.calendar[3],saved.calendar[4],
        th128::record::NativeReplaySlotPreview::word(metadata,20)*10u,ranks[rank],std::isfinite(slowdown)?slowdown:0.f);
    titleReplayStages=candidate;titleReplaySlot=slot;titleReplayDetails=true;titleReplayDetailHeading=heading;selection=0;return 1;
}

int th128_title_main_action_ack(unsigned serial,int action) {
    if(!titleMainAction||serial!=titleMainActionSerial||action!=titleMainAction)return 0;
    titleMainAction=0;return 1;
}

int th128_title_main_input(unsigned held,unsigned pressed,unsigned buttons) {
    if(!titleActive||!errorText.empty()||!titleReady||titleTransitioning
       ||((held|pressed)&~0x803ffu)||(buttons&0x80000000u))return 0;
    titleOptionsHeld=held;titleOptionsPressed|=pressed;titleKeyButtons=buttons;return 1;
}

void advanceMainInput() {
    if(titlePage==3||titlePage==4||titlePage==5)return;
    const auto repeated=titleOptionsInputFilter.advance(titleOptionsHeld),pressed=titleOptionsPressed;
    titleOptionsPressed=0;
    if(titlePage==8&&titleHelp.active()) {
        titleHelp.tick(titleReady&&!titleTransitioning?(repeated|pressed):0);
        while(titleHelpSoundCursor<titleHelp.sounds().size())enqueueTitleSound(titleHelp.sounds()[titleHelpSoundCursor++]);
        selection=titleHelp.selection();
        if(!titleHelp.active()) {titleReady=true;titleTransitioning=false;th128_title_menu_page(0,titleSelection[0]);}
        return;
    }
    if(!titleReady||titleTransitioning||titleMainAction||titleSimpleCloseFrame)return;
    const auto keys=repeated|pressed;
    if(titlePage==7){advanceOriginalResultInput(keys,pressed);return;}
    const int count=titleChoiceCount(titlePage);int choice=selection;
    if(keys&0x50u)choice=(choice+count-1)%count;
    if(keys&0xa0u)choice=(choice+1)%count;
    if(choice!=selection){
        if(titlePage==6&&titleReplayDetails){
            const int delta=(keys&0x50u)?-1:1;
            for(int attempts=0;attempts<3&&!titleReplayStages[choice].available;++attempts)choice=(choice+delta+3)%3;
        }
        if(th128_title_menu_select(choice))enqueueTitleSound(10);
    }
    if(pressed&0x102u) {
        enqueueTitleSound(9);
        if(titlePage==0) {if(selection==6){titleMainAction=2;++titleMainActionSerial;}else setSelection(6);}
        else if(titlePage==1)th128_title_menu_page(0,titleSelection[0]);
        else if(titlePage==2)th128_title_menu_page(1,titleSelection[1]);
        else if(titlePage==6&&titleReplayDetails){titleReplayDetails=false;selection=int(titleReplaySlot);titleSelection[6]=selection;}
        else {signal(100+titlePage,1,false);titleSimpleCloseFrame=frameNumber+10;titleReady=false;titleTransitioning=true;}
    } else if(pressed&0x80201u) {
        enqueueTitleSound(7);
        if(titlePage==0) {
            static constexpr std::array<int,7> targets{1,6,7,5,3,8,-1};
            if(targets[selection]>=0)th128_title_menu_page(targets[selection],0);
            else {titleMainAction=2;++titleMainActionSerial;}
        } else if(titlePage==1)th128_title_menu_page(2,titleSelection[2]);
        else if(titlePage==2) {titleMainAction=1;++titleMainActionSerial;}
        else if(titlePage==6&&titleReplayDetails&&titleReplayStages[selection].available){titleMainAction=4;++titleMainActionSerial;}
        else if(titlePage==6&&titleReplaySlots[selection].occupied) {titleMainAction=3;++titleMainActionSerial;}
    }
}

int th128_tick() {
    if (!errorText.empty()) return 0;
    if(ordinaryMainEntering&&!enterOrdinaryMain())return 0;
    if (titleActive) advanceMainInput();
    if (titleActive && titlePage == 3) advanceOptionsInput();
    if (titleActive && titlePage == 4) advanceKeyConfigInput();
    if (titleActive && titlePage == 5) advanceMusicRoom();
    if (titleActive && titlePage == 0 && frameNumber == 60 && !titleNode(3, 0)) {
        if (!bindRoot(3, 24, 0) || !bindRoot(4, 25, 0)) return 0;
    }
    if (titleActive && titlePage == 0 && frameNumber == 71 && !titleReady) {
        setSelection(selection);
        titleReady = true;
        titleTransitioning = false;
        titleReadyFrame = 0;
        titleError.clear();
    }
    if (titleActive && titleRemoveFrame && frameNumber >= titleRemoveFrame) {
        for (const int owner : titleRemoveOwners) scene().removeOwner(owner);
        titleRemoveOwners.clear();
        titleRemoveFrame = 0;
        scene().collect();
    }
    for (int index = 0; index < 2; ++index) {
        if (titleHeaderRemoveFrame[index] && frameNumber >= titleHeaderRemoveFrame[index]) {
            scene().removeOwner(titleHeaderRemoveOwner[index]);
            titleHeaderRemoveOwner[index] = 0;
            titleHeaderRemoveFrame[index] = 0;
            scene().collect();
        }
    }
    scene().tickPrimary();
    if (!errorText.empty()) return 0;
    scene().tickSecondary();
    scene().collect();
    if (scene().nodes().size() > 16384) fault("ANM live-node budget exceeded");
    ++frameNumber;
    if(titleActive&&titleSimpleCloseFrame&&frameNumber>=titleSimpleCloseFrame) {
        titleSimpleCloseFrame=0;titleReady=true;titleTransitioning=false;titleOptionsHeld=titleOptionsPressed=0;
        if(!th128_title_menu_page(0,titleSelection[0]))return 0;
    }
    if (titleActive && titlePage == 3 && titleOptionsCloseFrame && frameNumber >= titleOptionsCloseFrame) {
        const int action = titleOptionsNextAction;
        titleOptionsCloseFrame = 0;
        titleOptionsHeld = titleOptionsPressed = 0;
        titleOptionsInputFilter.reset();
        scene().removeOwner(14);
        scene().collect();
        if (action == 1) {
            titleOptionsNextAction = 0;
            titleTransitioning = false;
            titleReady = true;
            if (!th128_title_menu_page(0, titleSelection[0])) return 0;
        } else {
            titleOptionsNextAction = 0;
            titleTransitioning = false;
            titleReady = true;
            if (!th128_title_menu_page(4, 0)) return 0;
        }
    }
    if (titleActive && titlePage == 4 && titleKeyCloseFrame && frameNumber >= titleKeyCloseFrame) {
        titleKeyCloseFrame = 0;
        titleKeyButtons = 0;
        titleOptionsHeld = titleOptionsPressed = 0;
        titleOptionsInputFilter.reset();
        scene().removeOwner(15);
        scene().collect();
        titleTransitioning = false;
        titleReady = true;
        if (!th128_title_menu_page(3, titleSelection[3])) return 0;
    }
    if (titleActive && titlePage == 5 && titleMusicCloseFrame && frameNumber >= titleMusicCloseFrame) {
        titleMusicCloseFrame = 0;
        titleOptionsHeld = titleOptionsPressed = 0;
        titleOptionsInputFilter.reset();
        for (int row = 0; row < titleMusicBornRows; ++row) scene().removeOwner(160 + row);
        titleMusicBornRows = 0;
        scene().collect();
        signal(16, 1, false);
        titleTransitioning = false;
        titleReady = true;
        if (!th128_title_menu_page(0, titleSelection[0])) return 0;
    }
    if(titleActive&&titlePage==7&&titleResultCloseFrame&&frameNumber>=titleResultCloseFrame){titleResultCloseFrame=0;titleTransitioning=false;titleReady=true;if(!th128_title_menu_page(0,titleSelection[0]))return 0;}
    if (titleActive && titleTransitioning && !titleReady && titleReadyFrame && frameNumber >= titleReadyFrame) {
        if (titlePage == 0) setSelection(selection);
        if (titlePage == 4 && !selectKeyConfig(selection, true)) return 0;
        titleReady = true;
        titleTransitioning = false;
        titleReadyFrame = 0;
        titleError.clear();
    }
    return errorText.empty() ? 1 : 0;
}
}
