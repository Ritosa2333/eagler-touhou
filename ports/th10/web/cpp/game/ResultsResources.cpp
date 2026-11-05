#include "ResultsResources.hpp"
namespace th10 {
// 0x422150 / 0x422220. Preview ownership belongs to the result interface;
// animation files are borrowed from the GUI and shared resource manager.
i32 ResultsResources::start(){auto& env=environment;results.update_entry=(*env.chain)->add(env.update_callback,&results,9,false,false,*env.callbacks);results.draw_entry=(*env.chain)->add(env.draw_callback,&results,46,true,false,*env.callbacks);results.reset_timer(env.rate);return 0;}
void ResultsResources::shutdown(){auto& env=environment;(*env.chain)->remove_locked(results.update_entry,*env.callbacks);(*env.chain)->remove_locked(results.draw_entry,*env.callbacks);for(auto* preview:results.previews)if(preview)env.delete_replay(preview);env.registry->delete_and_clear(results.auxiliary_animation);*env.current=nullptr;}
Results* ResultsResources::create(ResultsResourceEnvironment& env){auto* result=env.allocate();if(!result)return nullptr;result->initialize(env.current);if(ResultsResources{*result,env}.start()){ResultsResources{*result,env}.shutdown();env.delete_object(result);return nullptr;}return result;}
}
