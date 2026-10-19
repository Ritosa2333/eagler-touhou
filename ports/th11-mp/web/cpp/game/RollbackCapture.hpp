#pragma once
#ifdef TH11_MULTIPLAYER
#include <eagler/netplay/RollbackJournal.hpp>
#include <type_traits>
namespace th11 {
// Call sites enumerate explicit value owners. This does not authorize copying
// a containing polymorphic object, reference binding, or dynamic container.
template<class... Values>
bool rollback_capture_values(Netplay::RollbackJournal& journal,Values&... values){
    static_assert((std::is_trivially_copyable<Values>::value&&...),
                  "rollback capture requires explicit value payloads");
    if(!journal.IsFrameOpen()||journal.Failed())return false;
    bool ok=true;
    ((ok=journal.Touch(&values,sizeof values)&&ok),...);
    return ok&&!journal.Failed();
}
// For explicitly once-per-checkpoint fields only: join strictly adjacent
// payloads without reading padding, pointer owners or container headers.
// Callers with later subfield Touch operations must retain capture_values.
template<class... Values>
bool rollback_capture_contiguous_values(Netplay::RollbackJournal& journal,Values&... values){
    static_assert((std::is_trivially_copyable<Values>::value&&...),"explicit value payloads only");
    if(!journal.IsFrameOpen()||journal.Failed())return false;
    // Keep the old duplicate/overlap contract for non-monotonic argument
    // lists. Decide before touching anything, so a later duplicate cannot
    // turn an earlier single-field record into an overlapping merged run.
    std::uintptr_t previous=0;std::size_t previous_size=0;bool ordered=true;
    const auto check_order=[&](void* address,std::size_t size){
        const auto a=reinterpret_cast<std::uintptr_t>(address);
        if(previous_size&&(a<previous||a-previous<previous_size))ordered=false;
        previous=a;previous_size=size;
    };
    (check_order(&values,sizeof values),...);
    if(!ordered)return rollback_capture_values(journal,values...);
    void* first=nullptr;std::size_t bytes=0;bool ok=true;
    const auto flush=[&](){if(bytes)ok=journal.Touch(first,bytes)&&ok;};
    const auto append=[&](void* address,std::size_t size){
        const auto a=reinterpret_cast<std::uintptr_t>(address),b=reinterpret_cast<std::uintptr_t>(first);
        if(bytes&&a>=b&&a-b==bytes)bytes+=size;
        else {flush();first=address;bytes=size;}
    };
    (append(&values,sizeof values),...);flush();
    return ok&&!journal.Failed();
}
}
#endif
