// Only this isolated oracle maps the reference PE; no entrypoint is executed.
#define wmain unused_native_cpu_main
#include "../../../tests/native_cpu_compare.cpp"
#undef wmain
#include "../trophy.hpp"
#include "../../runtime_core/runtime_core.hpp"
namespace tr=th20::source::trophy;
template<class R,class...A>R cpu(unsigned va,void* self,A...args){return reinterpret_cast<R(__thiscall*)(void*,A...)>(mapped_image_base+va-0x400000)(self,args...);}
struct Resource:std::pmr::memory_resource {
    std::vector<std::array<std::size_t,3>> events;
    void* do_allocate(std::size_t n,std::size_t a)override{events.push_back({1,n,a});return std::pmr::new_delete_resource()->allocate(n,a);}
    void do_deallocate(void* p,std::size_t n,std::size_t a)override{events.push_back({0,n,a});std::pmr::new_delete_resource()->deallocate(p,n,a);}
    bool do_is_equal(const std::pmr::memory_resource& o)const noexcept override{return this==&o;}
};
LONG WINAPI fault(EXCEPTION_POINTERS* p){std::cerr<<"fault="<<std::hex<<p->ExceptionRecord->ExceptionCode<<" EIP="<<p->ContextRecord->Eip<<" base="<<mapped_image_base<<std::endl;return EXCEPTION_EXECUTE_HANDLER;}
int wmain(int argc,wchar_t**argv){SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);SetUnhandledExceptionFilter(fault);try{
    if(argc!=4)throw std::runtime_error("Usage: trophy_compare ORIGINAL.exe trophy.txt REPORT.json");auto bytes=th20::read_file(argv[1]);if(sha256(bytes)!=expected_sha)throw std::runtime_error("Original SHA mismatch");const auto pe=th20::parse_pe(bytes);Mapping image(bytes,pe);mapped_image_base=image.address();
    for(const auto& item:pe.imports)if(item.name=="GetCurrentThreadId"||item.name=="AcquireSRWLockExclusive"||item.name=="ReleaseSRWLockExclusive"||item.name=="HeapAlloc"||item.name=="HeapFree"||item.name=="GetLastError")*reinterpret_cast<std::uintptr_t*>(mapped_image_base+item.iat_rva)=reinterpret_cast<std::uintptr_t>(GetProcAddress(GetModuleHandleW(L"kernel32.dll"),item.name.c_str()));
    *reinterpret_cast<HANDLE*>(mapped_image_base+0x1e5990)=GetProcessHeap();cpu<void>(0x452e00,reinterpret_cast<void*>(mapped_image_base+0x1c0240));
    Resource resource;auto* saved=std::pmr::set_default_resource(&resource);*reinterpret_cast<void**>(mapped_image_base+0x1e4d28)=&resource;
    unsigned passed=0,failed=0;std::map<std::string,unsigned> counts;std::vector<std::string> failures;std::mt19937 random(0x52e210);
    auto check=[&](const char* label,bool okay){++counts[label];if(okay){++passed;return;}++failed;if(failures.size()<20)failures.push_back(label);};
    for(unsigned test=0;test<16384;++test){
        std::string input(random()%256,' ');for(auto& c:input)c=static_cast<char>(1+random()%255);
        std::uint8_t e[256],a[256];std::memset(e,0xcd,256);std::memset(a,0xcd,256);
        using Encode=void(__cdecl*)(std::uint8_t*,std::pmr::string);reinterpret_cast<Encode>(mapped_image_base+0x12f100)(e,std::pmr::string(input));tr::encode_string(a,input.c_str());check("encode_complete_buffer",!std::memcmp(e,a,256));
        using Decode=const char*(__cdecl*)(const std::uint8_t*);const auto* decoded=reinterpret_cast<Decode>(mapped_image_base+0x12f060)(e);check("decode_original_bytes",std::strcmp(decoded,tr::decode_string(a))==0&&std::strcmp(decoded,input.c_str())==0);
    }
    for(unsigned run=0;run<64;++run){
        alignas(tr::Queue) std::uint8_t storage[sizeof(tr::Queue)];std::memset(storage,0xa5,sizeof(storage));auto& e=*reinterpret_cast<tr::Queue*>(storage);resource.events.clear();cpu<void*>(0x52d8c0,&e);const auto ctor_events=resource.events;resource.events.clear();tr::Queue a;
        check("queue_constructor",e.allocator==a.allocator&&e.map==a.map&&e.map_size==a.map_size&&e.offset==a.offset&&e.count==a.count&&e.proxy->container==&e.proxy&&!e.proxy->iterator&&a.proxy->container==&a.proxy&&!a.proxy->iterator&&resource.events==ctor_events);
        for(unsigned step=0;step<2048;++step){
            resource.events.clear();std::int32_t ev=0,av=0;
            const bool push=!e.count||(random()%5!=0&&step<1536);const auto value=static_cast<int>(random());
            if(push)cpu<void>(0x52efb0,&e,value);else ev=cpu<int>(0x52f740,&e,0u);const auto native_events=resource.events;resource.events.clear();
            if(push)a.push(value);else av=a.pop_front();check("queue_return_and_allocations",ev==av&&native_events==resource.events);
            bool equal=e.map_size==a.map_size&&e.offset==a.offset&&e.count==a.count;
            if(equal){for(unsigned b=0;b<e.map_size;++b)equal=equal&&bool(e.map[b])==bool(a.map[b]);for(unsigned i=0;i<e.count;++i){const unsigned pos=e.offset+i;equal=equal&&e.map[(pos/4)&(e.map_size-1)][pos%4]==a.map[(pos/4)&(a.map_size-1)][pos%4];}}
            check("queue_layout_and_contents",equal);
        }
        resource.events.clear();cpu<void>(0x52db90,&e);const auto native_events=resource.events;resource.events.clear();a.~Queue();const auto source_events=resource.events;check("queue_destructor_allocations",native_events==source_events);new(&a)tr::Queue;
    }
    const auto asset=th20::read_file(argv[2]);auto records=std::make_unique<tr::Message[]>(128);std::memset(records.get(),0xa5,sizeof(tr::Message)*128);tr::parse_messages(std::span<tr::Message,128>(records.get(),128),std::string_view(reinterpret_cast<const char*>(asset.data()),asset.size()));unsigned record_count=0;
    for(unsigned i=0;i<128;++i)if(records[i].id==static_cast<int>(i)){++record_count;for(unsigned field=0;field<7;++field){const auto* row=reinterpret_cast<const std::uint8_t*>(&records[i])+4+field*256;using Decode=const char*(__cdecl*)(const std::uint8_t*);const char* native=reinterpret_cast<Decode>(mapped_image_base+0x12f060)(row);check("asset_row_native_decode",std::strcmp(native,tr::decode_string(row))==0);}}
    std::pmr::set_default_resource(saved);std::ofstream out(argv[3]);out<<"{\"original_sha256\":"<<th20::json_string(expected_sha)<<",\"asset_sha256\":"<<th20::json_string(sha256(asset))<<",\"asset_records\":"<<record_count<<",\"passed\":"<<passed<<",\"failed\":"<<failed<<",\"counts\":{";bool first=true;for(auto& [name,count]:counts){if(!first)out<<',';first=false;out<<th20::json_string(name)<<':'<<count;}out<<"},\"failures\":[";for(unsigned i=0;i<failures.size();++i){if(i)out<<',';out<<th20::json_string(failures[i]);std::cerr<<failures[i]<<'\n';}out<<"]}\n";std::cout<<"passed="<<passed<<" failed="<<failed<<" asset_records="<<record_count<<std::endl;return failed?1:0;
}catch(const std::exception& e){std::cerr<<e.what()<<std::endl;return 2;}}
