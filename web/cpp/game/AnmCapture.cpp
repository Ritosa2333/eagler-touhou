#include "AnmCapture.hpp"
#include "AnmFile.hpp"
namespace th10 {
// 0x424480. The sprite bounds are converted before checking for an existing
// request; width/height preserve the original sprite field ordering.
i32 AnmCapture::from_animation(u32 animation,const CaptureRectangle& rectangle,const AnmRegistry& registry) noexcept {
    auto* vm=registry.find(animation);if(!vm||!vm->sprite)return -1;const auto& sprite=*vm->sprite;
    const i32 height=Scalar::truncate(sprite.height),width=Scalar::truncate(sprite.width),top=Scalar::truncate(sprite.top),left=Scalar::truncate(sprite.left);
    if(target_file>=0)return -1;target_file=vm->animation_file->file_index;source=rectangle;destination={left,top,width,height};flags=0;return 0;
}
// 0x424530. A queued capture is never overwritten.
i32 AnmCapture::request(i32 file,u32 mode,const CaptureRectangle& rectangle,const CaptureRectangle& target) noexcept {if(target_file>=0)return -1;target_file=file;source=rectangle;destination=target;flags=mode;return 0;}
// 0x447f70. Release in texture/surface/CPU-buffer order; platform callbacks
// may change another slot, so each pointer is read when it is consumed.
void AnmCaptureBuffers::release(i32 slot,AnmCaptureEnvironment& env){if(textures[slot]){env.release_surface(textures[slot]);textures[slot]=nullptr;}if(surfaces[slot]){env.release_surface(surfaces[slot]);surfaces[slot]=nullptr;}if(pixels[slot]){env.free_pixels(pixels[slot]);pixels[slot]=nullptr;}pixels[slot]=nullptr;}
}
