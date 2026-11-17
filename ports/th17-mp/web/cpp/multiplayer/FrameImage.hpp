#pragma once
#include <algorithm>
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
    struct Reusable {
        const void* type;
        explicit Reusable(const void* key):type(key){}
        virtual ~Reusable()=default;
        virtual void reset()=0;
    };
    template<class T> struct Storage final:Reusable {
        inline static const char key=0;
        T data;
        Storage():Reusable(&key){}
        void reset()override{data.reset();}
    };
public:
    class BufferPool {
        friend class FrameImage;
        std::vector<std::unique_ptr<Reusable>> available;
        auto find(const void* key){
            return std::find_if(available.begin(),available.end(),[key](const auto& buffer){return buffer->type==key;});
        }
    };
private:
    BufferPool local_pool;
    BufferPool* shared_pool=nullptr;
    std::vector<std::unique_ptr<Reusable>> buffers;
    std::vector<std::unique_ptr<Step>> steps;
    BufferPool& pool(){return shared_pool?*shared_pool:local_pool;}
public:
#if TH17_ROOM_BENCHMARK
    enum Payload { BulletState, BulletVisual, ItemVisual, IdleItemVisual, Animation, PayloadCount };
    std::array<std::uint32_t,PayloadCount> payload{};
    std::uint32_t unused_capacity=0;
    bool empty()const noexcept{return steps.empty();}
#endif
    // Bind before the first capture. The room's pool outlives its history.
    void use_buffer_pool(BufferPool& value){shared_pool=&value;}
    // A released image lends its buffers to the next capture. Live snapshots
    // still own their storage independently and can be restored in any order.
    template<class T> T& reuse(){
        auto& cache=pool();
        auto found=cache.find(&Storage<T>::key);
        if(found!=cache.available.end()){
            buffers.push_back(std::move(*found));
            cache.available.erase(found);
        }else{
            buffers.push_back(std::make_unique<Storage<T>>());
        }
        return static_cast<Storage<T>&>(*buffers.back()).data;
    }
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
        if(!buffers.empty()){
            auto& cache=pool();
            for(auto& buffer:buffers){
                buffer->reset();
                auto found=cache.find(buffer->type);
                if(found==cache.available.end())cache.available.push_back(std::move(buffer));
                else *found=std::move(buffer);
            }
            buffers.clear();
        }
#if TH17_ROOM_BENCHMARK
        payload.fill(0);unused_capacity=0;
#endif
    }
};
}
