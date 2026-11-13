#pragma once
#include <atomic>
#include <cstdlib>
#include <memory>
namespace th11 {
// One publishing worker; readers never access the unique ownership storage.
// Assignment requires an empty slot. reset/destruction require all publishers
// joined and all readers quiescent (the caller's Main teardown boundary).
template<class T> class PublishedOwnerTh14 {
    std::unique_ptr<T> owned;
    std::atomic<T*> visible{nullptr};
public:
    PublishedOwnerTh14()=default;
    PublishedOwnerTh14(const PublishedOwnerTh14&)=delete;
    PublishedOwnerTh14& operator=(const PublishedOwnerTh14&)=delete;
    ~PublishedOwnerTh14(){reset();}
    PublishedOwnerTh14& operator=(std::unique_ptr<T> next)noexcept{
        if(owned||visible.load(std::memory_order_acquire))std::abort();
        auto* pointer=next.get();owned=std::move(next);
        visible.store(pointer,std::memory_order_release);return *this;
    }
    T* get()const noexcept{return visible.load(std::memory_order_acquire);}
    T* operator->()const noexcept{return get();}
    explicit operator bool()const noexcept{return get()!=nullptr;}
    void reset()noexcept{visible.store(nullptr,std::memory_order_release);owned.reset();}
};
}
