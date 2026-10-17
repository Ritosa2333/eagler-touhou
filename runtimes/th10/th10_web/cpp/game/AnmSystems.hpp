#pragma once
#include "AnmManager.hpp"
#include "UpdateChain.hpp"
namespace th10 {
// Two original global templates have a fourth 32-bit attribute whose meaning
// depends on the vertex format. Construction preserves all untouched fields.
struct AnmVertex24 {Vec3 position;u32 attribute;Vec2 uv;};
struct AnmSystemEnvironment {
    AnmVertex24* initial_quad;
    AnmVertex* render_quad;
    AnmVertex24* model_quad;
    UpdateChain* chain;
    UpdateChainEnvironment* callbacks;
    CallbackToken frame_callbacks[20];
    virtual void clear_pixel_shader()=0;
    virtual void create_model_buffer(void*& buffer)=0;
    virtual void* lock_model_buffer(void* buffer)=0;
    virtual void unlock_model_buffer(void* buffer)=0;
    virtual void bind_model_buffer(void* buffer)=0;
};
}
