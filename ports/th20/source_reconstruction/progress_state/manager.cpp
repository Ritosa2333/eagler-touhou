#include "manager.hpp"
#include "file_codec.hpp"
#ifdef TH_SDL3
#include "platform/Files.hpp"
#endif
#ifdef TH_SDL3
#include "platform/Files.hpp"
#endif
#include "../archive/resource_manager.hpp"
#include "../program_entry/program_entry.hpp"
#include "../game_session/session.hpp"
#include <filesystem>
#include <new>
namespace th20::source::progress {
namespace pe=program_entry;
SaveManager* manager=nullptr;
namespace {
std::filesystem::path score_path(const char* name){return std::filesystem::path(pe::window_state.user_data_directory)/name;}
void read_file(Snapshot& snapshot,const char* name){
    const auto path=score_path(name).string();const auto contents=resources::read(path.c_str(),true);
    if(contents){snapshot.file_buffer=static_cast<std::uint8_t*>(runtime::allocate_bytes(contents->size()));if(!snapshot.file_buffer)throw std::bad_alloc();snapshot.file_size=static_cast<std::uint32_t>(contents->size());std::memcpy(snapshot.file_buffer,contents->data(),contents->size());}
    initialize_metadata(snapshot.metadata,state::random_streams[1]);
    // The original loop is 2*10, with char0/index9 revisiting char1/index0.
    for(unsigned character=0;character<2;++character)for(unsigned index=0;index<10;++index)initialize_profile(snapshot.profiles[character*9+index]);
}
void write_file(Snapshot& snapshot,const char* name){
    const auto bytes=serialize_snapshot(snapshot,[](const Bytes& input){Bytes result;resources::with_shared_dictionary([&](auto& dictionary){LzssEncoder encoder(dictionary);result=encoder.encode(input);});return result;});
    if(bytes.empty())return;
    const auto path=score_path(name).string();std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(2));
#ifdef TH_SDL3
    const auto output=web::files::open(path.c_str(),true);
    if(!output){
        runtime::log_error(pe::log_buffer,"error : \x83\x58\x83\x52\x83\x41\x83\x74\x83\x40\x83\x43\x83\x8b\x82\xaa\x8f\x91\x82\xab\x8d\x9e\x82\xdf\x82\xc8\x82\xa2\n");return;}
    DWORD written=web::files::write(output,bytes.data(),44);
    // Two writes are observable in the original. Its short-first-write path
    // closes the handle; the attempted second write therefore also fails.
    if(written!=44)web::files::close(output);
    const auto size=static_cast<DWORD>(bytes.size()-44);written=web::files::write(output,bytes.data()+44,size);
    if(written!=size)web::files::close(output);web::files::close(output);
#else
    wchar_t wide_path[MAX_PATH+2]{};MultiByteToWideChar(932,0,path.c_str(),-1,wide_path,MAX_PATH);
    HANDLE output=CreateFileW(wide_path,GENERIC_WRITE,FILE_SHARE_READ,nullptr,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,nullptr);
    if(output==INVALID_HANDLE_VALUE){wchar_t* message=nullptr;FormatMessageW(0x1300,nullptr,GetLastError(),0x400,reinterpret_cast<LPWSTR>(&message),0,nullptr);LocalFree(message);
        runtime::log_error(pe::log_buffer,"error : \x83\x58\x83\x52\x83\x41\x83\x74\x83\x40\x83\x43\x83\x8b\x82\xaa\x8f\x91\x82\xab\x8d\x9e\x82\xdf\x82\xc8\x82\xa2\n");return;}
    DWORD written=0;WriteFile(output,bytes.data(),44,&written,nullptr);
    // Two writes are observable in the original. Its short-first-write path
    // closes the handle; the attempted second write therefore also fails.
    if(written!=44)CloseHandle(output);
    const auto size=static_cast<DWORD>(bytes.size()-44);written=0;WriteFile(output,bytes.data()+44,size,&written,nullptr);
    if(written!=size)CloseHandle(output);CloseHandle(output);
#endif
}
}
SaveManager::SaveManager(){runtime::join_worker(worker);launch(&SaveManager::load);}
void SaveManager::launch(void(SaveManager::*operation)()){
    std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(6));
    {std::lock_guard<std::recursive_mutex> nested(runtime::shared_locks().slot(6));if(worker.thread.joinable())worker.thread.detach();}
    worker.close_requested.store(false,std::memory_order_seq_cst);worker.thread=TH20_WORKER_THREAD("save-manager",[this,operation]{(this->*operation)();});
}
int SaveManager::commit(){runtime::join_worker(worker);launch(&SaveManager::save);return 0;}
SaveManager::~SaveManager(){commit();runtime::join_worker(worker);release_snapshot_buffers(current);release_snapshot_buffers(backup);}
void SaveManager::load(){
    std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(20));
    read_file(backup,"scoreth20bak.dat");parse_snapshot(backup,resources::decode_shared);
    read_file(current,"scoreth20.dat");copy_snapshot_records(current,backup);parse_snapshot(current,resources::decode_shared);copy_snapshot_records(backup,current);
}
void SaveManager::save(){std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(20));write_file(backup,"scoreth20bak.dat");write_file(current,"scoreth20.dat");copy_snapshot_records(backup,current);}

std::int32_t SaveManager::selected_profile(int slot,int character){std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(20));verify_metadata();return progress::selected_profile(current.metadata,slot,character,game_session::session.player_table.field_1e0);}
SaveManager* create_manager(){void* memory=::operator new(sizeof(SaveManager),std::nothrow);if(!memory)return nullptr;std::memset(memory,0,sizeof(SaveManager));try{return ::new(memory)SaveManager;}catch(...){::operator delete(memory);throw;}}
void initialize(){manager=create_manager();}
void release(){if(manager){manager->~SaveManager();std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(1));::operator delete(manager);}manager=nullptr;}
}
namespace th20::source::startup::unrecovered {
void initialize_loading_cache(){progress::initialize();}
void release_loading_cache(){progress::release();}
}
namespace th20::source::gameplay::unrecovered {
std::int32_t selected_profile(int slot,int character){return progress::manager->selected_profile(slot,character);}
progress::Profile& progress_profile(int character,int index){return *progress::find_profile(progress::manager->current,character,index);}
void commit_progress(){progress::manager->commit();}
void commit_progress_if_present(){if(progress::manager)progress::manager->commit();}
}
