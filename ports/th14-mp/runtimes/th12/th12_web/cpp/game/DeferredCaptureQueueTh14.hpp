#pragma once
#include <array>
#include <cstdint>
namespace th11 {
struct DeferredCaptureRequestTh14 {
 std::int32_t file_index=-1,texture_index=0;
 // Both rectangles retain the original x/y/width/height record words.
 std::array<std::int32_t,4> source{},destination{};
};
static_assert(sizeof(DeferredCaptureRequestTh14)==40);
struct DeferredCaptureQueueTh14 {
 std::array<DeferredCaptureRequestTh14,4> slots{};
 // Main owns each record's payload. The loading worker only reads its
 // publication word; keep the native40-byte layout and avoid a C++ data race.
 std::int32_t first_file_index()const noexcept{return __atomic_load_n(&slots[0].file_index,__ATOMIC_ACQUIRE);}
 std::int32_t enqueue(const DeferredCaptureRequestTh14& request) noexcept {
  for(auto& slot:slots)if(__atomic_load_n(&slot.file_index,__ATOMIC_ACQUIRE)<0){
   slot.texture_index=request.texture_index;slot.source=request.source;slot.destination=request.destination;
   __atomic_store_n(&slot.file_index,request.file_index,__ATOMIC_RELEASE);break;
  }
  return 0; // Original44bdd0 also returns0 when all slots are occupied.
 }
 template<class Copy> void consume(Copy&& copy){
  for(auto& slot:slots)if(__atomic_load_n(&slot.file_index,__ATOMIC_ACQUIRE)>=0){
   copy(slot); // Null target and GetSurfaceLevel error still retire the record.
   __atomic_store_n(&slot.file_index,-1,__ATOMIC_RELEASE);
  }
 }
};
}
