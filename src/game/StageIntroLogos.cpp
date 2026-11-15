#include "StageIntroLogos.hpp"
#include <algorithm>
#include <array>
#include <sstream>
#include <utility>
namespace th128::stageintro {
namespace {
struct Pose {
    float x, y, z, sx, sy, rotation;
};
Pose poseFor(const anm::Scene& scene, const anm::Node& node, std::string& error, int depth = 0) {
    const auto& state = node.vm.state();
    Pose pose{state.x + state.engineX + state.offsetX, state.y + state.engineY + state.offsetY,
              state.z + state.engineZ + state.offsetZ, state.sx, state.sy, state.rotation};
    if (!node.parent) return pose;
    if (depth >= 16) {
        if (error.empty()) error = "ANM ancestry exceeds 16 levels";
        return pose;
    }
    const auto parent = scene.nodes().find(node.parent);
    if (parent == scene.nodes().end()) {
        if (error.empty()) error = "ANM parent is missing";
        return pose;
    }
    const auto inherited = poseFor(scene, *parent->second, error, depth + 1);
    pose.x += inherited.x;
    pose.y += inherited.y;
    pose.z += inherited.z;
    pose.sx *= parent->second->vm.state().sx;
    pose.sy *= parent->second->vm.state().sy;
    if (state.mode == 1 || state.mode == 3 || state.mode >= 14) pose.rotation += parent->second->vm.state().rotation;
    return pose;
}
int parseStageId(const std::string& stage) {
    int id = 0;
    if (stage.empty()) return 0;
    for (const char character : stage) {
        if (character < '0' || character > '9') return 0;
        id = id * 10 + (character - '0');
        if (id > 1000) return 0;
    }
    return id;
}
bool isFirstStage(int id) {
    return id == 1 || id == 6 || id == 11;
}
int atlasRowForTrack(int trackFile) {
    switch (trackFile) {
    case 0: return 0;
    case 2: return 1;
    case 1: return 2;
    case 5: return 3;
    case 6: return 4;
    case 10: return 5;
    case 11: return 6;
    default: return -1;
    }
}
const char* expectedLogo(int id) {
    static const char* logos[17] = {"", "logo_a1_1.anm", "logo_a1_2.anm", "logo_a1_3.anm", "logo_a2_2.anm",
                                    "logo_a2_3.anm", "logo_b1_1.anm", "logo_b1_2.anm", "logo_b1_3.anm",
                                    "logo_b2_2.anm", "logo_b2_3.anm", "logo_c1_1.anm", "logo_c1_2.anm",
                                    "logo_c1_3.anm", "logo_c2_2.anm", "logo_c2_3.anm", "logo_ex.anm"};
    return id >= 1 && id <= 16 ? logos[id] : nullptr;
}
}
Host::Host() {
    scene_.runtime.gameRandom32 = [this] {
        if (error_.empty()) error_ = "original logo game RNG callback was invoked without source proof";
        return 0u;
    };
    scene_.runtime.displayRandom32 = [this] {
        if (error_.empty()) error_ = "original logo display RNG callback was invoked without source proof";
        return 0u;
    };
    scene_.runtime.whiteSpriteBank = 11;
    scene_.runtime.whiteSprite = -1;
    scene_.runtime.unsupported = [this](int bank, int script, int opcode, const char* reason) {
        if (!error_.empty()) return;
        std::ostringstream message;
        message << "ANM bank=" << bank << " script=" << script << " opcode=" << opcode << ": " << reason;
        error_ = message.str();
    };
}
Host::~Host() {
    cleanup();
}
bool Host::bindRoot() {
    auto* node = scene_.bind(1, 1, LogoBank, 0, anm::Membership::Primary, 0, 0, {}, false, nullptr, 224, 16, 0,
                             false, 0);
    if (!node && error_.empty()) error_ = "unable to bind logo title script 0";
    return node != nullptr && error_.empty();
}
bool Host::load(const std::string& stage, const std::string& logo, const std::uint8_t* bytes,
                std::size_t length, std::string& error) {
    cleanup();
    stage_ = stage;
    logo_ = logo;
    stageId_ = parseStageId(stage);
    firstStage_ = isFirstStage(stageId_);
    captionTrackFile_ = -1;
    captionRole_ = -1;
    captionRow_ = -1;
    captionScript_ = -1;
    captionActive_ = false;
    if (stageId_ < 1 || stageId_ > 16 || stage != std::to_string(stageId_)) {
        error_ = "stage id must be the actual canonical catalog id 1..16";
        error = error_;
        return false;
    }
    const char* catalogLogo = expectedLogo(stageId_);
    if (!catalogLogo || logo != catalogLogo) {
        error_ = "stage id and logo do not match the fixed catalog mapping";
        error = error_;
        return false;
    }
    if (!bytes || length == 0) {
        error_ = "empty logo resource";
        error = error_;
        return false;
    }
    std::string loadError;
    if (!scene_.registry.loadBank(LogoBank, bytes, length, loadError)) {
        error_ = loadError.empty() ? "unable to load logo ANM bank" : loadError;
        error = error_;
        return false;
    }
    active_ = true;
    loaded_ = true;
    frame_ = 0;
    if (!bindRoot()) {
        active_ = false;
        loaded_ = false;
        scene_.reset();
        scene_.registry.unloadBank(LogoBank);
        error = error_;
        return false;
    }
    capture();
    error = error_;
    return error_.empty();
}
void Host::tick() {
    if (!loaded_) return;
    if (!error_.empty()) return;
    scene_.tickPrimary();
    if (error_.empty()) scene_.tickSecondary();
    scene_.collect();
    ++frame_;
    if (!error_.empty()) {
        active_ = false;
        return;
    }
    if (!scene_.find(1)) active_ = false;
    if (!scene_.find(2)) captionActive_ = false;
}
void Host::restart() {
    if (!scene_.registry.bank(LogoBank)) {
        error_ = "logo bank is not loaded";
        active_ = false;
        return;
    }
    scene_.reset();
    error_.clear();
    frame_ = 0;
    active_ = true;
    captionTrackFile_ = -1;
    captionRole_ = -1;
    captionRow_ = -1;
    captionScript_ = -1;
    captionActive_ = false;
    if (!bindRoot()) {
        active_ = false;
        return;
    }
    capture();
}
void Host::cleanup() {
    scene_.reset();
    scene_.registry.unloadBank(LogoBank);
    draws_.clear();
    active_ = false;
    loaded_ = false;
    frame_ = 0;
    stageId_ = 0;
    firstStage_ = false;
    captionTrackFile_ = -1;
    captionRole_ = -1;
    captionRow_ = -1;
    captionScript_ = -1;
    captionActive_ = false;
    stage_.clear();
    logo_.clear();
    error_.clear();
}
int Host::rootClock() {
    const auto* root = scene_.find(1);
    return root ? root->vm.state().clock : 0;
}
bool Host::beginTrackCaption(int trackFile, int role, std::string& error) {
    error.clear();
    if (!loaded_) {
        if (error_.empty()) error_ = "stage logo resources are not loaded";
        error = error_;
        return false;
    }
    if (!error_.empty()) {
        error = error_;
        return false;
    }
    if (role < 0 || role > 1) {
        error_ = "track caption role must be 0 or 1";
        error = error_;
        return false;
    }
    const int row = atlasRowForTrack(trackFile);
    if (row < 0) {
        error_ = "track file has no original music-name atlas row";
        error = error_;
        return false;
    }
    const auto bank = scene_.registry.bank(LogoBank);
    const int script = 3 + role;
    const int sprite = 3 + row;
    if (!bank || script < 0 || script >= int(bank->scripts.size()) || sprite < 0 ||
        sprite >= int(bank->sprites.size())) {
        error_ = "track caption script/sprite is not present in this stage logo bank";
        error = error_;
        return false;
    }
    scene_.remove(2);
    scene_.collect();
    auto* node = scene_.bind(2, 2, LogoBank, script, anm::Membership::Secondary, 0, 0,
                             [sprite](int) { return sprite; }, false, nullptr, 224, 16, 0, false, 0);
    if (!node) {
        error_ = "unable to bind original track caption script";
        error = error_;
        return false;
    }
    if (!error_.empty()) {
        error = error_;
        return false;
    }
    captionTrackFile_ = trackFile;
    captionRole_ = role;
    captionRow_ = row;
    captionScript_ = script;
    captionActive_ = true;
    return true;
}
bool Host::captionActive() {
    if (!captionActive_) return false;
    if (!scene_.find(2)) captionActive_ = false;
    return captionActive_;
}
CaptionInfo Host::captionInfo() {
    CaptionInfo info;
    info.trackFile = captionTrackFile_;
    info.role = captionRole_;
    info.atlasRow = captionRow_;
    info.script = captionScript_;
    const auto* node = scene_.find(2);
    if (!node) {
        captionActive_ = false;
        return info;
    }
    const auto& state = node->vm.state();
    const auto bank = scene_.registry.bank(state.spriteBank);
    info.active = true;
    info.sprite = state.sprite;
    info.entry = bank && state.sprite >= 0 && state.sprite < int(bank->sprites.size())
                     ? bank->sprites[state.sprite].textureEntry
                     : -1;
    info.clock = state.clock;
    info.x = state.x + state.engineX + state.offsetX;
    info.y = state.y + state.engineY + state.offsetY;
    return info;
}
int Host::scripts() const {
    const auto bank = scene_.registry.bank(LogoBank);
    return bank ? int(bank->scripts.size()) : 0;
}
int Host::sprites() const {
    const auto bank = scene_.registry.bank(LogoBank);
    return bank ? int(bank->sprites.size()) : 0;
}
const std::vector<DrawRecord>& Host::draw() {
    capture();
    return draws_;
}
void Host::capture() {
    draws_.clear();
    std::vector<const anm::Node*> ordered;
    for (const auto membership : {anm::Membership::Primary, anm::Membership::Secondary, anm::Membership::Embedded})
        scene_.appendChain(membership, ordered);
    std::stable_sort(ordered.begin(), ordered.end(), [](const anm::Node* first, const anm::Node* second) {
        return first->vm.state().layer < second->vm.state().layer;
    });
    int order = 0;
    for (const auto* node : ordered) {
        const auto& state = node->vm.state();
        if (!firstStage_ && state.script == 2) continue;
        if (!node->alive || !state.visible || !(state.flags & 2u) || state.deleteRequested) continue;
        if (state.sprite < 0) {
            if (state.spriteBank < 0 && state.geometryCount == 0) continue;
            if (error_.empty()) error_ = "visible negative/solid sprite is not configured for this logo bank";
            return;
        }
        const auto bank = scene_.registry.bank(state.spriteBank);
        if (!bank || state.sprite >= int(bank->sprites.size())) {
            if (error_.empty()) error_ = "ANM draw refers to an unavailable logo sprite";
            return;
        }
        const auto& sprite = bank->sprites[state.sprite];
        const auto pose = poseFor(scene_, *node, error_);
        if (!error_.empty()) return;
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
        draws_.push_back(record);
    }
}
}
