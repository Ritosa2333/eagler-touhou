// Original executable mappings are used only by this isolated verification tool.
#define wmain unused_native_cpu_main
#include "../../../tests/native_cpu_compare.cpp"
#undef wmain
#include "../records.hpp"
#include "../compression.hpp"
#include "../file_codec.hpp"
#include "../../runtime_core/runtime_core.hpp"
#include "../manager.hpp"
#include "../../stage_completion/progress.hpp"
#include "../../stage_completion/playtime.hpp"
#include "../../program_entry/program_entry.hpp"
#include "../../game_session/session.hpp"
#include "../../archive/resource_manager.hpp"
#include "../../title_system/player_data.hpp"
#include <locale.h>
namespace p=th20::source::progress;namespace state=th20::source::state;
namespace pe=th20::source::program_entry;namespace gs=th20::source::game_session;
namespace th20::source::program_entry {WindowStatePrefix window_state{};runtime::Log log_buffer;}
template<class R,class... A>R cpu(std::uint32_t va,void* self,A... args){using F=R(__thiscall*)(void*,A...);return reinterpret_cast<F>(mapped_image_base+va-0x400000)(self,args...);}
namespace {
double clock_samples[2];unsigned clock_index=0;double __fastcall clock_boundary(void*,void*){return clock_samples[clock_index++];}
th20::source::Bytes saved_file;std::map<std::string,th20::source::Bytes> saved_files;std::string saved_path;unsigned write_calls=0,close_calls=0;
int __cdecl begin_file(const char* name){saved_path=name;saved_file.clear();write_calls=close_calls=0;return 0;}
int __cdecl write_file(const void* data,std::uint32_t size){auto* bytes=static_cast<const std::uint8_t*>(data);saved_file.insert(saved_file.end(),bytes,bytes+size);++write_calls;return 0;}
int __cdecl close_file(){saved_files[saved_path]=saved_file;++close_calls;return 0;}
void intercept(std::uint32_t va,void* function){auto* at=reinterpret_cast<std::uint8_t*>(mapped_image_base+va-0x400000);at[0]=0xe9;const auto delta=reinterpret_cast<std::uint32_t>(function)-reinterpret_cast<std::uint32_t>(at)-5;std::memcpy(at+1,&delta,4);FlushInstructionCache(GetCurrentProcess(),at,5);}
}
int wmain(int argc,wchar_t** argv){try{
    AddVectoredExceptionHandler(1,[](PEXCEPTION_POINTERS p)->LONG{std::cerr<<"exception "<<std::hex<<p->ExceptionRecord->ExceptionCode<<" originalVA="<<(p->ContextRecord->Eip-mapped_image_base+0x400000)<<" address="<<p->ExceptionRecord->ExceptionInformation[1]<<" esp="<<p->ContextRecord->Esp<<std::endl;for(unsigned i=0;i<80;++i){auto value=reinterpret_cast<std::uint32_t*>(p->ContextRecord->Esp)[i];if(value>=mapped_image_base&&value<mapped_image_base+0x200000)std::cerr<<" stack+"<<i*4<<" VA="<<value-mapped_image_base+0x400000;}std::cerr<<std::dec<<std::endl;return EXCEPTION_CONTINUE_SEARCH;});
    if(argc!=3)throw std::runtime_error("Usage: th20_progress_cpu_compare ORIGINAL.exe REPORT.json");
    const auto bytes=th20::read_file(argv[1]);if(sha256(bytes)!=expected_sha)throw std::runtime_error("Original SHA mismatch");const auto pe=th20::parse_pe(bytes);Mapping image(bytes,pe);mapped_image_base=image.address();
    for(const auto& entry:pe.imports)if(auto address=GetProcAddress(GetModuleHandleW(L"kernel32.dll"),entry.name.c_str()))*reinterpret_cast<FARPROC*>(mapped_image_base+entry.iat_rva)=address;
    *reinterpret_cast<HANDLE*>(mapped_image_base+0x1e5990)=GetProcessHeap();cpu<void>(0x452e00,reinterpret_cast<void*>(mapped_image_base+0x1c0240));
    // The original CRT entry point is not run. Bind its locale query to the
    // test process's actual CRT locale rather than fabricating original TLS.
    intercept(0x55198d,reinterpret_cast<void*>(&___lc_codepage_func));
    std::mt19937 random(0x50fce0);unsigned passed=0,failed=0;std::vector<std::string> failures;
    auto check=[&](const char* name,unsigned test,const void* a,const void* b,std::size_t size){if(!std::memcmp(a,b,size)){++passed;return;}++failed;if(failures.size()<30){std::ostringstream out;out<<name<<" test="<<test;for(std::size_t i=0;i<size;++i)if(static_cast<const unsigned char*>(a)[i]!=static_cast<const unsigned char*>(b)[i]){out<<" +0x"<<std::hex<<i<<" expected="<<unsigned(static_cast<const unsigned char*>(a)[i])<<" actual="<<unsigned(static_cast<const unsigned char*>(b)[i]);break;}failures.push_back(out.str());}};
    auto fill=[&](void* pointer,std::size_t size,unsigned test){auto* output=static_cast<std::uint8_t*>(pointer);for(std::size_t i=0;i<size;++i)output[i]=test%3==0?0:test%3==1?0xa5:static_cast<std::uint8_t>(random());};
    for(unsigned test=0;test<512;++test){p::Profile original,source;fill(&original,sizeof(original),test);source=original;cpu<void>(0x50af20,&original);p::construct_profile(source);check("profile_constructor",test,&original,&source,sizeof(source));
        cpu<void>(0x50ef00,&original);p::initialize_profile(source);check("profile_defaults",test,&original,&source,sizeof(source));
        const auto expected=cpu<std::uint32_t>(0x50fc90,&original,int(sizeof(original)));const auto actual=p::record_checksum(source.bytes);check("profile_record_checksum",test,&expected,&actual,4);
    }
    for(unsigned test=0;test<1024;++test){p::Metadata original,source;fill(&original,sizeof(original),test);source=original;cpu<void>(0x50e6e0,&original);p::construct_metadata(source);check("metadata_constructor",test,&original,&source,sizeof(source));
        state::Random stream(1);state::seed(stream,random());auto* original_stream=reinterpret_cast<state::Random*>(mapped_image_base+0x1ba4c4);*original_stream=stream;
        cpu<void>(0x50f090,&original);p::initialize_metadata(source,stream);check("metadata_defaults",test,&original,&source,sizeof(source));check("metadata_defaults_rng",test,original_stream,&stream,sizeof(stream));
        const auto expected=cpu<std::uint8_t>(0x463f20,&original);const auto actual=p::metadata_checksum(source);check("metadata_checksum",test,&expected,&actual,1);
        cpu<void>(0x4beb60,&original);p::update_metadata_checksum(source,stream);check("metadata_update",test,&original,&source,sizeof(source));check("metadata_update_rng",test,original_stream,&stream,sizeof(stream));
    }
    auto original=std::make_unique<p::Snapshot>(),source=std::make_unique<p::Snapshot>();
    for(unsigned test=0;test<64;++test){fill(original.get(),sizeof(*original),test);std::memcpy(source.get(),original.get(),sizeof(*source));cpu<void>(0x50e540,original.get());::new(static_cast<void*>(source.get()))p::Snapshot;check("snapshot_constructor",test,original.get(),source.get(),sizeof(*source));}
    for(unsigned test=0;test<256;++test){const int character=int(random()%5)-1,index=int(random()%13)-2;const auto* expected=cpu<p::Profile*>(0x4bd460,original.get(),character,index);const auto* actual=p::find_profile(*original,character,index);check("profile_selection_pointer",test,&expected,&actual,4);}
    for(unsigned test=0;test<2048;++test){th20::source::Bytes input(test<64?test:random()%15000+1);for(unsigned i=0;i<input.size();++i)input[i]=static_cast<std::uint8_t>(test%4==0?0:test%4==1?i%31:test%4==2?random()%6:random());
        int compressed_size=-1;using Compress=std::uint8_t*(__cdecl*)(const void*,int,int*);auto* compressed=reinterpret_cast<Compress>(mapped_image_base+0x139550)(input.data(),int(input.size()),&compressed_size);
        std::array<std::uint8_t,8192> dictionary;dictionary.fill(0xa5);p::LzssEncoder encoder(dictionary);const auto output=encoder.encode(input);const auto actual_size=int(output.size());
        check("lzss_compressed_size",test,&compressed_size,&actual_size,4);if(compressed_size==actual_size)check("lzss_compressed_bytes",test,compressed,output.data(),output.size());
        check("lzss_final_dictionary",test,reinterpret_cast<void*>(mapped_image_base+0x1c6b38),dictionary.data(),dictionary.size());check("lzss_final_tree",test,reinterpret_cast<void*>(mapped_image_base+0x1c8b38),encoder.tree().data(),sizeof(encoder.tree()));
        if(compressed)HeapFree(GetProcessHeap(),0,compressed);
        th20::source::LzssDecoder decoder;const auto decoded=decoder.decode(output,std::uint32_t(input.size()));check("lzss_roundtrip",test,input.data(),decoded.data(),input.size());
    }
    for(unsigned test=0;test<4096;++test){th20::source::Bytes expected(test<64?test:random()%4096),actual;for(auto& byte:expected)byte=static_cast<std::uint8_t>(random());actual=expected;
        th20::source::CryptParameters parameters{static_cast<std::uint8_t>(random()),static_cast<std::uint8_t>(random()),std::uint32_t(4u<<(test%7)),static_cast<std::uint32_t>(test%2?expected.size():0x100)};
        if(parameters.limit<expected.size())parameters.limit=parameters.limit/parameters.block*parameters.block;
        using Crypt=void*(__cdecl*)(void*,unsigned,std::uint8_t,std::uint8_t,int,unsigned);reinterpret_cast<Crypt>(mapped_image_base+0x103c0)(expected.data(),unsigned(expected.size()),parameters.key,parameters.step,int(parameters.block),parameters.limit);p::encrypt(actual,parameters);check("encryption",test,expected.data(),actual.data(),actual.size());
    }
    std::cerr<<"full save serialization tests\n";
    intercept(0x410840,reinterpret_cast<void*>(&begin_file));intercept(0x411060,reinterpret_cast<void*>(&write_file));intercept(0x4106e0,reinterpret_cast<void*>(&close_file));
    strcpy_s(reinterpret_cast<char*>(mapped_image_base+0x1b67e1),260,"D:\\source_oracle_fixture");
    std::array<std::uint8_t,8192> dictionary;
    p::Encode encode=[&](const th20::source::Bytes& bytes){p::LzssEncoder encoder(dictionary);return encoder.encode(bytes);};
    for(unsigned test=0;test<80;++test){
        std::memset(original.get(),0,sizeof(*original));::new(static_cast<void*>(original.get()))p::Snapshot;
        state::Random stream(1);state::seed(stream,random());for(auto& record:original->profiles)p::initialize_profile(record);p::initialize_metadata(original->metadata,stream);
        p::parse_snapshot(*original,[](const th20::source::Bytes&,std::uint32_t)->th20::source::Bytes{throw std::logic_error("Unexpected decoder on missing score");});
        for(unsigned i=0;i<2000;++i){auto& record=original->profiles[random()%19];record.bytes[0x100+random()%0x7800]=static_cast<std::uint8_t>(test%3==0?random():0);}
        if(test%5==0)for(unsigned i=0;i<18;i+=3)p::write<std::uint16_t>(original->profiles[i].bytes,0,0);
        std::memcpy(source.get(),original.get(),sizeof(*source));source->file_buffer=static_cast<std::uint8_t*>(th20::source::runtime::allocate_bytes(44));std::memcpy(source->file_buffer,original->file_buffer,44);
        using Save=void(__stdcall*)(const char*,p::Snapshot*);reinterpret_cast<Save>(mapped_image_base+0x10f6b0)("scoreth20.dat",original.get());const auto expected=saved_file;
        const auto actual=p::serialize_snapshot(*source,encode);const auto expected_size=expected.size(),actual_size=actual.size();check("serialized_size",test,&expected_size,&actual_size,4);if(expected_size==actual_size)check("serialized_file",test,expected.data(),actual.data(),expected.size());
        check("serialize_mutated_profiles",test,original->profiles,source->profiles,sizeof(source->profiles));check("serialize_mutated_metadata",test,&original->metadata,&source->metadata,sizeof(source->metadata));check("serialize_mutated_header",test,original->file_buffer,source->file_buffer,44);
        check("serialize_dictionary",test,reinterpret_cast<void*>(mapped_image_base+0x1c6b38),dictionary.data(),dictionary.size());unsigned writes=2,closes=1;check("original_file_writes",test,&writes,&write_calls,4);check("original_file_close",test,&closes,&close_calls,4);
        p::release_snapshot_buffers(*original);p::release_snapshot_buffers(*source);
        std::memset(original.get(),0,sizeof(*original));::new(static_cast<void*>(original.get()))p::Snapshot;std::memset(source.get(),0,sizeof(*source));::new(static_cast<void*>(source.get()))p::Snapshot;
        original->file_size=source->file_size=static_cast<std::uint32_t>(expected.size());original->file_buffer=static_cast<std::uint8_t*>(HeapAlloc(GetProcessHeap(),0,expected.size()));source->file_buffer=static_cast<std::uint8_t*>(th20::source::runtime::allocate_bytes(expected.size()));std::memcpy(original->file_buffer,expected.data(),expected.size());std::memcpy(source->file_buffer,expected.data(),expected.size());
        using Parse=int(__stdcall*)(p::Snapshot*);const auto original_result=reinterpret_cast<Parse>(mapped_image_base+0x10eb70)(original.get());th20::source::LzssDecoder decoder;const auto source_result=p::parse_snapshot(*source,[&](const th20::source::Bytes& bytes,std::uint32_t size){return decoder.decode(bytes,size);});
        check("parse_return",test,&original_result,&source_result,4);check("parse_profiles",test,original->profiles,source->profiles,sizeof(source->profiles));check("parse_metadata",test,&original->metadata,&source->metadata,sizeof(source->metadata));check("parse_decrypted_file",test,original->file_buffer,source->file_buffer,source->file_size);
        const auto decoded_size=p::read<std::uint32_t>(original->file_buffer,0x28);check("parse_decoded_records",test,original->decoded_buffer,source->decoded_buffer,decoded_size);
        HeapFree(GetProcessHeap(),0,original->file_buffer);HeapFree(GetProcessHeap(),0,original->decoded_buffer);original->file_buffer=original->decoded_buffer=nullptr;p::release_snapshot_buffers(*source);
    }
    std::cerr<<"manager load/save and source worker tests\n";
    const auto fixture_directory=std::filesystem::absolute(std::filesystem::path(argv[2]).parent_path()/"oracle"/"isolated_files");std::filesystem::create_directories(fixture_directory);
    const auto directory=fixture_directory.string();strcpy_s(pe::window_state.user_data_directory,directory.c_str());strcpy_s(reinterpret_cast<char*>(mapped_image_base+0x1b67e1),4096,directory.c_str());
    auto* raw_manager=static_cast<p::SaveManager*>(VirtualAlloc(nullptr,sizeof(p::SaveManager),MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));if(!raw_manager)throw std::bad_alloc();
    auto store_file=[&](const char* name,const th20::source::Bytes* bytes){const auto path=fixture_directory/name;if(!bytes){std::filesystem::remove(path);return;}std::ofstream file(path,std::ios::binary);file.write(reinterpret_cast<const char*>(bytes->data()),bytes->size());};
    auto read_file=[&](const char* name){return th20::read_file(fixture_directory/name);};
    std::vector<th20::source::Bytes> fixtures;
    for(unsigned i=0;i<4;++i){auto snapshot=std::make_unique<p::Snapshot>();for(auto& profile:snapshot->profiles)p::initialize_profile(profile);state::Random stream(1);state::seed(stream,1234+i);p::initialize_metadata(snapshot->metadata,stream);snapshot->profiles[3].bytes[0x1234]=static_cast<std::uint8_t>(i+7);p::parse_snapshot(*snapshot,{});fixtures.push_back(p::serialize_snapshot(*snapshot,encode));p::release_snapshot_buffers(*snapshot);}
    for(unsigned test=0;test<24;++test){
        store_file("scoreth20bak.dat",test%6==0?nullptr:&fixtures[test%4]);store_file("scoreth20.dat",test%6<2?nullptr:&fixtures[(test+1)%4]);
        if(test%6==3){auto invalid=fixtures[1];invalid[0]=0;store_file("scoreth20.dat",&invalid);}
        std::memset(raw_manager,0,sizeof(*raw_manager));cpu<void>(0x50e540,&raw_manager->current);cpu<void>(0x50e540,&raw_manager->backup);
        state::Random initial(1);state::seed(initial,random());auto* original_stream=reinterpret_cast<state::Random*>(mapped_image_base+0x1ba4c4);*original_stream=initial;
        cpu<void>(0x50f3b0,raw_manager,0);const auto expected_stream=*original_stream;
        state::random_streams[1]=initial;p::manager=p::create_manager();auto& source_manager=*p::manager;th20::source::runtime::join_worker(source_manager.worker);
        auto compare_snapshot=[&](const char* label,const p::Snapshot& original,const p::Snapshot& actual){check(label,test,original.profiles,actual.profiles,sizeof(actual.profiles));check("manager_metadata",test,&original.metadata,&actual.metadata,sizeof(actual.metadata));check("manager_file_size",test,&original.file_size,&actual.file_size,4);const auto count=original.decoded_buffer?original.file_size:44;check("manager_file_bytes",test,original.file_buffer,actual.file_buffer,count);const bool e=original.decoded_buffer!=nullptr,a=actual.decoded_buffer!=nullptr;check("manager_decoded_present",test,&e,&a,1);if(e&&a)check("manager_decoded_bytes",test,original.decoded_buffer,actual.decoded_buffer,p::read<std::uint32_t>(original.file_buffer,0x28));};
        compare_snapshot("manager_current_profiles",raw_manager->current,source_manager.current);compare_snapshot("manager_backup_profiles",raw_manager->backup,source_manager.backup);check("manager_load_rng",test,&expected_stream,&state::random_streams[1],sizeof(initial));
        if(test%4==0){raw_manager->current.metadata.bytes[0x1d1]^=1;source_manager.current.metadata.bytes[0x1d1]^=1;}
        for(int mode:{1,4})for(int character=0;character<2;++character)for(int slot=0;slot<4;++slot){gs::session.player_table.field_1e0=mode;*reinterpret_cast<int*>(mapped_image_base+0x1ba7d0)=mode;pe::window_state.quit_requested=0;*reinterpret_cast<int*>(mapped_image_base+0x1b6760)=0;const auto expected=cpu<int>(0x464100,raw_manager,slot,character);const auto actual=source_manager.selected_profile(slot,character);check("manager_selected_profile",test,&expected,&actual,4);check("manager_checksum_quit",test,reinterpret_cast<void*>(mapped_image_base+0x1b6760),&pe::window_state.quit_requested,4);}
        saved_files.clear();cpu<void>(0x50fb60,raw_manager,0);const auto expected_files=saved_files;source_manager.commit();th20::source::runtime::join_worker(source_manager.worker);
        for(const char* name:{"scoreth20bak.dat","scoreth20.dat"}){const auto expected=expected_files.at((fixture_directory/name).string());const auto actual=read_file(name);const auto e=expected.size(),a=actual.size();check("manager_written_size",test,&e,&a,4);if(e==a)check("manager_written_bytes",test,expected.data(),actual.data(),e);}
        check("manager_backup_copy",test,raw_manager->backup.profiles,source_manager.backup.profiles,sizeof(raw_manager->backup.profiles));
        p::release();for(auto* snapshot:{&raw_manager->current,&raw_manager->backup}){if(snapshot->file_buffer)HeapFree(GetProcessHeap(),0,snapshot->file_buffer);if(snapshot->decoded_buffer)HeapFree(GetProcessHeap(),0,snapshot->decoded_buffer);}
    }
    std::cerr<<"StoneMenu profile selections, unlocks and inventory\n";
    for(unsigned test=0;test<4096;++test){
        fill(&raw_manager->current.metadata,sizeof(p::Metadata),test);const auto initial_metadata=raw_manager->current.metadata;
        state::Random initial(1);state::seed(initial,random());auto* original_stream=reinterpret_cast<state::Random*>(mapped_image_base+0x1ba4c4);*original_stream=initial;
        const auto character=int(test%2),index=int(test%9);const int mode=int(test%6);gs::session.player_table.field_1e0=mode;*reinterpret_cast<int*>(mapped_image_base+0x1ba7d0)=mode;
        const int slot=int(random()%7)-1,choice=int(random()%13)-2;const auto value=random();
        switch(test%3){case 0:cpu<void>(0x51c920,raw_manager,slot,character,choice);break;case 1:cpu<void>(0x51c7e0,raw_manager,index,value);break;case 2:cpu<void>(0x51b0c0,raw_manager,index);break;}
        const auto expected_metadata=raw_manager->current.metadata;const auto expected_rng=*original_stream;
        raw_manager->current.metadata=initial_metadata;state::random_streams[1]=initial;
        switch(test%3){case 0:raw_manager->select_profile(slot,character,choice);break;case 1:raw_manager->set_used_stone_count(index,value);break;case 2:raw_manager->consume_stone(index);break;}
        check("menu_mutated_metadata",test,&expected_metadata,&raw_manager->current.metadata,sizeof(p::Metadata));check("menu_mutated_rng",test,&expected_rng,&state::random_streams[1],sizeof(initial));
        pe::window_state.quit_requested=0;*reinterpret_cast<int*>(mapped_image_base+0x1b6760)=0;
        const auto ec=cpu<unsigned>(0x4bd610,raw_manager,index),ac=raw_manager->stone_count(index);check("stone_count",test,&ec,&ac,4);
        const auto eu=cpu<unsigned>(0x51b970,raw_manager,index),au=raw_manager->used_stone_count(index);check("used_stone_count",test,&eu,&au,4);
        p::write(raw_manager->current.profiles[character*9+index].bytes,0x76f0,test%2?random():0u);
        const bool unlocked=cpu<bool>(0x51bc10,raw_manager,character,index),actual_unlocked=raw_manager->extra_unlocked(character,index);check("extra_profile_unlock",test,&unlocked,&actual_unlocked,1);
        check("menu_checksum_quit",test,reinterpret_cast<void*>(mapped_image_base+0x1b6760),&pe::window_state.quit_requested,4);
    }
    #include "completion_cases.inc"
    #include "title_unlock_cases.inc"
    VirtualFree(raw_manager,0,MEM_RELEASE);
    std::ofstream report(argv[2],std::ios::binary);report<<"{\n  \"module\": \"progress_state\",\n  \"original_sha256\": \""<<expected_sha<<"\",\n  \"passed\": "<<passed<<",\n  \"failed\": "<<failed<<",\n  \"failures\": [";for(std::size_t i=0;i<failures.size();++i){if(i)report<<',';report<<'\"'<<failures[i]<<'\"';}report<<"]\n}\n";
    std::cout<<"passed="<<passed<<" failed="<<failed<<'\n';for(auto& failure:failures)std::cerr<<failure<<'\n';return failed?1:0;
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 2;}}
