#include "animation_file.hpp"
#include "../archive/resource_manager.hpp"
#include "file_text.hpp"
#include <cstring>
#include <limits>
#include <new>
#include <stdexcept>
namespace th20::source::sprite {
namespace {
std::uint8_t* copy_bytes(const th20::source::Bytes& bytes) {
    auto* memory=static_cast<std::uint8_t*>(runtime::allocate_bytes(bytes.size()));
    if(!memory)throw std::bad_alloc();std::memcpy(memory,bytes.data(),bytes.size());return memory;
}
}
AnmCounts count_animation_entries(std::span<const std::uint8_t> data) {
    AnmCounts result;std::size_t offset=0;
    for(;;) {
        if(offset>data.size()||data.size()-offset<sizeof(AnmHeader))throw std::runtime_error("Truncated ANM header");
        const auto* header=reinterpret_cast<const AnmHeader*>(data.data()+offset);
        ++result.textures;result.sprites+=header->sprite_count;result.scripts+=header->script_count;
        if(!header->next_offset)break;
        if(header->next_offset<sizeof(AnmHeader)||header->next_offset>data.size()-offset)throw std::runtime_error("Invalid ANM entry link");
        offset+=header->next_offset;
    }
    return result;
}
AnimationFile* create_animation_file() {
    std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(1));
    auto* storage=::operator new(sizeof(AnimationFile));
    std::memset(storage,0,sizeof(AnimationFile)); // original 4477b0 zeroes before 448a30
    return new(storage) AnimationFile{};
}
int preload_external_texture(AnimationFile& file,std::uint32_t index,AnmHeader* header,runtime::Log& log) {
    if(!header){runtime::log_error(log,file_text::s0056e428);return -1;}
    if(header->version!=8){runtime::log_error(log,file_text::s0056e464);return -1;}
    auto* name=reinterpret_cast<const char*>(header)+header->name_offset;
    if(!header->has_data&&name[0]!='@') {
        auto bytes=resources::read(name,true);
        if(!bytes){runtime::log_error(log,file_text::s0056e488,name);return -1;}
        file.textures[index].unknown_08=static_cast<std::uint32_t>(bytes->size());
        file.textures[index].unknown_04=reinterpret_cast<std::uint32_t>(copy_bytes(*bytes));
    }
    return 1; // Original 0x44ed2b returns ONE on every successful path.
}
int preload_animation_data(AnimationFile& file,const char* name,runtime::Log& log) {
    auto bytes=resources::read(name,false);if(!bytes)return -1;
    const auto counts=count_animation_entries(*bytes);
    file.bytes=copy_bytes(*bytes);
    const std::filesystem::path path(name);
    file.filename=path.filename().string();file.stem=path.stem().string();
    file.texture_count=static_cast<std::int32_t>(counts.textures);
    {
        std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(1));
        file.textures=new TextureRecord[counts.textures]{};
        file.sprites=new SpriteData[counts.sprites]{};
        file.scripts=new AnmInstruction*[counts.scripts];
    }
    file.script_count=counts.scripts;file.sprite_count=counts.sprites;
    auto* header=reinterpret_cast<AnmHeader*>(file.bytes);std::uint32_t index=0;
    for(;;) {
        if(preload_external_texture(file,index,header,log)<0)return 0;
        // This surprising zero on an external-texture error is the actual
        // 0x44aa46..68 result. Templates remain null on that path.
        ++index;if(!header->next_offset)break;
        header=reinterpret_cast<AnmHeader*>(reinterpret_cast<std::uint8_t*>(header)+header->next_offset);
    }
    {
        std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(1));
        file.templates=new Animation[counts.scripts];
        for(std::uint32_t i=0;i<counts.scripts;++i)construct_animation(file.templates[i]);
    }
    return 0;
}
void release_texture_data(TextureRecord& record) {
#ifdef TH_SDL3
    if(record.texture) web::shared_device().release_texture(record.texture);
#else
    if(record.texture){record.texture->Release();record.texture=nullptr;}
#endif
    if(record.unknown_04){runtime::release_bytes(reinterpret_cast<void*>(record.unknown_04));record.unknown_04=0;}
}
}
