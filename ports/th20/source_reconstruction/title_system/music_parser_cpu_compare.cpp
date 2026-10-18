// Test-only native archive stream boundary performs actual Win32 file I/O.
// No original entrypoint or instructions are patched.
#define wmain unused_native_cpu_main
#include "../../tests/native_cpu_compare.cpp"
#undef wmain
#include "music.hpp"
#include "../archive/archive.hpp"
#ifdef TH20_MUSIC_INTEGRATION_ORACLE
#include "../archive/resource_manager.hpp"
#include "music_parser_source_hashes.hpp"
#endif
#include <memory>
namespace ti=th20::source::title;
template<class R,class...A>R original(unsigned va,void* self,A...args){using F=R(__thiscall*)(void*,A...);return reinterpret_cast<F>(mapped_image_base+va-0x400000)(self,args...);}
struct Win32Stream {
    void** vtable;HANDLE file=INVALID_HANDLE_VALUE;unsigned reads=0,seeks=0;std::uint64_t bytes=0;
    static bool __fastcall open(Win32Stream* s,void*,const char* path,const char* mode){if(std::strcmp(mode,"r")){std::cerr<<"stream mode="<<mode<<"\n";throw std::runtime_error("Unexpected stream open mode");}s->file=CreateFileA(path,GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);return s->file!=INVALID_HANDLE_VALUE;}
    static int __fastcall read(Win32Stream* s,void*,void* data,unsigned bytes){DWORD got=0;if(!ReadFile(s->file,data,bytes,&got,nullptr))throw std::runtime_error("Native stream ReadFile failed");++s->reads;s->bytes+=got;return static_cast<int>(got);}
    static unsigned __fastcall size(Win32Stream* s,void*){LARGE_INTEGER value{};if(!GetFileSizeEx(s->file,&value)||value.HighPart)throw std::runtime_error("Native stream file size");return value.LowPart;}
    static bool __fastcall seek(Win32Stream* s,void*,unsigned where,int method){LARGE_INTEGER position{};position.QuadPart=static_cast<std::int32_t>(where);++s->seeks;return SetFilePointerEx(s->file,position,nullptr,method)!=0;}
    ~Win32Stream(){if(file!=INVALID_HANDLE_VALUE)CloseHandle(file);}
};
struct NativeRecord{char* name;unsigned offset,size,extra;};struct NativeArchive{NativeRecord* records;unsigned count;char* name;Win32Stream* stream;};
struct NativeHeap {
    inline static HANDLE current=nullptr;
    HANDLE handle=HeapCreate(0,0,0);
    NativeHeap(){if(!handle)throw std::runtime_error("Native CRT fixture heap creation failed");current=handle;}
    static HANDLE WINAPI get_process_heap(){return current;}
    ~NativeHeap(){HeapDestroy(handle);}
};
struct NativeRuntime {
    NativeRuntime(){
        using Initialize=bool(__cdecl*)();
        if(!reinterpret_cast<Initialize>(mapped_image_base+0x1465a5)()||!reinterpret_cast<Initialize>(mapped_image_base+0x1539ba)())throw std::runtime_error("Native CRT initialization failed");
    }
    ~NativeRuntime(){
        // Release native FLS callbacks before unloading their mapped code.
        using Uninitialize=bool(__cdecl*)(bool);
        reinterpret_cast<Uninitialize>(mapped_image_base+0x1539cc)(false);
        reinterpret_cast<Uninitialize>(mapped_image_base+0x1465c4)(false);
    }
};
int wmain(int argc,wchar_t**argv){try{
    if(argc!=4)throw std::runtime_error("Usage: music_parser_cpu_compare ORIGINAL.exe ORIGINAL.th20.dat OUTPUT.json");
    SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);
    AddVectoredExceptionHandler(1,[](PEXCEPTION_POINTERS p)->LONG{if(p->ExceptionRecord->ExceptionCode==EXCEPTION_ACCESS_VIOLATION){std::cerr<<"native AV VA="<<std::hex<<(p->ContextRecord->Eip-mapped_image_base+0x400000)<<" address="<<p->ExceptionRecord->ExceptionInformation[1]<<" eax="<<p->ContextRecord->Eax<<" ecx="<<p->ContextRecord->Ecx<<" edx="<<p->ContextRecord->Edx<<" actual_eip="<<p->ContextRecord->Eip<<"\n";auto* stack=reinterpret_cast<unsigned*>(p->ContextRecord->Esp);for(int i=0;i<1024;++i)if(stack[i]>=mapped_image_base&&stack[i]<mapped_image_base+0x210000)std::cerr<<"sp+"<<std::hex<<(i*4)<<" original="<<(stack[i]-mapped_image_base+0x400000)<<" ";std::cerr<<"\n"<<std::flush;}return EXCEPTION_CONTINUE_SEARCH;});
    const auto image_bytes=th20::read_file(argv[1]);if(sha256(image_bytes)!=expected_sha)throw std::runtime_error("Original SHA mismatch");const auto info=th20::parse_pe(image_bytes);Mapping image(image_bytes,info);mapped_image_base=image.address();
    for(const auto& item:info.imports)for(const auto* name:{L"kernel32.dll",L"gdi32.dll",L"user32.dll"})if(auto dll=GetModuleHandleW(name))if(auto entry=GetProcAddress(dll,item.name.c_str())){*reinterpret_cast<FARPROC*>(mapped_image_base+item.iat_rva)=entry;break;}
    NativeHeap native_heap;
    // Keep the original CRT's allocations independent of the host C++ CRT.
    // Resolve its process-heap OS boundary to a real dedicated Win32 heap.
    for(const auto& item:info.imports)if(item.name=="GetProcessHeap")*reinterpret_cast<FARPROC*>(mapped_image_base+item.iat_rva)=reinterpret_cast<FARPROC>(&NativeHeap::get_process_heap);
    *reinterpret_cast<HANDLE*>(mapped_image_base+0x1e5990)=native_heap.handle;for(unsigned slot=0;slot<22;++slot){auto* lock=reinterpret_cast<unsigned*>(mapped_image_base+0x1c0240+slot*0x30);std::memset(lock,0,0x30);lock[0]=0x101;lock[10]=GetCurrentThreadId();lock[11]=1;}
    NativeRuntime native_runtime;
    if(*reinterpret_cast<HANDLE*>(mapped_image_base+0x1e5990)!=native_heap.handle)throw std::runtime_error("Native CRT initialization changed the fixture heap");
    for(unsigned slot=0;slot<8;++slot)InitializeCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(mapped_image_base+0x1e4d40+slot*24));
    void* stream_vtable[7]{reinterpret_cast<void*>(&Win32Stream::open),nullptr,reinterpret_cast<void*>(&Win32Stream::read),nullptr,nullptr,reinterpret_cast<void*>(&Win32Stream::size),reinterpret_cast<void*>(&Win32Stream::seek)};Win32Stream stream{stream_vtable};auto& archive=*reinterpret_cast<NativeArchive*>(mapped_image_base+0x1b66c8);std::memset(&archive,0,sizeof(archive));archive.stream=&stream;
    const auto filename=std::filesystem::path(argv[2]).string();if(!original<bool>(0x539ed0,&archive,filename.c_str()))throw std::runtime_error("Native archive open failed");std::cout<<"native archive records="<<archive.count<<"\n"<<std::flush;
    auto archive_bytes=th20::read_file(argv[2]);th20::source::Archive source_archive(archive_bytes);auto resource=source_archive.read("musiccmt.txt");unsigned passed=0,failed=0;
    auto check=[&](bool condition,const char* what){if(condition)++passed;else{++failed;std::cerr<<"FAILED "<<what<<"\n";}};
    check(archive.count==source_archive.entries().size(),"catalog count");for(unsigned i=0;i<archive.count;++i){const auto& a=archive.records[i];const auto& b=source_archive.entries()[i];check(a.name==b.name&&a.offset==b.offset&&a.size==b.size&&a.extra==b.extra,"catalog record");}
