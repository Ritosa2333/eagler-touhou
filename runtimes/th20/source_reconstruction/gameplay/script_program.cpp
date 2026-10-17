#include "script_loader.hpp"
#include "../ecl_vm/vm.hpp"
#include <stdexcept>
namespace th20::source::gameplay {
void ScriptLoader::bind_program(ecl::Program& program) const {
    program.subroutines.clear();program.subroutines.reserve(records.size());
    for(const auto& record:records) {
        const auto first=reinterpret_cast<std::uintptr_t>(record.header);
        std::uintptr_t last=0;
        for(auto span:loaded_spans) {
            const auto begin=reinterpret_cast<std::uintptr_t>(span.data());
            if(first>=begin&&first-begin<=span.size()&&span.size()-(first-begin)>=16) {last=begin+span.size();break;}
        }
        if(!last)throw std::out_of_range("Unowned ECL subroutine record");
        // Give the VM the complete containing file's valid tail, preserving the
        // original address model without copying or executing native code.
        program.append_borrowed(record.name,record.header+16,last-first-16);
    }
}
}
