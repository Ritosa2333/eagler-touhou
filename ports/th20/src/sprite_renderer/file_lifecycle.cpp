#include "animation_file.hpp"
#include "file_text.hpp"
#include <stdexcept>
#include <atomic>
namespace th20::source::sprite {
void clear_animation_file(Controller& controller,AnimationFile& file) {
    if(!file.bytes)return;
    file_environment::detach_file_animations(controller,file,false);
    for(std::int32_t i=0;i<file.texture_count;++i)release_texture_data(file.textures[i]);
    {
        std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(1));
        delete[] file.textures;file.textures=nullptr;
        delete[] file.sprites;file.sprites=nullptr;
        delete[] file.scripts;file.scripts=nullptr;
    }
    if(file.fields_5c[4]){runtime::release_bytes(reinterpret_cast<void*>(file.fields_5c[4]));file.fields_5c[4]=0;}
    runtime::release_bytes(file.bytes);file.bytes=nullptr;
    {
        std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(1));
        if(file.templates)for(std::uint32_t i=file.script_count;i>0;--i)file_environment::destroy_animation_contents(file.templates[i-1]);
        delete[] file.templates;file.templates=nullptr;
    }
}
void destroy_animation_file(Controller& controller,AnimationFile* file) {
    if(!file)return;clear_animation_file(controller,*file);
    std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(1));delete file;
}
AnimationFile* preload_animation_file(Controller& controller,std::int32_t index,const char* name,runtime::Log& log) {
    if(index>=42){runtime::log_error(log,file_text::s0056e38c);return nullptr;}
    if(index<0)throw std::out_of_range("Original ANM negative index addresses outside file array");
    AnimationFile* file;
    {
        std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(17));
        file=create_animation_file();controller.files[index]=file;file->id=index;
    }
    if(preload_animation_data(*file,name,log)!=0) {
        destroy_animation_file(controller,file);
        // Do not expose the specimen's dangling pointer after failed loading.
        // Valid archives never take this defined failure-domain correction.
        controller.files[index]=nullptr;return nullptr;
    }
    return file;
}
bool animation_files_ready(Controller& controller,std::uint32_t graphics_flags) {
    std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(17));
    if(graphics_flags&0x60u)return true;
    for(auto* file:controller.files)if(file) {
        if(file->fields_5c[1]||std::atomic_ref<std::uint32_t>(file->fields_5c[0]).load())return false;
    }
    return true;
}
}
