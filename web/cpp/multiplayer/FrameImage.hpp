#pragma once
#include <functional>
#include <tuple>
#include <vector>
#if TH17_ROOM_BENCHMARK
#include <array>
#include <cstdint>
#endif

namespace th17::multiplayer {
// Values are copied by type. Owners must keep reference targets alive until
// the image is discarded, and restore owning containers before their graphs.
class FrameImage {
    std::vector<std::function<bool()>> steps;
public:
#if TH17_ROOM_BENCHMARK
    enum Payload { BulletState, BulletVisual, ItemVisual, IdleItemVisual, Animation, PayloadCount };
    std::array<std::uint32_t,PayloadCount> payload{};
    std::uint32_t unused_capacity=0;
    bool empty()const noexcept{return steps.empty();}
#endif
    template<class F> void restore_with(F action){steps.emplace_back(std::move(action));}
    template<class... T> void save(T&... values){
        restore_with([targets=std::tie(values...), data=std::make_tuple(values...)]() mutable {
            targets=data;
            return true;
        });
    }
    bool restore()const{
        for(const auto& step:steps)if(!step())return false;
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
