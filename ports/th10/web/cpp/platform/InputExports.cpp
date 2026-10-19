#include "Input.hpp"
#include <new>
#include <cstdlib>
using namespace th10;
#define INPUT_EXPORT(name) extern "C" __attribute__((export_name(name)))
INPUT_EXPORT("input_create") browser::Input* input_create(){auto* bytes=std::malloc(sizeof(browser::Input));return bytes?new(bytes)browser::Input:nullptr;}
INPUT_EXPORT("input_destroy") void input_destroy(browser::Input* input){if(input){input->~Input();std::free(input);}}
INPUT_EXPORT("input_snapshot") browser::InputSnapshot* input_snapshot(browser::Input* input){return &input->snapshot;}
INPUT_EXPORT("input_profiles") InputProfile* input_profiles(browser::Input* input){return input->profiles;}
INPUT_EXPORT("input_configure") void input_configure(browser::Input* input,u32 flags,i32 x,i32 y){input->flags=flags;input->thresholds[0]=x;input->thresholds[1]=y;}
INPUT_EXPORT("input_sample") u32 input_sample(browser::Input* input){return InputDevices{*input}.sample();}
INPUT_EXPORT("input_update") void input_update(browser::Input* input,u32 profile,u32 first){if(profile<2)InputDevices{*input}.update(profile,first!=0);}
INPUT_EXPORT("input_buttons") u8* input_buttons(browser::Input* input,u32 device){return InputDevices{*input}.controller_buttons(device);}
INPUT_EXPORT("input_release_keys") void input_release_keys(browser::Input* input){InputDevices{*input}.release_keys();}
