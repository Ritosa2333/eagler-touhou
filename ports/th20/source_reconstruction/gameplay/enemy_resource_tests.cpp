#include "enemy.hpp"
#include "script_loader.hpp"
#include "enemy_source_hashes.hpp"
#include "../ecl_vm/vm.hpp"
#include "../archive/resource_manager.hpp"
#include "../../include/th20/binary.hpp"
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <memory>
#include <map>
#include <set>
#include <cstring>
#include <functional>
namespace gp=th20::source::gameplay;
namespace s=th20::source::sprite;
namespace q=th20::source::scheduler;
namespace rt=th20::source::runtime;
namespace {
std::unique_ptr<th20::source::Archive> archive;
std::vector<std::string> reads;
std::vector<std::pair<int,std::string>> animation_loads;
std::map<s::AnimationFile*,std::unique_ptr<s::AnimationFile>> animation_owners;
std::vector<unsigned> unloads;
}
namespace th20::source::resources {
std::optional<Bytes> read(const char* name,bool loose) {
    if(loose)throw std::logic_error("Unexpected loose resource read");
    reads.emplace_back(name);return archive->read(name);
}
}
namespace th20::source::sprite {
AnimationFile* load_animation_file(Controller& controller,std::int32_t slot,const char* path,runtime::Log&,std::uint32_t&) {
    // Actual archive/ANM existence is checked. GPU texture allocation is a
    // recorded required boundary for this resource-order/lifecycle test only.
    const auto bytes=archive->read(path);if(bytes.size()<64)throw std::runtime_error("Invalid test ANM resource");
    auto file=std::make_unique<AnimationFile>();file->id=slot;file->filename=path;
    auto* result=file.get();animation_owners.emplace(result,std::move(file));controller.files[slot]=result;
    animation_loads.emplace_back(slot,path);return result;
}
void mark_file_animations(Controller&,AnimationFile*,bool preserve) {if(preserve)throw std::logic_error("Unexpected preserve flag");}
void unload_animation_file(Controller& controller,std::int32_t slot) {
    unloads.push_back(slot);auto* file=controller.files[slot];controller.files[slot]=nullptr;if(file)animation_owners.erase(file);
}
}
struct Environment:gp::EnemyServices {
    rt::Log log_data;q::State chain{};q::Environment chain_environment;
    s::Controller* controller=static_cast<s::Controller*>(VirtualAlloc(nullptr,sizeof(s::Controller),MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));
    std::uint32_t events=0;
    Environment(){if(!controller)throw std::bad_alloc();q::initialize_state(chain);}
    ~Environment(){VirtualFree(controller,0,MEM_RELEASE);}
    rt::Log& log() override{return log_data;}
    q::State& scheduler_state() override{return chain;}
    q::Environment& scheduler_environment() override{return chain_environment;}
    s::Controller& sprites() override{return *controller;}
    std::uint32_t& graphics_flags() override{return events;}
    int update_enemy(gp::EnemyController&) override{throw std::logic_error("Enemy simulation excluded from resource fixture");}
    void draw_enemy_overlay() override{throw std::logic_error("Enemy overlay excluded from resource fixture");}
    void select_layer(int,int) override{throw std::logic_error("Drawing excluded from resource fixture");}
    void retire_entity(void* entity) override {
        // Frame-test entities actually derive CallbackOwner. Production Enemy
        // has its own ECL-manager base and requires its separately recovered destructor.
        rt::retire_callback_owner(static_cast<rt::CallbackOwner*>(entity));
    }
    s::AnimationFile* existing_animation(gp::EnemyController& enemy,unsigned slot) override {
        // All shipped stage include blocks exercised here use2..6. Slot7's
        // real background dependency must not silently receive a fake value.
        if(slot<2||slot>=7)throw std::logic_error("Unexpected slot7 background dependency");
        return enemy.animation_files[slot];
    }
};
int wmain(int argc,wchar_t** argv) {
    unsigned assertions=0;std::map<std::string,unsigned> subroutine_counts;
    try {
        if(argc!=3)throw std::runtime_error("Usage: enemy_resource_tests th20.dat REPORT.json");
        archive=std::make_unique<th20::source::Archive>(std::filesystem::path(argv[1]));
        const auto check=[&](bool condition,const char* reason){++assertions;if(!condition)throw std::runtime_error(reason);};
        // Table row0 names st00.ecl, which is absent from the shipped archive.
        // Playable stage files are1..7; no synthetic st00 fallback is supplied.
        for(int stage=1;stage<8;++stage) {
            Environment environment;reads.clear();animation_loads.clear();unloads.clear();gp::clear_script_cache();
            const std::string name="st0"+std::to_string(stage)+".ecl";
            auto* first=gp::create_enemy_controller(environment,0,name.c_str());
            check(first!=nullptr&&first->context==&th20::source::game_session::context(0),"Wrong owner/context");
            check(first->data.field_88==99999&&first->update_node->priority==0x24&&first->draw_node->priority==0x17,"Initialization fields/callbacks differ");
            check(!(first->update_node->flags&2)&&!(first->draw_node->flags&2),"Original callbacks must start disabled");
            std::set<std::string> seen;std::vector<std::pair<std::string,std::vector<std::uint8_t>>> expected;
            std::function<void(const std::string&)> visit=[&](const std::string& file) {
                if(!seen.insert(file).second)return;
                auto document=th20::parse_ecl(archive->read(file));
                for(const auto& sub:document.subroutines)expected.push_back({sub.name,std::vector<std::uint8_t>(document.original.begin()+sub.file_offset,document.original.begin()+sub.end_offset)});
                for(const auto& included:document.includes)visit(included);
            };
            visit(name);std::stable_sort(expected.begin(),expected.end(),[](const auto& a,const auto& b){return a.first<b.first;});
            check(first->loader->records.size()==expected.size(),"SCPT closure subroutine count differs");
            check(first->loader->file_count==seen.size()&&gp::script_cache_size()==seen.size(),"SCPT cache file count differs");
            check(reads.size()==seen.size(),"Repeated resource read before second player");
            for(std::size_t i=0;i<expected.size();++i) {
                check(expected[i].first==first->loader->records[i].name,"Sorted SCPT name differs");
                check(std::memcmp(expected[i].second.data(),first->loader->records[i].header,expected[i].second.size())==0,"SCPT instruction bytes differ");
            }
            subroutine_counts[name]=static_cast<unsigned>(expected.size());
            th20::source::ecl::Program first_program;first->loader->bind_program(first_program);
            check(first_program.subroutines.size()==first->loader->records.size(),"VM view does not contain full SCPT closure");
            for(std::size_t i=0;i<first_program.subroutines.size();++i)
                check(first_program.subroutines[i].data()==first->loader->records[i].header+16&&first_program.subroutines[i].bytes.empty(),"VM script view must borrow actual cache bytes");
            const auto read_count=reads.size(),record_count=first->loader->records.size(),file_count=first->loader->file_count;
            first->loader->load(name.c_str());
            check(reads.size()==read_count&&first->loader->records.size()==record_count&&first->loader->file_count==file_count,"Per-player duplicate suppression failed");
            auto* second=gp::create_enemy_controller(environment,1,name.c_str());
            check(reads.size()==read_count&&second->loader->records.size()==record_count,"Cross-player cache reuse failed");
            check(std::memcmp(first->loader->records.data(),second->loader->records.data(),record_count*sizeof(gp::ScriptRecord))==0,"Cache must share actual mutable script/name pointers");
            th20::source::ecl::Program second_program;second->loader->bind_program(second_program);
            auto* shared_byte=first_program.subroutines[0].data()+12;const auto previous=*shared_byte;*shared_byte^=0x5a;
            check(second_program.subroutines[0].data()[12]==*shared_byte&&second_program.subroutines[0].data()+12==shared_byte,"Cross-player VM writes must share original cache storage");
            *shared_byte=previous;
            for(const auto& request:animation_loads)check(request.first>=25&&request.first<42,"ANM slot out of controller range");
            first->data.handles_44[2]=17;first->field_cc=9;first->data.fields_30[0]=8;first->data.fields_30[1]=7;
            first->clear_entities();check(first->loaded_names.empty()&&first->field_cc==0&&first->data.handles_44[2]==0,"Retained-stage cleanup failed");
            first->reset_for_stage();check(first->enemies.sentinel.next==nullptr&&first->enemies.tail==&first->enemies.sentinel,"Stage list reset failed");
            gp::destroy_enemy_controller(0);gp::destroy_enemy_controller(1);
            check(gp::script_cache_size()==0&&environment.chain.update.sentinel.next==nullptr&&environment.chain.draw.sentinel.next==nullptr,"Teardown did not clear real source owners/callbacks/cache");
            check(th20::source::game_session::context(0).objects_04[1]==nullptr&&th20::source::game_session::context(1).objects_04[1]==nullptr,"Context owners remained after teardown");
            check(unloads.size()==16,"Destructor ANM slot release count differs");
        }
        std::ofstream report(argv[2]);report<<"{\"status\":\"passed\",\"assertions\":"<<assertions<<",\"stage_subroutines\":{";bool first=true;
        for(auto& [name,count]:subroutine_counts){if(!first)report<<',';first=false;report<<th20::json_string(name)<<':'<<count;}
        report<<"},\"source_sha256\":{";first=true;
        for(auto& source:enemy_source_hashes){if(!first)report<<',';first=false;report<<th20::json_string(source.path)<<':'<<th20::json_string(source.sha);}
        report<<"},\"scope\":\"Actual encrypted archive data and source ECL cache/parser/EnemyController lifecycle. GPU allocation is a recorded required boundary; enemy simulation and rendered gameplay are excluded.\"}\n";
        std::cout<<assertions<<" actual-archive EnemyController/ECL resource assertions passed\n";return 0;
    }catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
}
