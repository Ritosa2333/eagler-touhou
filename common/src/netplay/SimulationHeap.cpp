// An independent, fixed-base dlmalloc mspace. Its free lists and chunk headers
// live in the journaled region, so restoring state also restores allocation.
#if defined(EAGLER_WRITE_JOURNAL) && EAGLER_WRITE_JOURNAL
#define ONLY_MSPACES 1
#define MSPACES 1
#define USE_LOCKS 0
#define HAVE_MORECORE 0
#define HAVE_MMAP 0
#define HAVE_MREMAP 0
#define NO_MALLINFO 0
#define NO_MALLOC_STATS 1
#define MALLOC_ALIGNMENT 16
#ifndef __EMSCRIPTEN__
#define DLMALLOC_EXPORT static
#endif
extern "C" {
#include "../../third_party/dlmalloc/dlmalloc.inc"

void* eagler_sim_create(void* base, size_t capacity) {
    auto space = create_mspace_with_base(base, capacity, 0);
    if (space) mspace_set_footprint_limit(space, capacity);
    return space;
}
void* eagler_sim_malloc(void* space, size_t bytes) {
    return mspace_malloc(static_cast<mspace>(space), bytes);
}
void* eagler_sim_memalign(void* space, size_t alignment, size_t bytes) {
    return mspace_memalign(static_cast<mspace>(space), alignment, bytes);
}
void eagler_sim_free(void* space, void* pointer) {
    mspace_free(static_cast<mspace>(space), pointer);
}
size_t eagler_sim_used(void* space) {
    return mspace_mallinfo(static_cast<mspace>(space)).uordblks;
}
}
#endif // EAGLER_WRITE_JOURNAL