#ifdef TH20_MUSIC_INTEGRATION_ORACLE
    if(!th20::source::resources::open(argv[2]))throw std::runtime_error("Source resource manager archive open failed");
    check(th20::source::resources::manager().entry_count()==archive.count,"production resource manager catalog count");
    auto managed_resource=th20::source::resources::read("musiccmt.txt",false);
    check(managed_resource&&*managed_resource==resource,"production resource read decoded bytes");
#endif
    std::cout<<"source resource size="<<resource.size()<<"\n"<<std::flush;std::vector<std::uint8_t> native_resource(resource.size());original<void>(0x53a3c0,&archive,"musiccmt.txt",native_resource.data());check(native_resource==resource,"music decoded bytes");
    std::cout<<"native resource read passed\n"<<std::flush;auto memory=std::make_unique<std::uint8_t[]>(sizeof(ti::TitleInf));auto& title=*reinterpret_cast<ti::TitleInf*>(memory.get());new(&title.cursor) th20::source::menu::Cursor;std::vector<std::uint8_t> before(sizeof(title)),expected(sizeof(title));std::uint32_t rng=0x15ab73e1;
    constexpr unsigned case_count=4096;
    unsigned completed_cases=0;
    for(unsigned test=0;test<case_count;++test){
        std::vector<std::uint8_t> cursor(sizeof(title.cursor));std::memcpy(cursor.data(),&title.cursor,cursor.size());for(std::size_t i=0;i<sizeof(title);++i){rng=rng*1664525u+1013904223u;memory[i]=static_cast<std::uint8_t>(rng>>24);}std::memcpy(&title.cursor,cursor.data(),cursor.size());title.cursor.excluded.clear();for(unsigned i=0;i<test%4;++i)title.cursor.excluded.push_back(i);title.cursor.current=static_cast<int>(rng);title.cursor.count=test%2?0:999;std::memcpy(before.data(),&title,sizeof(title));
        if(!(test%512))std::cout<<"parser case="<<test<<"\n"<<std::flush;
        const auto reads_before=stream.reads,seeks_before=stream.seeks;
        FloatingEnvironment::prepare();
        // Original worker callback consumes one unused stack argument (ret 4).
        // Omitting it corrupts the fixture stack even when the first cases agree.
        const auto native_return=original<std::uint32_t>(0x51f890,&title,rng);
        std::memcpy(expected.data(),&title,sizeof(title));std::memcpy(&title,before.data(),sizeof(title));
#ifdef TH20_MUSIC_INTEGRATION_ORACLE
        ti::read_music_comments(title);
#else
        ti::parse_music_comments(title,{reinterpret_cast<const char*>(resource.data()),resource.size()});
#endif
        ++completed_cases;
        check(native_return==0,"native worker return");
        check(stream.reads==reads_before+1&&stream.seeks==seeks_before+1,"original parser performs archive I/O");
        const bool equal=std::memcmp(expected.data(),&title,sizeof(title))==0;check(equal,"whole TitleInf after parser");if(!equal){for(std::size_t i=0,n=0;i<sizeof(title)&&n<20;++i)if(expected[i]!=memory[i]){std::cerr<<" offset="<<std::hex<<i<<" native="<<unsigned(expected[i])<<" source="<<unsigned(memory[i])<<"\n";++n;}break;}
    }
    title.cursor.~Cursor();archive.stream=nullptr;original<void>(0x53a170,&archive);
