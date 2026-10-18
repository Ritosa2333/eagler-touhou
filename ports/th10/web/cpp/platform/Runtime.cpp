// An invalid virtual dispatch is a Wasm trap. Pulling libc++abi's diagnostic
// implementation would otherwise import WASI file descriptors and stderr.
extern "C" [[noreturn]] void __cxa_pure_virtual(){__builtin_trap();}
#if !defined(TH_SDL3) && defined(__wasi__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpragma-clang-attribute"
#include <__verbose_abort>
#include <cstdlib>
#include <new>
// The standalone browser host has no operating-system stderr. Match its
// existing invalid-dispatch behavior for libc++ allocation/length failures.
_LIBCPP_BEGIN_NAMESPACE_STD
_LIBCPP_BEGIN_EXPLICIT_ABI_ANNOTATIONS
[[noreturn]] void __libcpp_verbose_abort(const char*,...) noexcept{__builtin_trap();}
_LIBCPP_END_EXPLICIT_ABI_ANNOTATIONS
_LIBCPP_END_NAMESPACE_STD
void* operator new(std::size_t size){if(auto* p=std::malloc(size?size:1))return p;__builtin_trap();}
void* operator new[](std::size_t size){return ::operator new(size);}
void operator delete(void* p) noexcept{std::free(p);}
void operator delete[](void* p) noexcept{std::free(p);}
void operator delete(void* p,std::size_t) noexcept{std::free(p);}
void operator delete[](void* p,std::size_t) noexcept{std::free(p);}
#pragma clang diagnostic pop
#endif
