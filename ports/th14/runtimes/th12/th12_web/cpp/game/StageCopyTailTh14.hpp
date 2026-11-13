#pragma once
#include "AnmVm.hpp"
namespace th11 {
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
// Retail436099/4360a4/4360af/4360ba call41bd80. Retain the VM's
// interrupt handling, then defer label2; never update the VM on this worker.
inline void th14_defer_stage_copy_tail(AnmVm (&copies)[4])noexcept{
    for(auto& copy:copies)copy.interrupt(2);
}
#endif
}
