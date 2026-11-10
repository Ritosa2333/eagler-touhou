#include "PresentationAudit.hpp"
#include <cassert>
#include <cstdio>

extern "C" const th11::presentation_audit::Record* audit_records();
extern "C" th11::u32 audit_count();

int main(){
    using namespace th11;
    using namespace th11::presentation_audit;
    enable(true);
    begin_reference();scope(1);
    AnmVm vm{};vm.flags=3;
    AnmVertex vertices[4]{};
    authored_rebase(true);capture(vm,vertices,4);
    assert(audit_count()==1);
    assert(audit_records()[0].continuous&AuthoredRebase);
    authored_rebase(false);capture(vm,vertices,4);
    assert(audit_count()==2);
    assert(!(audit_records()[1].continuous&AuthoredRebase));
    // An observation session cannot retain an event from an earlier session.
    authored_rebase(true);enable(false);enable(true);
    assert(!authored_rebase());
    std::puts("TH11 authored rebase audit CPU contract PASS");
}
