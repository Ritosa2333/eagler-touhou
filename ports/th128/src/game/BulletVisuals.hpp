#pragma once

#include "animation/AnmLogic.hpp"
#include <optional>
#include <functional>

namespace th128::bullet {

class BulletVisuals {
public:
    static constexpr int BankId = 0;
    static constexpr uint32_t MaxAdvanceTicks = 4096;

    struct CreateInfo {
        int type = 8;
        int color = 0;
        int script = 43;
        std::array<int32_t, 4> spriteRemap{-1, -1, -1, -1};
        std::optional<int> secondaryScript;
        bool identitySpriteRemap = false;
        int birthInterrupt = 0;
        bool deferPostBindAdvance = false;
    };

    struct Visual {
        anm::VM primary;
        anm::VM secondary;
        bool hasSecondary = false;

        bool faulted() const {
            return !failure_.empty() || primary.faulted() || (hasSecondary && secondary.faulted());
        }

    private:
        std::array<int32_t, 4> remap_{};
        bool identityRemap_ = false;
        std::weak_ptr<const uint8_t> generation_;
        std::string failure_;
        friend class BulletVisuals;
    };

    using InitialExtensionOwner = std::function<bool(const Visual&, std::string&)>;

    BulletVisuals() = default;
    BulletVisuals(const BulletVisuals&) = delete;
    BulletVisuals& operator=(const BulletVisuals&) = delete;
    BulletVisuals(BulletVisuals&&) = delete;
    BulletVisuals& operator=(BulletVisuals&&) = delete;

    bool load(const void* bytes, size_t size, std::string& error);
    std::optional<Visual> create(const CreateInfo& info, std::string& error);
    std::optional<Visual> createInitialExtension1048576(const CreateInfo& info,
        const InitialExtensionOwner& initialize, std::string& error);
    bool convertType25(Visual& visual,int color,std::string& error);
    bool interrupt(Visual& visual, int code, std::string& error);
    bool advance(Visual& visual, uint32_t ticks, std::string& error);
    void setRandomSources(std::function<uint32_t()> game, std::function<uint32_t()> display);
    const anm::State* animationState(const Visual& visual, bool secondary = false) const;
    const anm::SpriteInfo* spriteInfo(int sprite) const;
    const anm::Registry& registry() const { return registry_; }

private:
    anm::Registry registry_;
    std::shared_ptr<const uint8_t> generation_;
    std::function<uint32_t()> gameRandom32_, displayRandom32_;

    std::optional<Visual> createInternal(const CreateInfo& info,
        const InitialExtensionOwner& initialize, std::string& error);
    bool valid(const Visual& visual, std::string& error) const;
    anm::Runtime runtimeFor(Visual& visual, bool secondary);
    bool checked(const Visual& visual, std::string& error) const;
    bool tick(Visual& visual, bool secondaryUnconditional, std::string& error);
};

}
