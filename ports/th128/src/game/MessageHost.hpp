#pragma once
#include <array>
#include <cstdint>
#include <functional>
#include <optional>
#include <span>
#include <string>
#include <vector>

namespace th128::message {
struct Input {std::uint32_t holding=0,pressed=0,released=0;};
struct Event {int opcode=0,entry=0,time=0;std::vector<std::uint8_t> bytes;std::array<int,2> integers{};std::array<float,2> floats{};};
struct State {
    bool active=false,eclReleased=true,waiting=false,skipEnabled=false,pressSeen=false;
    int entry=-1,clock=0,waitRemaining=0,speaker=0,font=4,heldShoot=0,heldRapid=0;
    float x=400,y=480;
    std::vector<std::vector<std::uint8_t>> textLines;
};
class Host {
public:
    using EventHost=std::function<bool(const Event&,std::string&)>;
    struct ExternalWait {
        std::function<bool(const Event&,std::string&)> begin;
        std::function<std::optional<bool>(std::string&)> poll;
    };
    bool load(std::span<const std::uint8_t> bytes,std::string& error);
    bool setEventHost(EventHost host,std::string& error);
    bool setExternalWait(ExternalWait wait,std::string& error);
    bool start(int entry,std::string& error);
    bool tick(Input input,std::string& error);
    const State& state() const{return state_;}
    std::optional<bool> recordAuxiliaryLive() const noexcept {
        if(entries_.empty()||!failure_.empty()||routeChoicePresent_)return std::nullopt;
        return false;
    }
    bool configurationOpen() const{return !executing_&&failure_.empty()&&!state_.active&&state_.entry<0;}
private:
    struct Command {std::uint16_t time=0;std::uint8_t opcode=0;std::vector<std::uint8_t> bytes;};
    std::vector<std::vector<Command>> entries_;
    State state_;
    EventHost event_;
    ExternalWait externalWait_;
    std::size_t cursor_=0;
    bool executing_=false;
    bool externalHeld_=false;
    bool routeChoicePresent_=false;
    std::string failure_;
    bool fail(std::string reason,std::string& error);
    bool deliver(const Event& event,std::string& error);
};
}
