#pragma once
#include <memory>
#include <tuple>
#include <utility>
#include <vector>
#if TH17_ROOM_BENCHMARK
#include <array>
#include <cstdint>
#endif

namespace th17::multiplayer {
// Values are copied by type. Owners must keep reference targets alive until
// the image is discarded, and restore owning containers before their graphs.
class FrameImage {
    struct Step {
        virtual ~Step()=default;
        virtual bool restore()=0;
    };
    template<class F> struct Action final:Step {
        F action;
        explicit Action(F&& value):action(std::move(value)){}
        bool restore()override{return action();}
    };
    template<class... T> struct Values final:Step {
        std::tuple<T&...> targets;
        std::tuple<T...> data;
        explicit Values(T&... values):targets(values...),data(values...){}
        bool restore()override{targets=data;return true;}
    };
    std::vector<std::unique_ptr<Step>> steps;
public:
#if TH17_ROOM_BENCHMARK
    enum Payload { BulletState, BulletVisual, ItemVisual, IdleItemVisual, Animation, PayloadCount };
    std::array<std::uint32_t,PayloadCount> payload{};
    std::uint32_t unused_capacity=0;
    bool empty()const noexcept{return steps.empty();}
#endif
    template<class F> void restore_with(F action){steps.push_back(std::make_unique<Action<F>>(std::move(action)));}
    template<class... T> void save(T&... values){
        // Construct the snapshot in its final storage. Moving a closure that
        // contains inline projectile pools copies those entire arrays again.
        steps.push_back(std::make_unique<Values<T...>>(values...));
    }
    bool restore()const{
        for(const auto& step:steps)if(!step->restore())return false;
        return true;
    }
    void clear(){
        steps.clear();
#if TH17_ROOM_BENCHMARK
        payload.fill(0);unused_capacity=0;
#endif
    }
};
}
