#pragma once
#include <cstdint>
#include <limits>
#include <utility>

namespace th08 {
class DisposableWritePolicy {
public:
    struct Receipt {
        std::uint32_t version=1,locked=0,load_scope=0,attempts=0,ram_overrides=0;
        std::uint32_t discarded_bytes=0,committed_bytes=0,rejected_writes=0;
    };
private:
    Receipt receipt_{};
    static std::uint32_t add_saturated(std::uint32_t a,std::uint32_t b)noexcept{
        return b>std::numeric_limits<std::uint32_t>::max()-a?
            std::numeric_limits<std::uint32_t>::max():a+b;
    }
public:
    void arm()noexcept{
        if(receipt_.locked)return;
        receipt_={};receipt_.version=1;receipt_.locked=1;
    }
    void load_scope(bool active)noexcept{
        if(receipt_.locked)receipt_.load_scope=active?1u:0u;
    }
    template<class PersistentWrite,class RamWrite>
    bool write(std::uint32_t bytes,bool ram_override_allowed,
               PersistentWrite&& persistent_write,RamWrite&& ram_write){
        if(!receipt_.locked)return std::forward<PersistentWrite>(persistent_write)();
        receipt_.attempts=add_saturated(receipt_.attempts,1);
        if(!receipt_.load_scope||!ram_override_allowed){
            receipt_.rejected_writes=add_saturated(receipt_.rejected_writes,1);return false;
        }
        if(!std::forward<RamWrite>(ram_write)()){
            receipt_.rejected_writes=add_saturated(receipt_.rejected_writes,1);return false;
        }
        receipt_.ram_overrides=add_saturated(receipt_.ram_overrides,1);
        receipt_.discarded_bytes=add_saturated(receipt_.discarded_bytes,bytes);
        return true;
    }
    const Receipt& receipt()const noexcept{return receipt_;}
};
}
