#include "AnmVm.hpp"

#include <stddef.h>

const u32 g_TextureFormatD3D8Mapping[6] = {0, 1, 2, 3, 4, 5};

const i32 g_TextureBytesPerPixel[7] = {4, 4, 2, 2, 3, 2, 0};

i32 ZunTimer::NextTick()
{
    this->Tick();
    return this->current;
}

void AnmVm::Initialize()
{
    memset(this, 0, (u8 *)&this->pos - (u8 *)this);
    this->scale.x = 1.0f;
    this->scale.y = 1.0f;
    this->prevScale.x = 1.0f;
    this->prevScale.y = 1.0f;
    this->prevColor.color = this->color.color = 0xffffffff;
    this->matrix.Identity();
    *(u16 *)&this->flags = 7;
    this->currentTimeInScript.Initialize();
}
