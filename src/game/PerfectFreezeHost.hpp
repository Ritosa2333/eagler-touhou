#pragma once
#include "FreezeRules.hpp"
#include "animation/AnmScene.hpp"
#include <cstdint>
#include <functional>
#include <optional>
#include <span>
#include <string>
#include <vector>

namespace th128::perfect {
inline constexpr int CostRaw = 10000;
inline constexpr int ProtectionTicks = 200;
inline constexpr int DeathbombProtectionTicks = 10;
inline constexpr std::uint32_t XKeyBit = 2;
inline constexpr int NativeEffectBank = 36;
inline constexpr int NativeEffectScript = 0x24;
inline constexpr int NativeEffectLayer = 0x17;
inline constexpr int NativeOverridePhase = 0x78;
inline constexpr int NativeLaserPhase = 0xb4;
inline constexpr int PhaseFreezeProtectionTicks = 10;

enum class Mode { Normal, DeathbombWindow, Unknown };
enum class LaserContext { Empty, Populated, Unknown };

struct Context {
    int perfectCharge = 0;
    float x = 0, y = 0, radius = 0;
    std::uint32_t keys = 0;
    bool xPressed = false;
    bool systemPresent = false;
    int systemMode3c = 0;
    int busy = 0;
    int playerState = 0;
    Mode mode = Mode::Unknown;
    LaserContext lasers = LaserContext::Unknown;
    int laserCount = 0;
};

struct BulletRequest {
    float x = 0, y = 0, radius = 0;
    int protectionTicks = ProtectionTicks;
    bool perfect = true;
    bool overrideImmune = false;
};

struct BulletResult {
    std::uint32_t frozen = 0, cleared = 0, immune = 0;
    bool completed = false;
};

struct DamageRequest { float x = 0, y = 0, radius = 0; };
struct EffectRequest { float x = 0, y = 0; int layer = -1; int script = -1; };

struct Services {
    std::function<bool(const BulletRequest&, BulletResult&, std::string&)> freezeBullets;
    std::function<bool(const DamageRequest&, std::string&)> damage;
    std::function<bool(const EffectRequest&, std::string&)> effect;
    std::function<bool(int, std::string&)> protect;
    std::function<bool(int, std::string&)> freezeLasers;
    std::function<bool(std::string&)> shatterIce;
};

struct Activation {
    int chargeBefore = 0, chargeAfter = 0;
    int protectionTicks = 0;
    BulletResult bullets;
    int lasers = 0;
    bool effect = false;
};

struct EffectSnapshot {
    std::uint32_t id = 0;
    int bank = 0, script = 0, layer = 0;
    anm::State state;
    std::optional<anm::SpriteInfo> sprite;
    float x = 0, y = 0, z = 0;
};

struct Status {
    bool configured = false, active = false, failed = false;
    int charge = 0, protectionTicks = 0, phase = 0;
    std::uint32_t frozen = 0, cleared = 0, immune = 0;
    int lasers = 0;
    bool effect = false;
    std::string lastDenied;
};

class Host {
public:
    ~Host();
    bool configure(const Context& context, Services services, std::string& error);
    bool loadNativeEffect(std::span<const std::uint8_t> animation, std::string& error);
    bool setCharge(int charge, std::string& error);
    bool activate(const Context& input, std::string& error);
    bool reject(std::string reason, std::string& error);
    bool tick(bool paused, std::string& error);
    bool shutdown(std::string& error);
    Status status() const;
    const std::optional<Activation>& activation() const;
    std::vector<EffectSnapshot> effectSnapshots() const;
private:
    Context context_{};
    Services services_;
    anm::Scene scene_;
    int charge_ = 0;
    int protection_ = 0;
    int phase_ = 0;
    bool freezeDone_ = false;
    BulletRequest bulletRequest_;
    bool effectResources_ = false;
    std::uint64_t effectKey_ = 0;
    std::uint32_t effectHandle_ = 0;
    std::optional<Activation> activation_;
    std::string failure_, lastDenied_;
    bool configured_ = false, active_ = false, executing_ = false, xLatched_ = false;
    bool fail(std::string reason, std::string& error);
    bool deny(std::string reason, std::string& error);
    void removeEffect();
    bool callEffect(const EffectRequest& request, std::string& error);
};

}
