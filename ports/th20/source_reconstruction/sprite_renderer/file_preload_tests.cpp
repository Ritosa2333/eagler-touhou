#include "animation_file.hpp"
#include "../archive/resource_manager.hpp"
#include <fstream>
#include <iostream>
#include <regex>
#include <stdexcept>
namespace s=th20::source::sprite;
namespace r=th20::source::resources;
namespace {
unsigned checks;
void check(bool value,const char* why){++checks;if(!value)throw std::runtime_error(why);}
std::string text(const std::filesystem::path& path){std::ifstream in(path,std::ios::binary);if(!in)throw std::runtime_error("Reference DSL missing");return {std::istreambuf_iterator<char>(in),{}};}
unsigned matches(const std::string& text,const std::regex& pattern){return static_cast<unsigned>(std::distance(std::sregex_iterator(text.begin(),text.end(),pattern),std::sregex_iterator()));}
// This fixture frees only preloaded, unexecuted templates. It deliberately does
// not stand in for production clear_animation_file, which also retires live VMs.
struct PreloadFixture {
    s::AnimationFile* file=s::create_animation_file();
    ~PreloadFixture(){for(int i=0;i<file->texture_count;++i)s::release_texture_data(file->textures[i]);delete[] file->textures;delete[] file->sprites;delete[] file->scripts;delete[] file->templates;th20::source::runtime::release_bytes(file->bytes);delete file;}
};
}
int wmain(int argc,wchar_t** argv){
    try {
        if(argc!=4)throw std::runtime_error("Usage: file_preload_tests ARCHIVE RECOVERED_ANM_DSL REPORT.json");
        check(r::open(argv[1]),"Open original archive");
        th20::source::runtime::Log log;unsigned files=0,entries=0,sprites=0,scripts=0;
        const std::regex entry_pattern(R"((^|\n)entry entry[0-9]+ \{)"),script_pattern(R"((^|\n)script (-?[0-9]+ )?script[0-9]+ \{)"),sprite_pattern(R"(\bsprite[0-9]+\s*:)" );
        for(auto& item:std::filesystem::directory_iterator(argv[2])) {
            if(item.path().extension()!=".txt")continue;
            const auto name=item.path().stem().string();const auto dsl=text(item.path());
            const auto expected_entries=matches(dsl,entry_pattern),expected_scripts=matches(dsl,script_pattern),expected_sprites=matches(dsl,sprite_pattern);
            check(expected_entries>0,"Reference DSL has no entries");
            PreloadFixture fixture;auto& file=*fixture.file;
            check(s::preload_animation_data(file,name.c_str(),log)==0,"Preload real ANM failed");
            check(log.error==0,"ANM preload logged an error");
            check(static_cast<unsigned>(file.texture_count)==expected_entries&&file.sprite_count==expected_sprites&&file.script_count==expected_scripts,"Preloaded counts differ from independently recovered DSL");
            check(file.templates&&file.textures&&file.sprites&&file.scripts,"Preload arrays missing");
            check(file.filename==name.c_str()&&file.stem==item.path().stem().stem().string().c_str(),"Filesystem filename/stem");
            auto raw=r::read(name.c_str());check(raw&&std::memcmp(file.bytes,raw->data(),raw->size())==0,"Preload must retain writable identical ANM data");
            for(unsigned i=0;i<file.script_count;++i){bool self=true;for(auto& link:file.templates[i].links)self&=link.value==file.templates+i;check(self,"Template self-links");}
            ++files;entries+=expected_entries;sprites+=expected_sprites;scripts+=expected_scripts;
        }
        check(files==73,"Expected 73 unique ANM files");r::close();
        std::ofstream report(argv[3]);report<<"{\n  \"status\":\"passed\",\n  \"checks\":"<<checks<<",\n  \"files\":"<<files<<",\n  \"entries\":"<<entries<<",\n  \"sprites\":"<<sprites<<",\n  \"scripts\":"<<scripts<<",\n  \"animation_file_cpp_sha256\":\""<<TH20_FILE_CPP_SHA<<"\",\n  \"scope\":\"All real ANM preloads; writable archive bytes, counts compared against independently recovered DSL, real allocations and self-linked templates\",\n  \"limitations\":[\"Source integration test, not original-CPU preloader ABI comparison\",\"No Direct3D postload or live-animation teardown exercised\"]\n}\n";
        if(!report)throw std::runtime_error("Could not write preload report");std::cout<<"ANM preload: "<<checks<<" checks / "<<files<<" files, "<<entries<<" entries, "<<sprites<<" sprites, "<<scripts<<" scripts\n";return 0;
    }catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
}