#ifdef TH20_MUSIC_INTEGRATION_ORACLE
    th20::source::resources::close();
#endif
    check(sha256(th20::read_file(argv[2]))==sha256(archive_bytes),"archive input unchanged");
    std::ofstream out(argv[3]);out<<"{\n\"original_sha256\":\""<<expected_sha<<"\",\n\"archive_sha256\":\""<<sha256(archive_bytes)<<"\",\n\"musiccmt_sha256\":\""<<sha256(resource)<<"\",\n\"passed\":"<<passed<<",\"failed\":"<<failed<<",\n\"requested_parser_cases\":"<<case_count<<",\"full_parser_cases\":"<<completed_cases<<",\n\"original_file_reads\":"<<stream.reads<<",\"original_file_seeks\":"<<stream.seeks<<",\"original_bytes_read\":"<<stream.bytes;
#ifdef TH20_MUSIC_INTEGRATION_ORACLE
    out<<",\n\"source_chain\":\"read_music_comments -> resources::read -> Archive::read -> parse_music_comments\",\n\"native_chain\":\"51f890 -> 410aa0 -> 53a3c0 -> real Win32 file stream\",\n\"native_allocator\":\"unmodified native CRT, separate Win32 heap\",\n\"limitations\":[\"Original musiccmt.txt lacks a trailing NUL; native 411180 reads past the file allocation. Results describe this heap layout, not all undefined tail contents.\",\"Worker thread scheduling, asynchronous lifetime and rendered GPU frames are outside this comparison.\"],\n\"source_hashes\":{\n";
    bool comma=false;for(const auto& item:bound_source_hashes){if(comma)out<<",\n";comma=true;out<<th20::json_string(item.path)<<":"<<th20::json_string(item.hash);}out<<"\n}";
#endif
    out<<"\n}\n";std::cout<<passed<<" checks, "<<failed<<" failed\n";return failed?1:0;
}catch(const std::exception&e){std::cerr<<e.what()<<"\n";return 2;}}
