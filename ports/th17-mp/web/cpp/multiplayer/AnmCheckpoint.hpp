#pragma once
#include "../game/AnmVm.hpp"
#include <cstring>
#include <type_traits>

namespace th17::multiplayer {
// Stable curves can survive several frames and slot reuse. Share only exact
// copies, including expired interpolation fields that later scripts can reuse.
class AnmCheckpoint {
    AnmVmState state;
    AnmGeometry geometry;
    std::shared_ptr<const AnmInterpolators> resting;
    size_t curve_index=0;
public:
    using Cache=std::weak_ptr<const AnmInterpolators>;
    struct Curves {
        std::vector<AnmInterpolators> changing;
        std::shared_ptr<const AnmInterpolators> last_resting;
    };
    static bool changing(const AnmVm& vm)noexcept{
        return !vm.interpolators.mask_valid||vm.interpolators.active_mask!=0;
    }
    AnmCheckpoint(const AnmVm& vm,Cache& cache,Curves& curves):state(vm),geometry(vm.geometry){
        if(changing(vm)){
            curve_index=curves.changing.size();
            curves.changing.push_back(vm.interpolators);
            return;
        }
        static_assert(std::is_trivially_copyable_v<AnmInterpolators>);
        const auto matches=[&](const auto& saved){
            return saved&&std::memcmp(saved.get(),&vm.interpolators,sizeof(AnmInterpolators))==0;
        };
        resting=cache.lock();
        if(!matches(resting)){
            if(!matches(curves.last_resting))
                curves.last_resting=std::shared_ptr<const AnmInterpolators>(new AnmInterpolators(vm.interpolators));
            resting=curves.last_resting;
            cache=resting;
        }
    }
    void restore(AnmVm& vm,Cache& cache,const Curves& curves)const{
        static_cast<AnmVmState&>(vm)=state;
        vm.geometry=geometry;
        vm.interpolators=resting?*resting:curves.changing[curve_index];
        if(resting)cache=resting;
    }
};
}
