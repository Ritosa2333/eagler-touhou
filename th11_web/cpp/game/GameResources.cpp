#include "GameResources.hpp"
#include "PracticePatcher.hpp"
#include <functional>
#include <algorithm>
#include <cstdio>
#if defined(TH_NATIVE_PLATFORM) && defined(TH_ENABLE_THCRAP)
#include <SDL3/SDL.h>
#endif

namespace th11 {

bool GameResources::fail(const char* message) {
    last_error = message ? message : "resource error";
    return false;
}

bool GameResources::open_archive(const u8* bytes, u32 size) {
    lookup.clear(); animations = {}; last_error.clear();
    if (!archive.open(bytes, size)) return fail("invalid THA1 archive");
    for (u32 i=0; i<archive.entries.size(); ++i) {
        const auto& entry = archive.entries[i];
        if (entry.name.empty() || lookup.find(entry.name) != lookup.end()) {
            archive.entries.clear(); lookup.clear();
            return fail("duplicate or empty archive resource");
        }
        lookup.emplace(entry.name, i);
    }
    return true;
}

bool GameResources::has(const std::string& name) const noexcept {
    return lookup.find(name) != lookup.end();
}

bool GameResources::read(const std::string& name, std::vector<u8>& data) {
    const auto auxiliary=practice_auxiliary.find(name);
    if(auxiliary!=practice_auxiliary.end()){data=auxiliary->second;return true;}
    const auto found = lookup.find(name);
    if (found == lookup.end()) return fail("missing archive resource");
#if defined(TH_NATIVE_PLATFORM) && defined(TH_ENABLE_THCRAP)
    // Same boundary as TH10 ResourceFiles: a prepared /thcrap/th11/<entry>
    // overrides one original archive entry; absent files use the retail DAT.
    if (!name.empty() && name.front() != '/' && name.find("..") == std::string::npos &&
        name.find('\\') == std::string::npos && name.find(':') == std::string::npos) {
        const std::string path = "/thcrap/th11/" + name;
        size_t length = 0;
        void* bytes = SDL_LoadFile(path.c_str(), &length);
        if (bytes) {
            if (length > 64u * 1024u * 1024u) { SDL_free(bytes); return fail("oversized THCRAP override"); }
            const auto* begin = static_cast<const u8*>(bytes);
            data.assign(begin, begin + length);
            SDL_free(bytes);
            return true;
        }
    }
#endif
    if (!archive.read(found->second, data)) return fail("resource decode failed");
    return true;
}

bool GameResources::read_ecl(const std::string& name,EclResource& file){
    const auto found=practice_ecl.find(name);
    if(found==practice_ecl.end()){std::vector<u8> data;return read(name,data)&&file.open(data.data(),u32(data.size()));}
    const auto& resource=found->second;
    if(!file.open(resource.original.data(),u32(resource.original.size())))return fail("invalid practice source ECL");
    // The patcher already proved that directories, names and ECLH headers
    // remain byte-identical. Bind pointers only after installing instructions.
    file.bytes=resource.patched;
    return true;
}

bool GameResources::load_practice_stage(u32 stage,StageResources& output,const PracticeConfig& config){
    if(!config.valid()||config.stage!=i32(stage)-1)return fail("practice stage/config mismatch");
    if(config.mode!=1||!config.section)return load_stage(stage,output);
    practice_ecl.clear();practice_auxiliary.clear();
    struct Clear {GameResources& owner;~Clear(){owner.practice_ecl.clear();owner.practice_auxiliary.clear();owner.practice_scene_original.clear();}} clear{*this};
    PracticeBuffers buffers;std::vector<std::string> names;
    std::function<bool(const std::string&)> collect=[&](const std::string& name){
        if(names.size()>=32||std::find(names.begin(),names.end(),name)!=names.end())return fail("cyclic or excessive practice ECL includes");
        std::vector<u8> data;EclResource file;
        if(!read(name,data)||!file.open(data.data(),u32(data.size())))return fail("invalid practice source ECL");
        names.push_back(name);buffers.ecl.push_back(std::move(data));
        for(const auto& include:file.includes)if(!collect(include))return false;
        return true;
    };
    char name[40];std::snprintf(name,sizeof(name),"stage%02u.ecl",stage);
    if(!collect(name))return false;
    std::snprintf(name,sizeof(name),"stage%02u.std",stage);const std::string scene_name=name;
    if(!read(name,buffers.scene))return false;
    practice_scene_original=buffers.scene;
    std::snprintf(name,sizeof(name),"stage%02u.anm",stage);const std::string background_name=name;
    if(!read(name,buffers.background))return false;
    const auto originals=buffers.ecl;std::string error;
    if(!patch_practice_buffers(buffers,config,error)){last_error=error;return false;}
    for(size_t i=0;i<names.size();++i)practice_ecl.emplace(names[i],PracticeFile{originals[i],std::move(buffers.ecl[i])});
    practice_auxiliary.emplace(scene_name,std::move(buffers.scene));
    practice_auxiliary.emplace(background_name,std::move(buffers.background));
    if(!load_stage(stage,output))return false;
    output.practice_stage_section=buffers.stage_section;
    return true;
}

bool GameResources::open_anm(const std::string& name, AnmResource& output) {
    std::vector<u8> data;
    if (!read(name, data) || !output.open(data.data(), u32(data.size())))
        return fail("invalid ANM resource");
    return true;
}

bool GameResources::open_sht(const std::string& name, ShtResource& output) {
    std::vector<u8> data;
    if (!read(name, data) || !output.open(data.data(), u32(data.size())))
        return fail("invalid SHT resource");
    return true;
}

bool GameResources::load_ecl(const std::string& name, EclProgram& output) {
    output.files.clear(); output.definitions.clear(); output.error.clear();
    if (!output.load(name, *this)) {
        last_error = output.error.empty() ? "invalid ECL resource" : output.error;
        return false;
    }
    return true;
}

bool GameResources::load_stage(u32 stage, StageResources& output) {
    if (stage < 1 || stage > 7) return fail("stage outside TH11 range");
    char name[64];
    std::vector<u8> scene;
    std::snprintf(name, sizeof name, "stage%02u.std", stage);
    if (!read(name, scene))return false;
    const bool scene_ok=practice_scene_original.empty()?output.scene.open(scene.data(),u32(scene.size())):output.scene.open_practice(practice_scene_original,scene);
    if (!scene_ok) {last_error="invalid STD resource: "+output.scene.error;return false;}
    std::snprintf(name, sizeof name, "stage%02u.anm", stage);
    if (!open_anm(name, output.background)) return false;
    std::snprintf(name, sizeof name, "st%02ulogo.anm", stage);
    if (!open_anm(name, output.logo)) return false;
    std::snprintf(name, sizeof name, "stgenm%02u.anm", stage);
    if (!open_anm(name, output.enemies)) return false;
    std::snprintf(name, sizeof name, "stage%02u.ecl", stage);
    if (!load_ecl(name, output.timeline)) return false;
    for (u32 i=0; i<output.messages.size(); ++i) {
        std::snprintf(name, sizeof name, "st%02u_%02u%c.msg", stage, i/3, char('a'+i%3));
        if (!read(name, output.messages[i])) return false;
    }
    output.boss_programs.clear();
    for (const char* suffix : {"boss", "mboss"}) {
        std::snprintf(name, sizeof name, "stage%02u%s.ecl", stage, suffix);
        if (!has(name)) continue;
        output.boss_programs.emplace_back();
        if (!load_ecl(name, output.boss_programs.back())) return false;
    }
    return true;
}

bool GameResources::animation(u32 slot, const std::string& name) {
    if (slot >= animations.size() || !has(name)) return fail("missing ECL animation");
    animations[slot] = name;
    return true;
}

const std::string& GameResources::animation_name(u32 slot) const noexcept {
    static const std::string empty;
    return slot < animations.size() ? animations[slot] : empty;
}

}
