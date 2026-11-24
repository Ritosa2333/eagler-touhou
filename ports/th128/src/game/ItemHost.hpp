#pragma once
#include "animation/AnmLogic.hpp"
#include "ItemRules.hpp"
#include <optional>
#include <span>

namespace th128::item {
struct Snapshot {
    std::uint32_t id=0,slot=0;
    int kind=0,age=0,script=0;
    Position position,velocity;
    anm::State state;
    anm::SpriteInfo sprite;
};
struct IceState {unsigned cursor=0,births=0,pending=0;};
class ItemHost {
public:
    static constexpr std::size_t Capacity=600;
    static constexpr std::size_t IceCapacity=100;
    bool load(std::span<const std::uint8_t> animation,std::string& error);
    bool spawn(int kind,Position position,float angle,float speed,std::string& error);
    bool tick(Position player,int playerState,bool focused,std::string& error);
    using CollectHost=std::function<bool(int,Position,int,std::string&)>;
    bool setCollectHost(CollectHost host);
    bool setMissHost(std::function<bool(std::string&)> host);
    bool enableIceItems(float attractionSpeed,std::string& error);
    IceState iceState() const;
    std::vector<Snapshot> snapshots() const;
private:
    struct Item {
        std::uint32_t id=0;
        int kind=0,age=0;
        int state=1;
        int delay=0;
        float attractionSpeed=0;
        Position position,velocity;
        anm::VM primary,secondary;
        std::string fault;
    };
    anm::Registry registry_;
    std::vector<std::unique_ptr<Item>> slots_=std::vector<std::unique_ptr<Item>>(Capacity+IceCapacity);
    unsigned iceCursor_=0,iceBirths_=0;
    bool iceEnabled_=false;
    float nativeAttractionSpeed_=5;
    std::uint32_t nextId_=1;
    bool loaded_=false,executing_=false;
    std::string failure_;
    CollectHost collect_;
    std::function<bool(std::string&)> miss_;
    anm::Runtime runtime(Item& item);
    bool fail(std::string reason,std::string& error);
};
}
