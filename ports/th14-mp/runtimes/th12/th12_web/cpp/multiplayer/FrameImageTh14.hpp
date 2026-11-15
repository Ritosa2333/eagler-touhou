#pragma once
#include <algorithm>
#include <array>
#include <bitset>
#include <cstddef>
#include <cstring>
#include <cstdlib>
#include <memory>
#include <new>
#include <tuple>
#include <type_traits>
#include <vector>

namespace th11 {
// Typed frame images keep the live graph at its original addresses. Dynamic
// owners are retained until the last image referring to them is discarded.
class FrameImageTh14 {
    static constexpr std::size_t block_size=32*1024;
public:
    enum class Array {Stage, ActiveAnimations, IdleAnimations, Count};
private:
    struct Step {virtual ~Step()=default;virtual void restore()const=0;};
    template<class F> struct Action final:Step {
        F action;
        explicit Action(F value):action(std::move(value)){}
        void restore()const override{action();}
    };
    template<class T> struct Value {
        T& target;
        T value;
        explicit Value(T& object):target(object),value(object){}
        void restore()const{target=value;}
    };
    template<class T,size_t N> struct Value<T[N]> {
        T (&target)[N];
        std::array<T,N> value;
        explicit Value(T (&object)[N]):target(object){
            if constexpr(std::is_array_v<T>)std::memcpy(value.data(),object,sizeof object);
            else std::copy_n(object,N,value.begin());
        }
        void restore()const{
            if constexpr(std::is_array_v<T>)std::memcpy(target,value.data(),sizeof target);
            else std::copy_n(value.begin(),N,target);
        }
    };
    template<class... T> struct Values final:Step {
        std::tuple<Value<T>...> values;
        explicit Values(T&... args):values(args...){}
        void restore()const override{std::apply([](const auto&... value){(value.restore(),...);},values);}
    };
    // Each live image retains its owners independently. Reuse the backing
    // blocks after destroying the values, including their shared references.
    struct Block {
        std::unique_ptr<std::byte,decltype(&std::free)> data{nullptr,&std::free};
        std::size_t size=0,used=0;
        Block()=default;
        explicit Block(std::size_t bytes):data(static_cast<std::byte*>(std::malloc(bytes)),&std::free),size(bytes){}
        bool fits(std::size_t offset,std::size_t bytes)const noexcept{return offset<=size&&bytes<=size-offset;}
    };
    std::vector<Block> blocks;
    std::array<Block,static_cast<std::size_t>(Array::Count)> arrays;
    std::vector<Step*> steps;
    std::size_t reserved=0;
    std::size_t limit=0;
    bool complete=true;
    void* allocate(std::size_t bytes,std::size_t alignment){
        if(!complete)return nullptr;
        for(auto& block:blocks){
            const auto offset=(block.used+alignment-1)&~(alignment-1);
            if(!block.fits(offset,bytes))continue;
            block.used=offset+bytes;
            return block.data.get()+offset;
        }
        Block next=new_block(bytes);
        if(!next.data)return nullptr;
        next.used=bytes;
        blocks.push_back(std::move(next));
        return blocks.back().data.get();
    }
    Block new_block(std::size_t bytes){
        // Large arrays need one contiguous allocation. Round them to pages,
        // so a 5 MiB animation array does not reserve 8 MiB in every slot.
        if(bytes>limit||bytes>static_cast<std::size_t>(-1)-(block_size-1)){
            complete=false;
            return {};
        }
        const auto capacity=(bytes+block_size-1)&~(block_size-1);
        if(capacity>limit-reserved){
            complete=false;
            return {};
        }
        Block next(capacity);
        if(!next.data){
            complete=false;
            return {};
        }
        reserved+=capacity;
        return next;
    }
    template<class T,class... Args> void append(Args&&... args){
        static_assert(alignof(T)<=alignof(std::max_align_t));
        if(auto* storage=allocate(sizeof(T),alignof(T)))
            steps.push_back(::new(storage) T(std::forward<Args>(args)...));
    }
public:
    FrameImageTh14()=default;
    FrameImageTh14(const FrameImageTh14&)=delete;
    FrameImageTh14& operator=(const FrameImageTh14&)=delete;
    ~FrameImageTh14(){clear();}
    void begin(std::size_t bytes){limit=std::max(reserved,bytes);complete=true;}
    bool ready()const noexcept{return complete;}
    std::size_t reserved_bytes()const noexcept{return reserved;}
    template<class F> void after_restore(F action){append<Action<F>>(std::move(action));}
    template<class... T> void save(T&... value){append<Values<T...>>(value...);}
    // Build animation entries in the arena. Only values that need no
    // destructor belong here; retained owners still use typed actions.
    template<class T> T* plain_storage(Array array,std::size_t count){
        static_assert(std::is_trivially_destructible_v<T>&&alignof(T)<=alignof(std::max_align_t));
        if(!count||!complete)return nullptr;
        auto& block=arrays[static_cast<std::size_t>(array)];
        const auto bytes=sizeof(T)*count;
        if(bytes>block.size){
            // Each of these arrays is captured once per image. Its previous
            // allocation is idle and can be released before growing it.
            reserved-=block.size;
            block=Block{};
            block=new_block(bytes);
        }
        return reinterpret_cast<T*>(block.data.get());
    }
    template<class T> void save_block(Array array,std::vector<T>& objects){
        if(objects.empty())return;
        if constexpr(std::is_trivially_copyable_v<T>){
            const auto bytes=objects.size()*sizeof(T);
            auto* data=objects.data();
            auto* saved=plain_storage<T>(array,objects.size());
            if(!saved)return;
            std::memcpy(saved,data,bytes);
            after_restore([data,saved,bytes]{std::memcpy(data,saved,bytes);});
        }else after_restore([data=objects.data(),saved=objects]{std::copy(saved.begin(),saved.end(),data);});
    }
    void restore()const{for(const auto& step:steps)step->restore();}
    void clear(){
        for(auto* step:steps)step->~Step();
        steps.clear();
        for(auto& block:blocks)block.used=0;
    }
    void release_storage(){
        clear();
        blocks.clear();
        for(auto& block:arrays)block=Block{};
        reserved=0;
    }
    void swap_storage(FrameImageTh14& other) noexcept{
        blocks.swap(other.blocks);
        arrays.swap(other.arrays);
        steps.swap(other.steps);
        std::swap(reserved,other.reserved);
    }
};

// Occupied slots are saved at the frame boundary. An allocation also touches
// the idle slot it is about to reuse, so its old payload survives release/spawn.
template<class T,std::size_t N> struct SlotImageTh14 {
    using Restored=std::bitset<N>;
    T* base=nullptr;
    std::bitset<N> touched;
    std::vector<std::pair<std::size_t,T>> saved;
    void save(T& value){
        const auto n=std::size_t(&value-base);
        if(!touched[n]){touched.set(n);saved.emplace_back(n,value);}
    }
    void restore()const{for(const auto& [n,value]:saved)base[n]=value;}
    void restore_first(Restored& restored)const{
        if((touched&~restored).none())return;
        for(const auto& [n,value]:saved)if(!restored.test(n)){
            base[n]=value;
            restored.set(n);
        }
    }
    void clear(){touched.reset();saved.clear();}
    std::size_t reserved_bytes()const noexcept{return saved.capacity()*sizeof(saved.front());}
    void release_storage(){clear();decltype(saved){}.swap(saved);}
    void swap_storage(SlotImageTh14& other) noexcept{saved.swap(other.saved);}
};
}
