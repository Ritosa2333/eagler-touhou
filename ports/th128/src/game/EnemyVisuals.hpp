#pragma once

#include "EnemyRules.hpp"
#include "animation/AnmLogic.hpp"
#include <list>

namespace th128::enemy {

class EnemyVisuals {
public:
    static constexpr int FirstIncludeBank = 1;
    static constexpr int LargeEnemyBank = 2;
    static constexpr int BossBank = 3;
    static constexpr size_t MaxNodes = 4096;
    static constexpr size_t InstructionBudget = 4096;

    struct Pose {
        float x=0,y=0,z=0,sx=1,sy=1,rotation=0,radius=0;
    };

    struct Snapshot {
        uint32_t entity = 0;
        int slot = 0;
        anm::State state;
        std::optional<anm::SpriteInfo> sprite;
        uint32_t node=0,parent=0;
        Pose world;
    };

    enum class BankDomain { Ordinary, ExtraStage16Rank4 };
    explicit EnemyVisuals(BankDomain domain = BankDomain::Ordinary) : bankDomain_(domain) {}
    EnemyVisuals(const EnemyVisuals&) = delete;
    EnemyVisuals& operator=(const EnemyVisuals&) = delete;
    EnemyVisuals(EnemyVisuals&&) = delete;
    EnemyVisuals& operator=(EnemyVisuals&&) = delete;

    bool load(int bank, const void* bytes, size_t size, std::string& error);
    bool create(uint32_t entity, int slot, int bank, int script, int layer,
                VisibilityEffect visibility, std::string& error);
    bool create(uint32_t entity, int slot, int bank, int script, int layer, std::string& error) {
        return create(entity, slot, bank, script, layer, VisibilityEffect::None, error);
    }
    bool position(uint32_t entity, float x, float y, float z);
    bool visibility(uint32_t entity, VisibilityEffect effect);
    bool tickAll(std::string& error);
    std::optional<Bounds> primaryAppearanceBounds(uint32_t entity,int bank,int script,std::string& error);
    std::optional<HitFeedbackPlan> applyHitFeedback(uint32_t entity,const HitFeedbackInput& input,std::string& error);
    bool removeOwner(uint32_t entity);
    bool retireSlot(uint32_t entity,int slot,std::string& error);
    bool setRandomSources(std::function<uint32_t()> game, std::function<uint32_t()> display);
    bool setUpdateControl(anm::UpdateControl control);
    std::vector<Snapshot> snapshots() const;
    std::optional<anm::SpriteInfo> spriteInfo(int bank, int sprite) const;

private:
    struct Node {
        uint32_t entity = 0;
        uint32_t node=0,parent=0;
        int slot = 0, bank = 0;
        anm::VM vm;
        std::string failure;
    };
    struct Owner {
        std::array<float, 3> position{};
        size_t nodes = 0;
    };
    using Nodes = std::list<Node>;
    const BankDomain bankDomain_;
    anm::Registry registry_;
    Nodes nodes_;
    std::unordered_map<uint32_t, Owner> owners_;
    std::function<uint32_t()> gameRandom32_, displayRandom32_;
    std::optional<anm::UpdateControl> updateControl_;
    bool executing_ = false;
    std::string operationFailure_;
    uint32_t nextNode_=1;

    anm::Runtime runtimeFor(Node& node);
    uint32_t random32(Node& node, bool display);
    bool checked(Node& node, std::string& error);
    bool rejectReentry(std::string* error=nullptr);
    bool scriptSupported(int bank, int script, std::string& error) const;
    Nodes::iterator erase(Nodes::iterator node);
    void eraseChildren(uint32_t parent);
    void spawnChild(Node& parent,const anm::ChildRequest& request);
    const Node* findNode(uint32_t identity) const;
    bool worldPose(const Node& node,Pose& pose) const;
    bool validBank(int bank) const;
    static bool validVisibility(VisibilityEffect effect);
    static void applyVisibility(anm::State& state, VisibilityEffect effect);
};

}
