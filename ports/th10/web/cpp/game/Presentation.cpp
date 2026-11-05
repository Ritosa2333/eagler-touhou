#include "Presentation.hpp"
namespace th10 {
void Presentation::configure_defaults(){
    auto& env=environment;
#ifdef TH_NATIVE_PLATFORM
    env.configure_graphics();
#else
    constexpr u32 states[][2]={{7,1},{137,0},{22,1},{27,1},{9,2},{19,5},{20,6},{23,8},{15,1},{24,1},{25,7},{28,1},{38,0x3f800000},{35,0},{140,3},{34,0xffa0a0a0},{36,0x447a0000},{37,0x459c4000},{161,0}};
    constexpr u32 stages[][2]={{4,4},{5,2},{6,3},{1,4},{2,2},{3,3},{24,2},{11,0}};
    constexpr u32 samplers[][2]={{7,0},{5,2},{6,2},{3,3},{1,1},{2,1}};
    for(const auto& state:states)env.render_state(env.application->device,state[0],state[1]);
    for(const auto& stage:stages)env.texture_stage(env.application->device,0,stage[0],stage[1]);
    for(const auto& sampler:samplers)env.sampler_state(env.application->device,0,sampler[0],sampler[1]);
#endif
    if(auto* manager=*env.animations){manager->cached_draw_state[0]=3;manager->cached_draw_state[1]=255;manager->cached_draw_state[2]=255;manager->current_texture=nullptr;manager->cached_draw_state[4]=255;}
}
void Presentation::release_textures(AnmManager& manager){auto* buffers=reinterpret_cast<AnmCaptureBuffers*>(manager.resource_state+4);for(auto& texture:buffers->textures)if(texture){environment.release_surface(texture);texture=nullptr;}}
void Presentation::process_captures(AnmManager& manager){
    auto& requests=*reinterpret_cast<CaptureRequests*>(manager.header);
    if(requests.texture.target_file>=0){const auto request=requests.texture;environment.capture_texture(manager,request.target_file,request.flags,request.source,request.destination);requests.texture.target_file=-1;}
    if(requests.texture.reserved_000>=0){environment.capture_screen(manager,requests.texture.reserved_000,requests.screen_source,requests.screen_destination);requests.texture.reserved_000=-1;}
}
void Presentation::submit(){
    auto& env=environment;if(env.present(env.application->device)<0){release_textures(**env.animations);env.reset_device(env.application->device,env.presentation_parameters);configure_defaults();*env.reset_frames=2;}
    process_captures(**env.animations);
    if(*env.pressed_keys&0x800){env.create_directory("snapshot");for(u32 i=0;i<1000;++i){char name[]="snapshot/th000.bmp";name[11]='0'+i/100;name[12]='0'+i/10%10;name[13]='0'+i%10;if(!env.file_exists(name)){env.save_screenshot(*env.application,name);break;}}}
}
}
