#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d9.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <intrin.h>
#include <climits>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <share.h>
#include <vector>

namespace {
using Create9 = IDirect3D9* (WINAPI*)(UINT);
using DisplayMode = HRESULT (STDMETHODCALLTYPE*)(IDirect3D9*, UINT, D3DDISPLAYMODE*);
using CreateDevice = HRESULT (STDMETHODCALLTYPE*)(IDirect3D9*, UINT, D3DDEVTYPE, HWND,
    DWORD, D3DPRESENT_PARAMETERS*, IDirect3DDevice9**);
using Present = HRESULT (STDMETHODCALLTYPE*)(IDirect3DDevice9*, const RECT*, const RECT*,
    HWND, const RGNDATA*);
using DrawPrimitive = HRESULT (STDMETHODCALLTYPE*)(IDirect3DDevice9*,D3DPRIMITIVETYPE,UINT,UINT);
using DrawIndexedPrimitive = HRESULT (STDMETHODCALLTYPE*)(IDirect3DDevice9*,D3DPRIMITIVETYPE,INT,UINT,UINT,UINT,UINT);
using DrawPrimitiveUP = HRESULT (STDMETHODCALLTYPE*)(IDirect3DDevice9*,D3DPRIMITIVETYPE,UINT,const void*,UINT);
using DrawIndexedPrimitiveUP = HRESULT (STDMETHODCALLTYPE*)(IDirect3DDevice9*,D3DPRIMITIVETYPE,UINT,UINT,UINT,const void*,D3DFORMAT,const void*,UINT);
CreateDevice originalCreateDevice;
DisplayMode originalDisplayMode;
Present originalPresent;
DrawPrimitive originalDrawPrimitive;
DrawIndexedPrimitive originalDrawIndexedPrimitive;
DrawPrimitiveUP originalDrawPrimitiveUP;
DrawIndexedPrimitiveUP originalDrawIndexedPrimitiveUP;
bool stateOnlyDrawActive;
bool cpuDebugActive;
std::uintptr_t gameBase;
FILE* trace;
LONG CALLBACK recordCaptureException(EXCEPTION_POINTERS* event) {
    if(event->ExceptionRecord->ExceptionCode!=EXCEPTION_ACCESS_VIOLATION)return EXCEPTION_CONTINUE_SEARCH;
    if(FILE* output=std::fopen("native-exception.jsonl","ab")){
        const auto& c=*event->ContextRecord;
        std::fprintf(output,"{\"code\":%lu,\"eip\":%lu,\"rva\":%lu,\"eax\":%lu,\"ebx\":%lu,\"ecx\":%lu,\"edx\":%lu,\"esi\":%lu,\"edi\":%lu,\"esp\":%lu,\"access\":%llu,\"address\":%llu,\"stack\":[",
            event->ExceptionRecord->ExceptionCode,c.Eip,static_cast<unsigned long>(c.Eip-gameBase),c.Eax,c.Ebx,c.Ecx,c.Edx,c.Esi,c.Edi,c.Esp,
            static_cast<unsigned long long>(event->ExceptionRecord->ExceptionInformation[0]),static_cast<unsigned long long>(event->ExceptionRecord->ExceptionInformation[1]));
        unsigned words[24]{};SIZE_T count=0;ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(c.Esp),words,sizeof(words),&count);
        for(unsigned i=0;i<count/4;++i)std::fprintf(output,"%s%u",i?",":"",words[i]);
        std::fputs("]}\n",output);std::fclose(output);
    }
    return EXCEPTION_CONTINUE_SEARCH;
}
// Layout from the Windows SDK D3D9ON12_ARGS; keep old MinGW SDKs usable.
struct On12Arguments {
    BOOL enabled;
    IUnknown* device;
    IUnknown* queues[2];
    UINT queueCount;
    UINT nodeMask;
};
void backendEvent(const char* stage, HRESULT result) {
    if (FILE* output=std::fopen("native-backend.jsonl","ab")) {
        std::fprintf(output,"{\"backend\":\"warp-d3d9on12\",\"stage\":\"%s\",\"result\":%ld}\n",stage,long(result));
        std::fclose(output);
    }
}
IDirect3D9* createWarp9(HMODULE d3d9, UINT sdkVersion) {
    using CreateFactory = HRESULT (WINAPI*)(REFIID, void**);
    using CreateDevice12 = HRESULT (WINAPI*)(IUnknown*, D3D_FEATURE_LEVEL, REFIID, void**);
    using CreateOn12 = IDirect3D9* (WINAPI*)(UINT, On12Arguments*, UINT);
    const auto dxgi=LoadLibraryExW(L"dxgi.dll",nullptr,LOAD_LIBRARY_SEARCH_SYSTEM32);
    const auto d3d12=LoadLibraryExW(L"d3d12.dll",nullptr,LOAD_LIBRARY_SEARCH_SYSTEM32);
    const auto createFactory=dxgi?reinterpret_cast<CreateFactory>(GetProcAddress(dxgi,"CreateDXGIFactory1")):nullptr;
    const auto create12=d3d12?reinterpret_cast<CreateDevice12>(GetProcAddress(d3d12,"D3D12CreateDevice")):nullptr;
    const auto create9=reinterpret_cast<CreateOn12>(GetProcAddress(d3d9,"Direct3DCreate9On12"));
    if(!createFactory||!create12||!create9){backendEvent("entrypoints",E_NOINTERFACE);return nullptr;}
    IDXGIFactory4* factory=nullptr;
    HRESULT hr=createFactory(__uuidof(IDXGIFactory4),reinterpret_cast<void**>(&factory));
    backendEvent("factory",hr);
    if(FAILED(hr))return nullptr;
    IDXGIAdapter* adapter=nullptr;
    hr=factory->EnumWarpAdapter(__uuidof(IDXGIAdapter),reinterpret_cast<void**>(&adapter));
    factory->Release();
    backendEvent("warp-adapter",hr);
    if(FAILED(hr))return nullptr;
    ID3D12Device* device=nullptr;
    hr=create12(adapter,D3D_FEATURE_LEVEL_11_0,__uuidof(ID3D12Device),reinterpret_cast<void**>(&device));
    adapter->Release();
    backendEvent("warp-device",hr);
    if(FAILED(hr))return nullptr;
    On12Arguments arguments{};
    arguments.enabled=TRUE;
    arguments.device=device;
    IDirect3D9* result=create9(sdkVersion,&arguments,1);
    device->Release();
    backendEvent("d3d9",result?S_OK:E_FAIL);
    return result;
}
unsigned presentIndex, generation, priorFrame;
std::uintptr_t priorOwner;
constexpr unsigned imageFrames[] = {80, 83, 84, 85, 86, 87, 88, 90, 100, 105, 111, 112, 113, 114, 120, 124, 125, 126, 127, 130, 140, 180, 300, 600, 1200};
bool savedImage[sizeof(imageFrames) / sizeof(imageFrames[0])];
std::vector<unsigned> extraImageFrames;
unsigned imageSegment = UINT_MAX;
void* originalRng;
DWORD captureThread;
FILE* rngTrace;
unsigned rngCallCount, rngCallsFlushed;
unsigned traceFirst, traceLast;
bool hasTraceWindow;
volatile LONG skippedRngThreads;
struct RngCall {
    unsigned present, frame, flags, object, before, caller, bp, si, di;
    unsigned stack[8], frameWords[12], instructionWords[6];
    unsigned subIndex, ip, subContext[6];
    unsigned animation[12];
    unsigned counterManagerIndex, counterSlot, counterSlotFlags;
    char subName[64];
};
RngCall rngCalls[8192];

bool detailedFrame(unsigned frame,unsigned first,unsigned last) {
    return hasTraceWindow ? frame>=traceFirst && frame<=traceLast : frame>=first && frame<=last;
}

template<class Fn> bool patch(void** slot, Fn replacement, Fn& original) {
    DWORD oldProtection, ignored;
    if (!VirtualProtect(slot, sizeof(*slot), PAGE_READWRITE, &oldProtection)) return false;
    original = reinterpret_cast<Fn>(*slot);
    *slot = reinterpret_cast<void*>(replacement);
    VirtualProtect(slot, sizeof(*slot), oldProtection, &ignored);
    return true;
}

template<class T> T read(std::uintptr_t address) {
    T value;
    std::memcpy(&value, reinterpret_cast<const void*>(address), sizeof(value));
    return value;
}

void __cdecl recordRngCall(unsigned object, const unsigned* stack, unsigned bp, unsigned si, unsigned di) {
    if (object != gameBase + 0x1c6620 && object != gameBase + 0x1c6638) return;
    if (GetCurrentThreadId() != captureThread) {
        InterlockedIncrement(&skippedRngThreads);
        return;
    }
    const auto owner = read<unsigned>(gameBase + 0x1d1a54);
    if (!owner) return;
    const auto frame = read<unsigned>(owner + 0x14);
    if (hasTraceWindow) {
        if (!detailedFrame(frame,0,0)) return;
    } else if (frame > 10 && (frame < 80 || frame > 90) && (frame < 110 || frame > 114) && (frame < 122 || frame > 130)) return;
    if (rngCallCount >= _countof(rngCalls)) return;
    auto& sample = rngCalls[rngCallCount++];
    sample.present = presentIndex + 1;
    sample.frame = frame;
    sample.flags = read<unsigned>(owner + 0x140);
    sample.object = object - gameBase;
    sample.before = read<unsigned>(object + 4);
    sample.caller = stack[0] - gameBase;
    sample.bp = bp;
    sample.si = si;
    sample.di = di;
    if(cpuDebugActive&&sample.caller==0x10c6fb){
        if(FILE* output=std::fopen("native-enemy-death.jsonl","ab")){
            std::fprintf(output,"{\"frame\":%u,\"seed\":%u,\"id\":%u,\"position\":[%.9g,%.9g],\"flags\":%u,\"status\":%u,\"sprite\":%d}\n",frame,sample.before,read<unsigned>(si+0x6858),read<float>(si+0x1278),read<float>(si+0x127c),read<unsigned>(si+0x639c),read<unsigned>(si+0x63a0),read<int>(si+0x14a8));
            std::fclose(output);
        }
    }
    if(cpuDebugActive&&sample.caller==0x14a033){
        if(FILE* output=std::fopen("native-graze-debug.jsonl","ab")){
            std::fprintf(output,"{\"frame\":%u,\"seed\":%u,\"position\":[%u,%u]}\n",frame,sample.before,read<unsigned>(di),read<unsigned>(di+4));
            std::fclose(output);
        }
    }
    if (sample.caller == 0x11be8e) {
        // 0x51c042 selects the receiving manager. 0x4f3925 takes its first free slot.
        const auto get=[](unsigned address){
            unsigned value=0;
            ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<const void*>(address),&value,sizeof(value),nullptr);
            return value;
        };
        sample.counterManagerIndex=get(di+0x6e4);
        const auto manager=sample.counterManagerIndex<2
            ? get(gameBase+0x1d1a9c-sample.counterManagerIndex*60) : 0;
        sample.counterSlot=get(get(manager+0x8156b0));
        if(sample.counterSlot)sample.counterSlotFlags=get(sample.counterSlot+0x10);
    }
    const auto top = __readfsdword(4);
    for (unsigned i = 0; i < _countof(sample.stack) && unsigned(stack + 1 + i) < top; ++i)
        sample.stack[i] = stack[1 + i];
    if (detailedFrame(frame,110,130) && (sample.caller==0x4be1||sample.caller==0x4ca1)) {
        const auto vm=sample.stack[0];
        auto get=[](unsigned address){
            unsigned value=0;
            ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<const void*>(address),&value,sizeof(value),nullptr);
            return value;
        };
        sample.animation[0]=vm;sample.animation[1]=get(vm+0x1c);
        sample.animation[2]=get(vm+0x28);sample.animation[3]=get(vm+0x2c);
        sample.animation[4]=get(vm+0x454);
        sample.animation[5]=get(di);sample.animation[6]=get(di+4);sample.animation[7]=get(di+12);
        sample.animation[8]=get(vm+0x4cc);sample.animation[9]=get(vm+0x4e4);
        sample.animation[10]=get(vm+0x508);sample.animation[11]=get(vm+0x564);
    }
    if (frame >= 85 && frame <= 86 && (sample.caller == 0x4c48 || sample.caller == 0x4ca1)) {
        ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<const void*>(bp),
                          sample.frameWords, sizeof(sample.frameWords), nullptr);

        if (sample.caller == 0x4c48) {
            auto get = [](unsigned address) {
                unsigned value=0;
                ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<const void*>(address), &value, sizeof(value), nullptr);
                return value;
            };
            sample.subIndex=get(di+4);
            sample.ip=get(di+8);
            auto host=get(di+0x1018), program=get(host+0x1218), table=get(program+0x20c);
            auto entry=table+sample.subIndex*8;
            auto name=get(entry), header=get(entry+4);
            sample.subContext[0]=host; sample.subContext[1]=program; sample.subContext[2]=table;
            sample.subContext[3]=name; sample.subContext[4]=header; sample.subContext[5]=get(header);
            if(sample.subIndex<10000 && name)
                ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<const void*>(name), sample.subName, sizeof(sample.subName)-1, nullptr);
        }
        if (sample.caller == 0x4ca1)
            ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<const void*>(di),
                              sample.instructionWords, sizeof(sample.instructionWords), nullptr);
    }

}

__declspec(naked) void observeRng() {
    __asm {
        pushfd
        pushad
        mov ebx, esp
        and esp, -16
        sub esp, 512
        fxsave [esp]
        lea eax, [ebx + 36]
        push dword ptr [ebx]
        push dword ptr [ebx + 4]
        push dword ptr [ebx + 8]
        push eax
        push ecx
        call recordRngCall
        add esp, 20
        fxrstor [esp]
        mov esp, ebx
        popad
        popfd
        jmp dword ptr [originalRng]
    }
}

void relativeJump(unsigned char* instruction, std::uintptr_t destination) {
    instruction[0] = 0xe9;
    const auto delta = destination - reinterpret_cast<std::uintptr_t>(instruction + 5);
    std::memcpy(instruction + 1, &delta, sizeof(delta));
}

void* originalInputReset;
__declspec(naked) void observeInputReset() {
    __asm {
        cmp dword ptr [ecx + 0x2e10], 0
        je initialize
        xor eax, eax
        ret
    initialize:
        jmp dword ptr [originalInputReset]
    }
}

bool installCpuInputResetCompatibility() {
    auto* entry=reinterpret_cast<unsigned char*>(gameBase+0xb8b80);
    const unsigned char prolog[]={0x55,0x8b,0xec,0x6a,0xff,0x68};
    if(std::memcmp(entry,prolog,sizeof(prolog))!=0||
       read<unsigned>(gameBase+0xb8b86)!=gameBase+0x17645b)return false;
    auto* trampoline=static_cast<unsigned char*>(VirtualAlloc(nullptr,10,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE));
    if(!trampoline)return false;
    std::memcpy(trampoline,entry,5);
    relativeJump(trampoline+5,reinterpret_cast<std::uintptr_t>(entry+5));
    DWORD protection=0,ignored=0;
    if(!VirtualProtect(entry,5,PAGE_EXECUTE_READWRITE,&protection)){
        VirtualFree(trampoline,0,MEM_RELEASE);return false;
    }
    originalInputReset=trampoline;
    relativeJump(entry,reinterpret_cast<std::uintptr_t>(&observeInputReset));
    FlushInstructionCache(GetCurrentProcess(),entry,5);
    VirtualProtect(entry,5,protection,&ignored);
    return true;
}

void* originalAnmChild;
unsigned __fastcall observeAnmChild(void* file,void*,unsigned output,int script,unsigned parent,unsigned flags){
    const auto game=read<unsigned>(gameBase+0x1d1a54);
    const auto frame=game?read<unsigned>(game+0x14):0;
    if(parent&&GetCurrentThreadId()==captureThread&&cpuDebugActive&&detailedFrame(frame,0,0)){
        const auto nextNode=read<unsigned>(parent+0x510);
        const auto next=nextNode?read<unsigned>(nextNode):0;
        if(FILE* trace=std::fopen("native-anm-child.jsonl","ab")){
            std::fprintf(trace,"{\"frame\":%u,\"script\":%d,\"flags\":%u,\"parent\":%u,\"parentScript\":%d,\"parentTime\":%d,\"parentFlags\":%u,\"nextScript\":%d}\n",frame,script,flags,parent,read<int>(parent+0x28),read<int>(parent+0x4e4),read<unsigned>(parent+0x57c),next?read<int>(next+0x28):-1);
            std::fclose(trace);
        }
    }
    using Create=unsigned(__thiscall*)(void*,unsigned,int,unsigned,unsigned);
    return reinterpret_cast<Create>(originalAnmChild)(file,output,script,parent,flags);
}
bool installAnmChildObserver(){
    auto* entry=reinterpret_cast<unsigned char*>(gameBase+0xd0770);
    const unsigned char prolog[]={0x55,0x8b,0xec,0x6a,0xff};
    if(std::memcmp(entry,prolog,sizeof(prolog))!=0)return false;
    auto* relay=static_cast<unsigned char*>(VirtualAlloc(nullptr,10,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE));
    if(!relay)return false;
    std::memcpy(relay,entry,5);
    relativeJump(relay+5,reinterpret_cast<std::uintptr_t>(entry+5));
    DWORD protection=0,ignored=0;
    if(!VirtualProtect(entry,5,PAGE_EXECUTE_READWRITE,&protection)){VirtualFree(relay,0,MEM_RELEASE);return false;}
    originalAnmChild=relay;relativeJump(entry,reinterpret_cast<std::uintptr_t>(&observeAnmChild));
    FlushInstructionCache(GetCurrentProcess(),entry,5);VirtualProtect(entry,5,protection,&ignored);return true;
}

void* originalAnmDetached;
unsigned __fastcall observeAnmDetached(void* file,void*,unsigned output,int script,unsigned parent,unsigned flags){
    const auto game=read<unsigned>(gameBase+0x1d1a54);
    const auto frame=game?read<unsigned>(game+0x14):0;
    if(parent&&GetCurrentThreadId()==captureThread&&cpuDebugActive&&detailedFrame(frame,0,0)){
        static unsigned lastQueueFrame=~0u;
        static std::vector<unsigned> queueManagers;
        if(frame!=lastQueueFrame){lastQueueFrame=frame;queueManagers.clear();}
        const auto manager=read<unsigned>(parent+0x508);
        if(std::find(queueManagers.begin(),queueManagers.end(),manager)==queueManagers.end()){
            queueManagers.push_back(manager);
            if(FILE* queue=std::fopen("native-anm-queue.jsonl","ab")){
                unsigned index=0;
                for(auto node=manager?read<unsigned>(manager):0;node&&index<10000;node=read<unsigned>(node+4),++index){
                    const auto vm=read<unsigned>(node);
                    const int sid=read<int>(vm+0x28);
                    if(sid==106||sid==170)
                        std::fprintf(queue,"{\"frame\":%u,\"manager\":%u,\"index\":%u,\"vm\":%u,\"script\":%d,\"time\":%d,\"registration\":%u,\"flags\":%u,\"position\":[%.9g,%.9g]}\n",frame,manager,index,vm,sid,read<int>(vm+0x4e4),read<unsigned>(vm+0x4d8),read<unsigned>(vm+0x57c),read<float>(vm+0x30),read<float>(vm+0x34));
                }
                std::fclose(queue);
            }
        }
        const auto nextNode=read<unsigned>(parent+0x510);
        const auto next=nextNode?read<unsigned>(nextNode):0;
        if(FILE* trace=std::fopen("native-anm-detached.jsonl","ab")){
            std::fprintf(trace,"{\"frame\":%u,\"script\":%d,\"flags\":%u,\"parent\":%u,\"parentScript\":%d,\"parentTime\":%d,\"parentFlags\":%u,\"nextScript\":%d}\n",frame,script,flags,parent,read<int>(parent+0x28),read<int>(parent+0x4e4),read<unsigned>(parent+0x57c),next?read<int>(next+0x28):-1);
            std::fclose(trace);
        }
    }
    using Create=unsigned(__thiscall*)(void*,unsigned,int,unsigned,unsigned);
    return reinterpret_cast<Create>(originalAnmDetached)(file,output,script,parent,flags);
}
bool installAnmDetachedObserver(){
    auto* entry=reinterpret_cast<unsigned char*>(gameBase+0xd09b0);
    const unsigned char prolog[]={0x55,0x8b,0xec,0x6a,0xff};
    if(std::memcmp(entry,prolog,sizeof(prolog))!=0)return false;
    auto* relay=static_cast<unsigned char*>(VirtualAlloc(nullptr,10,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE));
    if(!relay)return false;
    std::memcpy(relay,entry,5);
    relativeJump(relay+5,reinterpret_cast<std::uintptr_t>(entry+5));
    DWORD protection=0,ignored=0;
    if(!VirtualProtect(entry,5,PAGE_EXECUTE_READWRITE,&protection)){VirtualFree(relay,0,MEM_RELEASE);return false;}
    originalAnmDetached=relay;relativeJump(entry,reinterpret_cast<std::uintptr_t>(&observeAnmDetached));
    FlushInstructionCache(GetCurrentProcess(),entry,5);VirtualProtect(entry,5,protection,&ignored);return true;
}

using StraightGrazeCall=unsigned(__thiscall*)(unsigned,unsigned);
StraightGrazeCall originalStraightGraze;
unsigned __fastcall observeStraightGraze(unsigned self,unsigned,unsigned argument){
    const auto game=read<unsigned>(gameBase+0x1d1a54);
    const auto frame=game?read<unsigned>(game+0x14):0;
    const int before=read<int>(self+0x3c);
    const unsigned result=originalStraightGraze(self,argument);
    if(cpuDebugActive&&GetCurrentThreadId()==captureThread&&detailedFrame(frame,0,0)){
        if(FILE* output=std::fopen("native-straight-graze.jsonl","ab")){
            std::fprintf(output,"{\"frame\":%u,\"laser\":%u,\"position\":[%.9g,%.9g],\"angle\":%.9g,\"clock\":[%d,%d],\"contacts\":%d}\n",frame,self,read<float>(self+0x60),read<float>(self+0x64),read<float>(self+0x78),before,read<int>(self+0x3c),read<int>(self+0x864));
            const auto shape=read<unsigned>(self+0x868);
            std::fprintf(output,"{\"frame\":%u,\"laser\":%u,\"rectangle\":[%.9g,%.9g,%.9g,%.9g],\"length\":%.9g}\n",frame,self,read<float>(shape),read<float>(shape+4),read<float>(shape+0x18),read<float>(shape+0x1c),read<float>(self+0x7c));
            std::fclose(output);
        }
    }
    return result;
}
bool installStraightGrazeObserver(){
    auto* entry=reinterpret_cast<unsigned char*>(gameBase+0x12dd30);
    const unsigned char signature[]={0x55,0x8b,0xec,0x83,0xec,0x34};
    if(std::memcmp(entry,signature,sizeof(signature)))return false;
    auto* relay=static_cast<unsigned char*>(VirtualAlloc(nullptr,11,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE));
    if(!relay)return false;
    std::memcpy(relay,entry,6);relativeJump(relay+6,reinterpret_cast<unsigned>(entry+6));
    DWORD protection,ignored;
    if(!VirtualProtect(entry,6,PAGE_EXECUTE_READWRITE,&protection))return false;
    originalStraightGraze=reinterpret_cast<StraightGrazeCall>(relay);
    relativeJump(entry,reinterpret_cast<unsigned>(observeStraightGraze));entry[5]=0x90;
    FlushInstructionCache(GetCurrentProcess(),entry,6);VirtualProtect(entry,6,protection,&ignored);return true;
}

void* originalFieldRay;
void __cdecl recordFieldRay(float angle,unsigned origin){
    const auto game=read<unsigned>(gameBase+0x1d1a54);
    const auto frame=game?read<unsigned>(game+0x14):0;
    if(GetCurrentThreadId()!=captureThread||!cpuDebugActive||!detailedFrame(frame,0,0))return;
    if(FILE* output=std::fopen("native-field-ray.jsonl","ab")){
        std::fprintf(output,"{\"frame\":%u,\"position\":[%.9g,%.9g],\"angle\":%.9g}\n",frame,read<float>(origin),read<float>(origin+4),angle);
        std::fclose(output);
    }
}
__declspec(naked) void observeFieldRay(){
    __asm {
        pushfd
        pushad
        sub esp,16
        movss dword ptr [esp],xmm3
        mov eax,dword ptr [esp+56]
        mov dword ptr [esp+4],eax
        call recordFieldRay
        movss xmm3,dword ptr [esp]
        add esp,16
        popad
        popfd
        jmp dword ptr [originalFieldRay]
    }
}
bool installFieldRayObserver(){
    auto* entry=reinterpret_cast<unsigned char*>(gameBase+0xd3140);
    const unsigned char prolog[]={0x55,0x8b,0xec,0x83,0xe4,0xf8};
    if(std::memcmp(entry,prolog,sizeof(prolog))!=0)return false;
    auto* relay=static_cast<unsigned char*>(VirtualAlloc(nullptr,11,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE));
    if(!relay)return false;
    std::memcpy(relay,entry,6);
    relativeJump(relay+6,reinterpret_cast<std::uintptr_t>(entry+6));
    DWORD protection=0,ignored=0;
    if(!VirtualProtect(entry,6,PAGE_EXECUTE_READWRITE,&protection)){
        VirtualFree(relay,0,MEM_RELEASE);return false;
    }
    originalFieldRay=relay;
    relativeJump(entry,reinterpret_cast<std::uintptr_t>(&observeFieldRay));entry[5]=0x90;
    FlushInstructionCache(GetCurrentProcess(),entry,6);
    VirtualProtect(entry,6,protection,&ignored);
    return true;
}

void* originalBulletFire;
int __fastcall observeBulletFire(void* manager,void*,unsigned descriptor,int column,int row,float aim){
    const auto game=read<unsigned>(gameBase+0x1d1a54);
    const auto frame=game?read<unsigned>(game+0x14):0;
    if(GetCurrentThreadId()==captureThread&&cpuDebugActive&&detailedFrame(frame,0,0)){
        if(FILE* output=std::fopen("native-bullet-fire.jsonl","ab")){
            std::fprintf(output,"{\"frame\":%u,\"descriptor\":%u,\"index\":[%d,%d],\"position\":[%.9g,%.9g],\"angles\":[%.9g,%.9g,%.9g],\"speed\":[%.9g,%.9g],\"muzzle\":%.9g,\"kind\":%u,\"color\":%u}\n",frame,descriptor,column,row,read<float>(descriptor+8),read<float>(descriptor+12),read<float>(descriptor+20),read<float>(descriptor+24),aim,read<float>(descriptor+28),read<float>(descriptor+32),read<float>(descriptor+36),unsigned(read<unsigned short>(descriptor)),unsigned(read<unsigned short>(descriptor+4)));
            std::fclose(output);
        }
    }
    using Fire=int(__thiscall*)(void*,unsigned,int,int,float);
    return reinterpret_cast<Fire>(originalBulletFire)(manager,descriptor,column,row,aim);
}
bool installBulletFireObserver(){
    auto* entry=reinterpret_cast<unsigned char*>(gameBase+0xf3910);
    const unsigned char prolog[]={0x55,0x8b,0xec,0x83,0xe4,0xc0};
    if(std::memcmp(entry,prolog,sizeof(prolog))!=0)return false;
    auto* relay=static_cast<unsigned char*>(VirtualAlloc(nullptr,11,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE));
    if(!relay)return false;
    std::memcpy(relay,entry,6);
    relativeJump(relay+6,reinterpret_cast<std::uintptr_t>(entry+6));
    DWORD protection=0,ignored=0;
    if(!VirtualProtect(entry,6,PAGE_EXECUTE_READWRITE,&protection)){
        VirtualFree(relay,0,MEM_RELEASE);return false;
    }
    originalBulletFire=relay;
    relativeJump(entry,reinterpret_cast<std::uintptr_t>(&observeBulletFire));
    entry[5]=0x90;
    FlushInstructionCache(GetCurrentProcess(),entry,6);
    VirtualProtect(entry,6,protection,&ignored);
    return true;
}

void* originalFixedCut;
void* originalFixedCircle;
void* originalFixedUpdate;
int __fastcall observeFixedUpdate(void* laser,void*){
    using Update=int(__thiscall*)(void*);
    const int result=reinterpret_cast<Update>(originalFixedUpdate)(laser);
    const auto address=reinterpret_cast<std::uintptr_t>(laser);
    const auto game=read<unsigned>(gameBase+0x1d1a54);
    const auto frame=game?read<unsigned>(game+0x14):0;
    if(GetCurrentThreadId()==captureThread&&cpuDebugActive&&detailedFrame(frame,0,0))
        if(FILE* output=std::fopen("native-fixed-update.jsonl","ab")){
            std::fprintf(output,"{\"frame\":%u,\"laser\":%u,\"position\":[%.9g,%.9g],\"length\":%.9g,\"width\":%.9g,\"phase\":%u,\"clock\":%d,\"durations\":[%d,%d,%d,%d]}\n",frame,unsigned(address),read<float>(address+0x60),read<float>(address+0x64),read<float>(address+0x7c),read<float>(address+0x80),read<unsigned>(address+0x1c),read<int>(address+0x28),read<int>(address+0x8b8),read<int>(address+0x8bc),read<int>(address+0x8c0),read<int>(address+0x8c4));
            std::fclose(output);
        }
    return result;
}
int __fastcall observeFixedCircle(void* laser,void*,unsigned center,unsigned radius,unsigned third,unsigned lock){
    const auto address=reinterpret_cast<std::uintptr_t>(laser);
    const auto game=read<unsigned>(gameBase+0x1d1a54);
    const auto frame=game?read<unsigned>(game+0x14):0;
    if(GetCurrentThreadId()==captureThread&&cpuDebugActive&&detailedFrame(frame,0,0))
        if(FILE* output=std::fopen("native-fixed-circle.jsonl","ab")){
            float radiusValue;std::memcpy(&radiusValue,&radius,4);
            std::fprintf(output,"{\"frame\":%u,\"present\":%u,\"laser\":%u,\"center\":[%.9g,%.9g],\"radius\":%.9g,\"position\":[%.9g,%.9g],\"length\":%.9g,\"phase\":%u}\n",frame,presentIndex+1,unsigned(address),read<float>(center),read<float>(center+4),radiusValue,read<float>(address+0x60),read<float>(address+0x64),read<float>(address+0x7c),read<unsigned>(address+0x1c));
            std::fclose(output);
        }
    using Circle=int(__thiscall*)(void*,unsigned,unsigned,unsigned,unsigned);
    return reinterpret_cast<Circle>(originalFixedCircle)(laser,center,radius,third,lock);
}
int __fastcall observeFixedCut(void* laser,void*,unsigned mask,int count){
    const auto address=reinterpret_cast<std::uintptr_t>(laser);
    const auto game=read<unsigned>(gameBase+0x1d1a54);
    const auto frame=game?read<unsigned>(game+0x14):0;
    const bool capture=GetCurrentThreadId()==captureThread&&cpuDebugActive&&detailedFrame(frame,0,0);
    const float before=capture?read<float>(address+0x7c):0;
    using Cut=int(__thiscall*)(void*,unsigned,int);
    const int result=reinterpret_cast<Cut>(originalFixedCut)(laser,mask,count);
    if(capture)if(FILE* output=std::fopen("native-fixed-cut.jsonl","ab")){
        std::fprintf(output,"{\"frame\":%u,\"present\":%u,\"laser\":%u,\"position\":[%.9g,%.9g],\"angle\":%.9g,\"before\":%.9g,\"after\":%.9g,\"phase\":%u,\"mask\":[",frame,presentIndex+1,unsigned(address),read<float>(address+0x60),read<float>(address+0x64),read<float>(address+0x78),before,read<float>(address+0x7c),read<unsigned>(address+0x1c));
        for(int i=0;i<count&&i<512;++i)std::fprintf(output,"%s%u",i?",":"",unsigned(read<unsigned char>(mask+i)));
        std::fputs("]}\n",output);std::fclose(output);
    }
    return result;
}
bool installFixedCutObserver(){
    auto* entry=reinterpret_cast<unsigned char*>(gameBase+0x12ac90);
    const unsigned char prolog[]={0x55,0x8b,0xec,0x81,0xec,0x90,0x04,0x00,0x00};
    if(std::memcmp(entry,prolog,sizeof(prolog))!=0)return false;
    auto* relay=static_cast<unsigned char*>(VirtualAlloc(nullptr,14,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE));
    if(!relay)return false;
    std::memcpy(relay,entry,9);relativeJump(relay+9,reinterpret_cast<std::uintptr_t>(entry+9));
    DWORD protection=0,ignored=0;
    if(!VirtualProtect(entry,9,PAGE_EXECUTE_READWRITE,&protection)){VirtualFree(relay,0,MEM_RELEASE);return false;}
    originalFixedCut=relay;relativeJump(entry,reinterpret_cast<std::uintptr_t>(&observeFixedCut));
    std::memset(entry+5,0x90,4);FlushInstructionCache(GetCurrentProcess(),entry,9);
    VirtualProtect(entry,9,protection,&ignored);return true;
}
bool installFixedCircleObserver(){
    auto* entry=reinterpret_cast<unsigned char*>(gameBase+0x12a040);
    const unsigned char prolog[]={0x55,0x8b,0xec,0x81,0xec,0x40,0x02,0x00,0x00};
    if(std::memcmp(entry,prolog,sizeof(prolog))!=0)return false;
    auto* relay=static_cast<unsigned char*>(VirtualAlloc(nullptr,14,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE));
    if(!relay)return false;
    std::memcpy(relay,entry,9);relativeJump(relay+9,reinterpret_cast<std::uintptr_t>(entry+9));
    DWORD protection=0,ignored=0;
    if(!VirtualProtect(entry,9,PAGE_EXECUTE_READWRITE,&protection)){VirtualFree(relay,0,MEM_RELEASE);return false;}
    originalFixedCircle=relay;relativeJump(entry,reinterpret_cast<std::uintptr_t>(&observeFixedCircle));
    std::memset(entry+5,0x90,4);FlushInstructionCache(GetCurrentProcess(),entry,9);
    VirtualProtect(entry,9,protection,&ignored);return true;
}
bool installFixedUpdateObserver(){
    auto* entry=reinterpret_cast<unsigned char*>(gameBase+0x12b550);
    const unsigned char prolog[]={0x55,0x8b,0xec,0x83,0xec,0x18};
    if(std::memcmp(entry,prolog,sizeof(prolog))!=0)return false;
    auto* relay=static_cast<unsigned char*>(VirtualAlloc(nullptr,11,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE));
    if(!relay)return false;
    std::memcpy(relay,entry,6);relativeJump(relay+6,reinterpret_cast<std::uintptr_t>(entry+6));
    DWORD protection=0,ignored=0;
    if(!VirtualProtect(entry,6,PAGE_EXECUTE_READWRITE,&protection)){VirtualFree(relay,0,MEM_RELEASE);return false;}
    originalFixedUpdate=relay;relativeJump(entry,reinterpret_cast<std::uintptr_t>(&observeFixedUpdate));
    entry[5]=0x90;FlushInstructionCache(GetCurrentProcess(),entry,6);
    VirtualProtect(entry,6,protection,&ignored);return true;
}

void* originalShot;
struct ShotGate {
    unsigned timer, seat, warmup, emitClock, seatFlags, gameFlags, stageReady, frozen;
};
ShotGate shotGateSamples[512];
unsigned shotGateCount, shotGateFlushed;
struct PlayerSample {unsigned owner, timer, x, y, controls, bulletManager;};
PlayerSample playerSamples[2];
bool captureBulletPositions;
struct ProjectileSample {unsigned timer,seat,projectile,vm,age,x,y,angle,speed,kind,damage;};
ProjectileSample projectileSamples[16384];
unsigned projectileCount,projectileFlushed;
struct EnemySample {unsigned timer,id,x,y,flags,status;int hp;unsigned seat;};
EnemySample enemySamples[8192];
unsigned enemyCount,enemyFlushed;

void __cdecl recordShotGate(unsigned shot) {
    if (GetCurrentThreadId()!=captureThread) return;
    const auto game=read<unsigned>(gameBase+0x1d1a54);
    if(!game) return;
    const auto timer=read<unsigned>(game+0x14);
    const auto ctx=read<unsigned>(shot+0x1293c);
    const auto status=ctx?read<unsigned>(ctx+4):0;
    const auto seat=read<unsigned>(shot+0x12938);
    // The shot manager receives the player's field position after movement.
    if(status&&seat<2){
        const auto input=read<unsigned>(gameBase+0x1d19a0+4*seat);
        playerSamples[seat]={game,timer,read<unsigned>(status+0x68c),read<unsigned>(status+0x690),
            input?read<unsigned>(input+0x29c)&0xffu:0x100u,read<unsigned>(ctx)};
    }
    if(!detailedFrame(timer,0,130)) return;
    if(cpuDebugActive&&ctx&&seat<2){
        const auto manager=read<unsigned>(ctx+8);
        auto node=manager?read<unsigned>(manager+0x4b50):0;
        if(FILE* output=std::fopen("native-enemy-motion.jsonl","ab")){
            for(unsigned i=0;node&&i<1024;++i){
                const auto enemy=read<unsigned>(node);
                if(!enemy)break;
                std::fprintf(output,"{\"frame\":%u,\"seat\":%u,\"id\":%u,\"position\":[%u,%u],\"spawn\":[%u,%u],\"hp\":%d,\"viewport\":[%d,%d],\"motion\":[",timer,seat,read<unsigned>(enemy+0x6858),read<unsigned>(enemy+0x1278),read<unsigned>(enemy+0x127c),read<unsigned>(enemy+0x6880),read<unsigned>(enemy+0x6884),read<int>(enemy+0x6238),read<int>(gameBase+0x22ce48),read<int>(gameBase+0x22ce4c));
                // Enemy::update receives the gameplay record at allocation +0x1230.
                for(unsigned word=0;word<20;++word)std::fprintf(output,"%s%u",word?",":"",read<unsigned>(enemy+0x12c0+word*4));
                std::fputs("]}\n",output);
                node=read<unsigned>(node+4);
            }
            std::fclose(output);
        }
    }
    if(shotGateCount>=_countof(shotGateSamples))return;
    auto& sample=shotGateSamples[shotGateCount++];
    const auto stage=read<unsigned>(gameBase+0x1d1a68);
    const auto director=read<unsigned>(gameBase+0x1d1a50);
    sample={timer,seat,read<unsigned>(shot+0x1282c),
        read<unsigned>(shot+0x12804),status?read<unsigned>(status+0x14):0,
        read<unsigned>(game+0x140),stage?read<unsigned>(stage+0x4b78):0,
        director?read<unsigned>(director+0x310):0};
    if(detailedFrame(timer,84,130)&&sample.seat<2&&ctx){
        auto manager=read<unsigned>(ctx+8);
        auto node=manager?read<unsigned>(manager+0x4b50):0;
        for(unsigned i=0;node&&i<24&&enemyCount<_countof(enemySamples);++i){
            const auto enemy=read<unsigned>(node);
            if(!enemy)break;
            // 0x509e68 subtracts damage from the +0x1230 subobject at +0x5008.
            enemySamples[enemyCount++]={timer,read<unsigned>(enemy+0x6858),read<unsigned>(enemy+0x1278),
                read<unsigned>(enemy+0x127c),read<unsigned>(enemy+0x639c),read<unsigned>(enemy+0x63a0),read<int>(enemy+0x6238),sample.seat};
            node=read<unsigned>(node+4);
        }
    }
    if(detailedFrame(timer,86,130)&&sample.seat<2) {
        auto node=read<unsigned>(shot+0x12840);
        for(unsigned i=0;node&&i<96&&projectileCount<_countof(projectileSamples);++i) {
            const auto projectile=read<unsigned>(node);
            if(!projectile)break;
            if(cpuDebugActive){
                const auto vmId=read<unsigned>(projectile+0x14);
                using FindVm=unsigned(__thiscall*)(void*,unsigned);
                const auto vm=reinterpret_cast<FindVm>(gameBase+0xcb8b0)(reinterpret_cast<void*>(read<unsigned>(gameBase+0x1d19bc)),vmId);
                if(vm)if(FILE* output=std::fopen("native-shot-vertices.jsonl","ab")){
                    float vertices[12]{};
                    using Quad=void(__thiscall*)(void*,float*);
                    reinterpret_cast<Quad>(gameBase+0xca2e0)(reinterpret_cast<void*>(vm),vertices);
                    std::fprintf(output,"{\"frame\":%u,\"seat\":%u,\"shot\":%u,\"age\":%u,\"position\":[%.9g,%.9g],\"viewport\":[%d,%d],\"vertices\":[",timer,seat,projectile,read<unsigned>(projectile+0x1c),read<float>(projectile+0x54),read<float>(projectile+0x58),read<int>(gameBase+0x22ce48),read<int>(gameBase+0x22ce4c));
                    for(unsigned j=0;j<12;++j)std::fprintf(output,"%s%.9g",j?",":"",vertices[j]);
                    std::fputs("]}\n",output);std::fclose(output);
                }
            }
            projectileSamples[projectileCount++]={timer,sample.seat,projectile,read<unsigned>(projectile+0x14),
                read<unsigned>(projectile+0x1c),read<unsigned>(projectile+0x54),read<unsigned>(projectile+0x58),
                read<unsigned>(projectile+0x70),read<unsigned>(projectile+0x6c),read<unsigned>(projectile+0x9c),read<unsigned>(projectile+0xac)};
            node=read<unsigned>(node+4);
        }
    }

}
__declspec(naked) void observeShotGate() {
    __asm {
        pushfd
        pushad
        mov ebx, esp
        and esp, -16
        sub esp, 512
        fxsave [esp]
        push ecx
        call recordShotGate
        add esp, 4
        fxrstor [esp]
        mov esp, ebx
        popad
        popfd
        jmp dword ptr [originalShot]
    }
}
bool installShotObserver() {
    auto* entry=reinterpret_cast<unsigned char*>(gameBase+0x14f7c0);
    const unsigned char signature[]={0xa1,0x50,0x1a,0x5d,0x00,0x56,0x8b,0xf1};
    const auto moved=gameBase+0x1d1a50;
    if(entry[0]!=signature[0] || read<unsigned>(reinterpret_cast<unsigned>(entry+1))!=moved ||
        std::memcmp(entry+5,signature+5,3)!=0) return false;
    auto* relay=static_cast<unsigned char*>(VirtualAlloc(nullptr,10,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));
    if(!relay) return false;
    std::memcpy(relay,entry,5);
    relativeJump(relay+5,reinterpret_cast<unsigned>(entry+5));
    DWORD protection, ignored;
    if(!VirtualProtect(relay,10,PAGE_EXECUTE_READ,&ignored) ||
        !VirtualProtect(entry,5,PAGE_EXECUTE_READWRITE,&protection)) return false;
    FlushInstructionCache(GetCurrentProcess(),relay,10);
    originalShot=relay;
    relativeJump(entry,reinterpret_cast<unsigned>(observeShotGate));
    FlushInstructionCache(GetCurrentProcess(),entry,5);
    VirtualProtect(entry,5,protection,&ignored);
    return true;
}
void* originalImpact;
struct ImpactSample {unsigned timer,seat,vm,x,y,kind,age;};
ImpactSample impactSamples[256];
unsigned impactCount,impactFlushed;
void __cdecl recordImpact(unsigned shot,unsigned caller) {
    if(GetCurrentThreadId()!=captureThread || impactCount>=_countof(impactSamples))return;
    const auto game=read<unsigned>(gameBase+0x1d1a54);
    if(!game)return;
    const auto frame=read<unsigned>(game+0x14);
    if(!detailedFrame(frame,100,130))return;
    if(cpuDebugActive)if(FILE* output=std::fopen("native-impact-detail.jsonl","ab")){
        std::fprintf(output,"{\"frame\":%u,\"shot\":%u,\"caller\":%u,\"age\":%d,\"x\":%u,\"y\":%u}\n",frame,shot,caller-gameBase,read<int>(shot+0x1c),read<unsigned>(shot+0x54),read<unsigned>(shot+0x58));
        std::fclose(output);
    }
    impactSamples[impactCount++]={frame,read<unsigned>(shot+0x11c),read<unsigned>(shot+0x14),
        read<unsigned>(shot+0x54),read<unsigned>(shot+0x58),read<unsigned>(shot+0x9c),
        read<unsigned>(shot+0x114)};
}
__declspec(naked) void observeImpact() {
    __asm {
        pushfd
        pushad
        mov ebx, esp
        and esp, -16
        sub esp, 512
        fxsave [esp]
        push dword ptr [ebx + 36]
        push ecx
        call recordImpact
        add esp, 8
        fxrstor [esp]
        mov esp, ebx
        popad
        popfd
        jmp dword ptr [originalImpact]
    }
}
bool installImpactObserver() {
    auto* entry=reinterpret_cast<unsigned char*>(gameBase+0x14fba0);
    const unsigned char signature[]={0x56,0x8b,0xf1,0x80,0xbe,0x14,0x01,0x00,0x00,0x00};
    constexpr unsigned size=sizeof(signature);
    if(std::memcmp(entry,signature,size)!=0)return false;
    auto* relay=static_cast<unsigned char*>(VirtualAlloc(nullptr,size+5,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));
    if(!relay)return false;
    std::memcpy(relay,entry,size);
    relativeJump(relay+size,reinterpret_cast<unsigned>(entry+size));
    DWORD protection, ignored;
    if(!VirtualProtect(relay,size+5,PAGE_EXECUTE_READ,&ignored)||
       !VirtualProtect(entry,size,PAGE_EXECUTE_READWRITE,&protection))return false;
    FlushInstructionCache(GetCurrentProcess(),relay,size+5);
    originalImpact=relay;
    relativeJump(entry,reinterpret_cast<unsigned>(observeImpact));
    std::memset(entry+5,0x90,size-5);
    FlushInstructionCache(GetCurrentProcess(),entry,size);
    VirtualProtect(entry,size,protection,&ignored);
    return true;
}
void* originalRect;
struct RectSample {unsigned timer,area,flags;float x,y,w,h,angle,ex,ey,ew,eh,ea;};
RectSample rectSamples[16384];
unsigned rectCount,rectFlushed;
void __cdecl recordRect(unsigned area,const unsigned* stack,float enemyAngle) {
    if(GetCurrentThreadId()!=captureThread||rectCount>=_countof(rectSamples))return;
    const auto game=read<unsigned>(gameBase+0x1d1a54);
    if(!game)return;
    const auto frame=read<unsigned>(game+0x14);
    if(!detailedFrame(frame,111,130)||!stack[1])return;
    const float x=read<float>(area+0x34),y=read<float>(area+0x38);
    if(!hasTraceWindow&&(x>-70.f||x<-145.f||y<235.f||y>335.f))return;
    rectSamples[rectCount++]={frame,area,read<unsigned>(area+0x14),x,y,read<float>(area+0x2c),read<float>(area+0x30),
        read<float>(area+0x24),read<float>(stack[1]),read<float>(stack[1]+4),stack[2]?read<float>(stack[2]):0.f,stack[2]?read<float>(stack[2]+4):read<float>(reinterpret_cast<unsigned>(stack+3)),enemyAngle};
}
__declspec(naked) void observeRect() {
    __asm {
        pushfd
        pushad
        mov ebx, esp
        and esp, -16
        sub esp, 512
        fxsave [esp]
        movd eax, xmm3
        lea edx, [ebx + 36]
        push eax
        push edx
        push ecx
        call recordRect
        add esp, 12
        fxrstor [esp]
        mov esp, ebx
        popad
        popfd
        jmp dword ptr [originalRect]
    }
}
bool installRectObserver() {
    auto* entry=reinterpret_cast<unsigned char*>(gameBase+0x11e670);
    const unsigned char signature[]={0x55,0x8b,0xec,0x8b,0x41,0x14};
    constexpr unsigned size=sizeof(signature);
    if(std::memcmp(entry,signature,size)!=0)return false;
    auto* relay=static_cast<unsigned char*>(VirtualAlloc(nullptr,size+5,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));
    if(!relay)return false;
    std::memcpy(relay,entry,size);
    relativeJump(relay+size,reinterpret_cast<unsigned>(entry+size));
    DWORD protection, ignored;
    if(!VirtualProtect(relay,size+5,PAGE_EXECUTE_READ,&ignored)||
       !VirtualProtect(entry,size,PAGE_EXECUTE_READWRITE,&protection))return false;
    FlushInstructionCache(GetCurrentProcess(),relay,size+5);
    originalRect=relay;
    relativeJump(entry,reinterpret_cast<unsigned>(observeRect));
    entry[5]=0x90;
    FlushInstructionCache(GetCurrentProcess(),entry,size);
    VirtualProtect(entry,size,protection,&ignored);
    return true;
}
bool installRngObserver() {
    const char* first=std::getenv("TH19_CAPTURE_TRACE_FIRST");
    const char* last=std::getenv("TH19_CAPTURE_TRACE_LAST");
    if (first && last) {
        traceFirst=std::strtoul(first,nullptr,10);
        traceLast=std::strtoul(last,nullptr,10);
        hasTraceWindow=traceFirst<=traceLast;
    }
    const unsigned char signature[] = {0x55,0x8b,0xec,0x83,0xec,0x24,0x53,0x56,0x57,0x8b,0xf1};
    auto* entry = reinterpret_cast<unsigned char*>(gameBase + 0x2540);
    if (std::memcmp(entry, signature, sizeof(signature)) != 0) return false;
    rngTrace = _fsopen("native-rng-calls.jsonl", "wb", _SH_DENYWR);
    if (!rngTrace) return false;
    constexpr unsigned displaced = 6;
    auto* relay = static_cast<unsigned char*>(VirtualAlloc(nullptr, displaced + 5,
        MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
    if (!relay) return false;
    std::memcpy(relay, entry, displaced);
    relativeJump(relay + displaced, reinterpret_cast<std::uintptr_t>(entry + displaced));
    DWORD protection, ignored;
    if (!VirtualProtect(relay, displaced + 5, PAGE_EXECUTE_READ, &ignored) ||
        !VirtualProtect(entry, displaced, PAGE_EXECUTE_READWRITE, &protection)) return false;
    FlushInstructionCache(GetCurrentProcess(), relay, displaced + 5);
    originalRng = relay;
    captureThread = GetCurrentThreadId();
    relativeJump(entry, reinterpret_cast<std::uintptr_t>(observeRng));
    entry[5] = 0x90;
    FlushInstructionCache(GetCurrentProcess(), entry, displaced);
    VirtualProtect(entry, displaced, protection, &ignored);
    return true;
}

void flushRngCalls() {
    if (!rngTrace) return;
    for (; rngCallsFlushed < rngCallCount; ++rngCallsFlushed) {
        const auto& sample = rngCalls[rngCallsFlushed];
        std::fprintf(rngTrace, "{\"present\":%u,\"timer\":%u,\"gameFlags\":%u,\"rngObjectRva\":%u,"
                              "\"before\":%u,\"callerRva\":%u,\"bp\":%u,\"si\":%u,\"di\":%u,\"stack\":[",
            sample.present, sample.frame, sample.flags, sample.object, sample.before,
            sample.caller, sample.bp, sample.si, sample.di);
        for (unsigned i = 0; i < _countof(sample.stack); ++i)
            std::fprintf(rngTrace, "%s%u", i ? "," : "", sample.stack[i]);

        if (sample.frameWords[1]) {
            std::fputs("],\"frameWords\":[", rngTrace);
            for (unsigned i = 0; i < _countof(sample.frameWords); ++i)
                std::fprintf(rngTrace, "%s%u", i ? "," : "", sample.frameWords[i]);
            std::fputs("],\"instructionWords\":[", rngTrace);
            for (unsigned i = 0; i < _countof(sample.instructionWords); ++i)
                std::fprintf(rngTrace, "%s%u", i ? "," : "", sample.instructionWords[i]);
        }

        if (sample.subContext[2]) {
            std::fputs("],\"subContext\":[", rngTrace);
            for (unsigned i = 0; i < _countof(sample.subContext); ++i)
                std::fprintf(rngTrace, "%s%u", i ? "," : "", sample.subContext[i]);
            std::fprintf(rngTrace, "],\"subIndex\":%u,\"ip\":%u,\"subNameBytes\":[", sample.subIndex, sample.ip);
            for (unsigned i = 0; i < _countof(sample.subName) && (i==0 || sample.subName[i-1]); ++i)
                std::fprintf(rngTrace, "%s%u", i ? "," : "", static_cast<unsigned char>(sample.subName[i]));
        }
        if(sample.animation[0]){
            std::fputs("],\"animation\":[",rngTrace);
            for(unsigned i=0;i<_countof(sample.animation);++i)
                std::fprintf(rngTrace,"%s%u",i?",":"",sample.animation[i]);
        }
        std::fputc(']', rngTrace);
        if(sample.counterSlot)
            std::fprintf(rngTrace,",\"counterSlot\":{\"managerIndex\":%u,\"address\":%u,\"flags\":%u}",
                sample.counterManagerIndex,sample.counterSlot,sample.counterSlotFlags);
        std::fputs("}\n", rngTrace);
    }
    std::fflush(rngTrace);
}

bool saveBackbuffer(IDirect3DDevice9* device, const char* path) {
    if(stateOnlyDrawActive)return false;
    IDirect3DSurface9* back = nullptr;
    if (FAILED(device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &back))) return false;
    D3DSURFACE_DESC desc{};
    back->GetDesc(&desc);
    IDirect3DSurface9* resolved = nullptr;
    IDirect3DSurface9* memory = nullptr;
    D3DLOCKED_RECT locked{};
    bool success = false;
    if (desc.Format == D3DFMT_A8R8G8B8 || desc.Format == D3DFMT_X8R8G8B8) {
        HRESULT status = S_OK;
        if (desc.MultiSampleType != D3DMULTISAMPLE_NONE) {
            status = device->CreateRenderTarget(desc.Width, desc.Height, desc.Format,
                D3DMULTISAMPLE_NONE, 0, FALSE, &resolved, nullptr);
            if (SUCCEEDED(status)) status = device->StretchRect(back, nullptr, resolved, nullptr, D3DTEXF_NONE);
        }
        if (SUCCEEDED(status)) status = device->CreateOffscreenPlainSurface(
            desc.Width, desc.Height, desc.Format, D3DPOOL_SYSTEMMEM, &memory, nullptr);
        if (SUCCEEDED(status)) status = device->GetRenderTargetData(resolved ? resolved : back, memory);
        if (SUCCEEDED(status) && SUCCEEDED(memory->LockRect(&locked, nullptr, D3DLOCK_READONLY))) {
            FILE* file = nullptr;
            if (fopen_s(&file, path, "wb") == 0) {
                const auto stride = (desc.Width * 3u + 3u) & ~3u;
                BITMAPFILEHEADER header{};
                BITMAPINFOHEADER info{};
                header.bfType = 0x4d42;
                header.bfOffBits = sizeof(header) + sizeof(info);
                header.bfSize = header.bfOffBits + stride * desc.Height;
                info.biSize = sizeof(info);
                info.biWidth = static_cast<LONG>(desc.Width);
                info.biHeight = -static_cast<LONG>(desc.Height);
                info.biPlanes = 1;
                info.biBitCount = 24;
                std::vector<unsigned char> row(stride);
                success = fwrite(&header, sizeof(header), 1, file) == 1 &&
                          fwrite(&info, sizeof(info), 1, file) == 1;
                for (unsigned y = 0; success && y < desc.Height; ++y) {
                    const auto* pixels = static_cast<const unsigned char*>(locked.pBits) + y * locked.Pitch;
                    for (unsigned x = 0; x < desc.Width; ++x) std::memcpy(&row[x * 3], &pixels[x * 4], 3);
                    success = fwrite(row.data(), 1, stride, file) == stride;
                }
                success = fclose(file) == 0 && success;
            }
            memory->UnlockRect();
        }
    }
    if (memory) memory->Release();
    if (resolved) resolved->Release();
    back->Release();
    return success;
}

void writeBulletPositions(FILE* output, unsigned manager) {
    // 0x40e0fb starts the active list and 0x40e0b3 advances it.
    auto bullet = read<unsigned>(manager+0x8156d8);
    const auto owner=read<unsigned>(gameBase+0x1d1a54);
    const auto frame=owner?read<unsigned>(owner+0x14):0;
    FILE* detail=nullptr;
    if(cpuDebugActive&&detailedFrame(frame,0,130))detail=std::fopen("native-bullet-detail.jsonl","ab");
    std::fputc('[', output);
    bool first = true;
    for (unsigned i=0; bullet && i<2000; ++i, bullet=read<unsigned>(bullet+4)) {
        const auto state = read<unsigned>(bullet+0x1044);
        if (state!=1 && state!=2) continue; // 0x4ee3e9 selects live bullets.
        if(detail)std::fprintf(detail,"{\"frame\":%u,\"manager\":%u,\"bullet\":%u,\"kind\":%u,\"color\":%u,\"position\":[%.9g,%.9g],\"radius\":%.9g,\"scale\":%.9g,\"flags\":%u,\"changes\":%u,\"velocity\":[%.9g,%.9g],\"speed\":%.9g,\"angle\":%.9g}\n",frame,manager,bullet,unsigned(read<unsigned short>(bullet+0x1078)),unsigned(read<unsigned short>(bullet+0x107a)),read<float>(bullet+0x650),read<float>(bullet+0x654),read<float>(bullet+0x674),read<float>(bullet+0xfd4),read<unsigned>(bullet+0x10),read<unsigned>(bullet+0x6a8),read<float>(bullet+0x65c),read<float>(bullet+0x660),read<float>(bullet+0x668),read<float>(bullet+0x670));
        std::fprintf(output,"%s[%u,%u,%u,%u]",first?"":",",
            unsigned(read<unsigned short>(bullet+0x1078)),unsigned(read<unsigned short>(bullet+0x107a)),
            read<unsigned>(bullet+0x650),read<unsigned>(bullet+0x654));
        first = false;
    }
    std::fputc(']', output);
    if(detail)std::fclose(detail);
}

void capture(IDirect3DDevice9* device) {
    const unsigned present = ++presentIndex;
    if (!trace) return;
    flushRngCalls();
    const auto owner = read<std::uint32_t>(gameBase + 0x1d1a54);
    if (!owner) {
        if (present == 360 || present == 900 || present == 1800 || present == 2700) {
            char name[80]{};
            sprintf_s(name, "native-menu-present-%u.bmp", present);
            if (saveBackbuffer(device, name)) std::fprintf(trace, "{\"present\":%u,\"image\":\"%s\"}\n", present, name);
        }
        if (priorOwner) { priorOwner = 0; ++generation; }
        std::fflush(trace);
        return;
    }
    const auto frame = read<std::uint32_t>(owner + 0x14);
    if (frame >= 10000000) return;
    if (priorOwner && (priorOwner != owner || frame < priorFrame)) ++generation;
    priorOwner = owner;
    priorFrame = frame;
    if (imageSegment != generation) {
        imageSegment = generation;
        std::memset(savedImage, 0, sizeof(savedImage));
        // Ju.N.Owen 1.10c's in-game GameSettings has one common and two seat words.
        const auto settings = gameBase + 0x22bb10;
        std::fprintf(trace, "{\"segment\":%u,\"settingsWords\":[%u,%u,%u]}\n", generation,
            read<std::uint32_t>(settings), read<std::uint32_t>(settings + 4), read<std::uint32_t>(settings + 8));
    }
    const auto rng = gameBase + 0x1c6624;
    std::fprintf(trace, "{\"present\":%u,\"segment\":%u,\"frame\":%u,\"previous\":%u,"
                        "\"clockBits\":%u,\"flags\":%u,\"gameFlags\":%u,\"rng\":[%u,%u,%u,%u]",
        present, generation, frame, read<std::uint32_t>(owner + 0x10),
        read<std::uint32_t>(owner + 0x18), read<std::uint32_t>(owner + 0x20),
        read<std::uint32_t>(owner + 0x140), read<std::uint32_t>(rng),
        read<std::uint32_t>(rng+16), read<std::uint32_t>(rng+24), read<std::uint32_t>(rng+40));
    if (rngTrace && frame == 0 && read<unsigned>(owner + 0x10) != frame) {
        std::fprintf(trace, ",\"waveRng\":[%u,%u,%u],\"waveDraws\":[",
            read<unsigned>(owner + 0x16c), read<unsigned>(owner + 0x178), read<unsigned>(owner + 0x17c));
        for (unsigned j = 0; j < 256; ++j)
            std::fprintf(trace, "%s%u", j ? "," : "", read<unsigned>(gameBase + 0x22b674 + j * 4));
        std::fputc(']', trace);
    }
    const auto& left=playerSamples[0];
    const auto& right=playerSamples[1];
    if(rngTrace&&left.owner==owner&&right.owner==owner&&left.timer==frame&&right.timer==frame){
        std::fprintf(trace,",\"playerPositionBits\":[%u,%u,%u,%u]",left.x,left.y,right.x,right.y);
        if(left.controls<=0xffu&&right.controls<=0xffu)
            std::fprintf(trace,",\"playerControls\":[%u,%u]",left.controls,right.controls);
        if(captureBulletPositions&&left.bulletManager&&right.bulletManager){
            std::fputs(",\"bulletPositions\":[",trace);
            writeBulletPositions(trace,left.bulletManager);
            std::fputc(',',trace);
            writeBulletPositions(trace,right.bulletManager);
            std::fputc(']',trace);
        }
    }
    if (rngTrace && shotGateFlushed < shotGateCount) {
        std::fputs(",\"shotContexts\":[",trace);
        bool first=true;
        for(;shotGateFlushed<shotGateCount;++shotGateFlushed) {
            const auto& g=shotGateSamples[shotGateFlushed];
            std::fprintf(trace,"%s[%u,%u,%u,%u,%u,%u,%u,%u]",first?"":",",
                g.timer,g.seat,g.warmup,g.emitClock,g.seatFlags,g.gameFlags,g.stageReady,g.frozen);
            first=false;
        }
        std::fputc(']',trace);
    }
    if(rngTrace&&rectFlushed<rectCount){
        std::fputs(",\"shotRectangles\":[",trace);
        bool first=true;
        for(;rectFlushed<rectCount;++rectFlushed){
            const auto& r=rectSamples[rectFlushed];
            std::fprintf(trace,"%s[%u,%u,%u,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g]",first?"":",",
                r.timer,r.area,r.flags,r.x,r.y,r.w,r.h,r.angle,r.ex,r.ey,r.ew,r.eh,r.ea);
            first=false;
        }
        std::fputc(']',trace);
    }
    if(rngTrace&&enemyFlushed<enemyCount){
        std::fputs(",\"enemiesBeforeShots\":[",trace);
        bool first=true;
        for(;enemyFlushed<enemyCount;++enemyFlushed){
            const auto& g=enemySamples[enemyFlushed];
            std::fprintf(trace,"%s[%u,%u,%u,%u,%u,%u,%d,%u]",first?"":",",g.timer,g.id,g.x,g.y,g.flags,g.status,g.hp,g.seat);
            first=false;
        }
        std::fputc(']',trace);
    }
    if(rngTrace&&projectileFlushed<projectileCount){
        std::fputs(",\"projectilesBeforeUpdate\":[",trace);
        bool first=true;
        for(;projectileFlushed<projectileCount;++projectileFlushed){
            const auto& g=projectileSamples[projectileFlushed];
            std::fprintf(trace,"%s[%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u]",first?"":",",
                g.timer,g.seat,g.projectile,g.vm,g.age,g.x,g.y,g.angle,g.speed,g.kind,g.damage);
            first=false;
        }
        std::fputc(']',trace);
    }
    if (rngTrace && impactFlushed<impactCount) {
        std::fputs(",\"impacts\":[",trace);
        bool first=true;
        for(;impactFlushed<impactCount;++impactFlushed){
            const auto& g=impactSamples[impactFlushed];
            std::fprintf(trace,"%s[%u,%u,%u,%u,%u,%u,%u]",first?"":",",
                g.timer,g.seat,g.vm,g.x,g.y,g.kind,g.age);
            first=false;
        }
        std::fputc(']',trace);
    }
    if (rngTrace && frame == 11) std::fprintf(trace, ",\"rngCalls\":%u,\"rngCallsOnOtherThreads\":%ld",
        rngCallCount, InterlockedCompareExchange(&skippedRngThreads, 0, 0));
    if (rngTrace && frame <= 2 && frame != read<unsigned>(owner + 0x10)) {
        char name[80]{};
        sprintf_s(name, "native-segment-%u-frame-%u.bmp", generation, frame);
        if (GetFileAttributesA(name) == INVALID_FILE_ATTRIBUTES && saveBackbuffer(device, name))
            std::fprintf(trace, ",\"image\":\"%s\"", name);
    }
    for (unsigned i = 0; i < sizeof(imageFrames) / sizeof(imageFrames[0]); ++i) {
        if (frame != imageFrames[i] || savedImage[i]) continue;
        char name[80]{};
        sprintf_s(name, "native-segment-%u-frame-%u.bmp", generation, frame);
        savedImage[i] = saveBackbuffer(device, name);
        if (savedImage[i]) std::fprintf(trace, ",\"image\":\"%s\"", name);
    }
    for (unsigned wanted : extraImageFrames) {
        if(frame!=wanted)continue;
        char name[80]{};
        sprintf_s(name,"native-segment-%u-frame-%u.bmp",generation,frame);
        if(GetFileAttributesA(name)==INVALID_FILE_ATTRIBUTES&&saveBackbuffer(device,name))
            std::fprintf(trace,",\"image\":\"%s\"",name);
    }
    std::fputs("}\n", trace);
    std::fflush(trace);
}

bool enableFastStateCapture() {
    // Bypass the wall-clock wait while leaving the simulation step intact.
    auto* begin=reinterpret_cast<unsigned char*>(gameBase+0xb5cf2);
    auto* second=reinterpret_cast<unsigned char*>(gameBase+0xb5ee5);
    auto* third=reinterpret_cast<unsigned char*>(gameBase+0xb605e);
    auto* gate=reinterpret_cast<unsigned char*>(gameBase+0xb6078);
    const unsigned char waitGate[]={0x0f,0x86,0xac,0x01,0x00,0x00};
    for(const auto* site:{begin,second,third}){
        if(site[0]!=0xff||site[1]!=0x15||read<unsigned>(unsigned(site+2))!=gameBase+0x17b0e0)
            return false;
    }
    if(std::memcmp(gate,waitGate,sizeof(waitGate)))return false;
    const SIZE_T size=gate+sizeof(waitGate)-begin;
    DWORD old,ignored;
    if(!VirtualProtect(begin,size,PAGE_EXECUTE_READWRITE,&old))return false;
    const unsigned char skipSleep[]={0x83,0xc4,0x04,0x90,0x90,0x90};
    for(auto* site:{begin,second,third})std::memcpy(site,skipSleep,sizeof(skipSleep));
    // 0x4b60b9 enters the update path and skips catch-up against wall time.
    const unsigned char skipGate[]={0xe9,0x3c,0x00,0x00,0x00,0x90};
    std::memcpy(gate,skipGate,sizeof(skipGate));
    FlushInstructionCache(GetCurrentProcess(),begin,size);
    VirtualProtect(begin,size,old,&ignored);
    return true;
}

HRESULT STDMETHODCALLTYPE observeDrawPrimitive(IDirect3DDevice9* self,D3DPRIMITIVETYPE type,UINT start,UINT count){
    return stateOnlyDrawActive?D3D_OK:originalDrawPrimitive(self,type,start,count);
}
HRESULT STDMETHODCALLTYPE observeDrawIndexedPrimitive(IDirect3DDevice9* self,D3DPRIMITIVETYPE type,INT base,UINT minimum,UINT vertices,UINT start,UINT count){
    return stateOnlyDrawActive?D3D_OK:originalDrawIndexedPrimitive(self,type,base,minimum,vertices,start,count);
}
HRESULT STDMETHODCALLTYPE observeDrawPrimitiveUP(IDirect3DDevice9* self,D3DPRIMITIVETYPE type,UINT count,const void* vertices,UINT stride){
    return stateOnlyDrawActive?D3D_OK:originalDrawPrimitiveUP(self,type,count,vertices,stride);
}
HRESULT STDMETHODCALLTYPE observeDrawIndexedPrimitiveUP(IDirect3DDevice9* self,D3DPRIMITIVETYPE type,UINT minimum,UINT vertices,UINT count,const void* indices,D3DFORMAT format,const void* data,UINT stride){
    return stateOnlyDrawActive?D3D_OK:originalDrawIndexedPrimitiveUP(self,type,minimum,vertices,count,indices,format,data,stride);
}

HRESULT STDMETHODCALLTYPE observePresent(IDirect3DDevice9* self, const RECT* source, const RECT* dest,
                                          HWND window, const RGNDATA* dirty) {
    capture(self);
    const auto owner=read<unsigned>(gameBase+0x1d1a54);
    static const bool stateOnly=GetEnvironmentVariableA("TH19_CAPTURE_STATE_ONLY",nullptr,0)>0;
    static const unsigned renderFrom=[](){char value[32]{};GetEnvironmentVariableA("TH19_CAPTURE_RENDER_FROM",value,sizeof(value));return unsigned(std::strtoul(value,nullptr,10));}();
    stateOnlyDrawActive=owner&&(read<unsigned>(owner+0x140)&0x4000000u)&&
        (stateOnly||(renderFrom&&read<unsigned>(owner+0x14)+1<renderFrom));
    static const bool requested=GetEnvironmentVariableA("TH19_CAPTURE_FAST_STATE",nullptr,0)>0;
    if(requested){
        const auto owner=read<unsigned>(gameBase+0x1d1a54);
        if(owner&&(read<unsigned>(owner+0x140)&0x4000000u)){
            static const bool active=[](){
                const bool result=enableFastStateCapture();
                std::fprintf(trace,"{\"fastStateActive\":%s}\n",result?"true":"false");
                std::fflush(trace);
                return result;
            }();
            if(active&&(stateOnlyDrawActive||read<unsigned>(owner+0x14)%60!=0))return D3D_OK;
        }
    }
    return originalPresent(self, source, dest, window, dirty);
}

HRESULT STDMETHODCALLTYPE observeDisplayMode(IDirect3D9* self, UINT adapter, D3DDISPLAYMODE* mode) {
    const HRESULT result = originalDisplayMode(self, adapter, mode);
    if (FAILED(result) && mode && adapter == D3DADAPTER_DEFAULT &&
        GetEnvironmentVariableA("TH19_CAPTURE_DISPLAY_FALLBACK", nullptr, 0)>0) {
        DEVMODEW current{};
        current.dmSize = sizeof(current);
        const BOOL enumerated = EnumDisplaySettingsW(nullptr, ENUM_CURRENT_SETTINGS, &current);
        const DWORD enumerationError = enumerated ? 0 : GetLastError();
        if (trace) {
            std::fprintf(trace, "{\"displayModeFallbackAttempt\":true,\"enumerated\":%s,\"error\":%lu,\"bitsPerPixel\":%lu,\"width\":%lu,\"height\":%lu}\n",
                enumerated?"true":"false", enumerationError, current.dmBitsPerPel, current.dmPelsWidth, current.dmPelsHeight);
            std::fflush(trace);
        }
        if (enumerated &&
            current.dmBitsPerPel == 32 && current.dmPelsWidth && current.dmPelsHeight) {
            mode->Width = current.dmPelsWidth;
            mode->Height = current.dmPelsHeight;
            mode->RefreshRate = current.dmDisplayFrequency;
            mode->Format = D3DFMT_X8R8G8B8;
            if (trace) {
                std::fprintf(trace, "{\"displayModeFallback\":true,\"originalResult\":%ld,\"width\":%u,\"height\":%u,\"refresh\":%u,\"bitsPerPixel\":%lu}\n",
                    long(result), mode->Width, mode->Height, mode->RefreshRate, current.dmBitsPerPel);
                std::fflush(trace);
            }
            return D3D_OK;
        }
    }
    if (trace && mode) {
        std::fprintf(trace, "{\"displayMode\":{\"result\":%ld,\"width\":%u,\"height\":%u,\"refresh\":%u,\"format\":%u}}\n",
            long(result), SUCCEEDED(result)?mode->Width:0, SUCCEEDED(result)?mode->Height:0,
            SUCCEEDED(result)?mode->RefreshRate:0, SUCCEEDED(result)?unsigned(mode->Format):0);
        std::fflush(trace);
    }
    return result;
}

using CpuObstacleCall=unsigned(__fastcall*)(unsigned,unsigned);
CpuObstacleCall originalCpuObstacle;
unsigned __fastcall observeCpuObstacle(unsigned self,unsigned unused){
    const unsigned selected=originalCpuObstacle(self,unused);
    const auto game=read<unsigned>(gameBase+0x1d1a54);
    const auto frame=game?read<unsigned>(game+0x14):0;
    if(detailedFrame(frame,0,0))if(FILE* output=std::fopen("native-cpu-obstacles.jsonl","ab")){
        const auto count=read<unsigned>(self+0x36ee84);
        std::fprintf(output,"{\"frame\":%u,\"cpu\":%u,\"position\":[%u,%u],\"selected\":%d,\"objects\":[",frame,self,
            read<unsigned>(self+0x36ee88),read<unsigned>(self+0x36ee8c),selected?int((selected-self-4)/60):-1);
        for(unsigned i=0;i<count&&i<30000;++i){
            std::fputs(i?",[":"[",output);
            for(unsigned word=0;word<15;++word)std::fprintf(output,"%s%u",word?",":"",read<unsigned>(self+4+i*60+word*4));
            std::fputc(']',output);
        }
        std::fputs("]}\n",output);std::fclose(output);
    }
    return selected;
}
bool installCpuObstacleObserver(){
    auto* entry=reinterpret_cast<unsigned char*>(gameBase+0xf9530);
    const unsigned char signature[]={0x55,0x8b,0xec,0x83,0xe4,0xc0,0x83,0xec,0x34};
    if(std::memcmp(entry,signature,sizeof(signature)))return false;
    auto* relay=static_cast<unsigned char*>(VirtualAlloc(nullptr,11,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));
    if(!relay)return false;
    std::memcpy(relay,entry,6);relativeJump(relay+6,reinterpret_cast<unsigned>(entry+6));
    DWORD protection,ignored;
    if(!VirtualProtect(relay,11,PAGE_EXECUTE_READ,&ignored)||!VirtualProtect(entry,6,PAGE_EXECUTE_READWRITE,&protection))return false;
    originalCpuObstacle=reinterpret_cast<CpuObstacleCall>(relay);
    relativeJump(entry,reinterpret_cast<unsigned>(observeCpuObstacle));entry[5]=0x90;
    FlushInstructionCache(GetCurrentProcess(),entry,6);FlushInstructionCache(GetCurrentProcess(),relay,11);
    VirtualProtect(entry,6,protection,&ignored);return true;
}
CpuObstacleCall originalCpuControl;
unsigned __fastcall observeCpuControl(unsigned self,unsigned unused){
    const unsigned result=originalCpuControl(self,unused);
    const auto game=read<unsigned>(gameBase+0x1d1a54);
    const auto frame=game?read<unsigned>(game+0x14):0;
    if(detailedFrame(frame,0,0))if(FILE* output=std::fopen("native-cpu-control.jsonl","ab")){
        const auto count=read<unsigned>(self+0x36ee84);
        const auto player=read<unsigned>(read<unsigned>(self+0x36eec8)+4);
        std::fprintf(output,"{\"frame\":%u,\"cpu\":%u,\"bits\":%u,\"player\":[%u,%u,%u,%u],\"position\":[%u,%u],\"radius\":%u,\"steps\":[%u,%u,%u,%u],\"state\":[",frame,self,result,
            read<unsigned>(player+0x10),read<unsigned>(player+0x14),read<unsigned>(player+0x6b8),read<unsigned>(player+0x2078),
            read<unsigned>(self+0x36ee88),read<unsigned>(self+0x36ee8c),read<unsigned>(player+0x20c8),
            read<unsigned>(player+0x20e4),read<unsigned>(player+0x20e8),read<unsigned>(player+0x20ec),read<unsigned>(player+0x20f0));
        for(unsigned offset=0x36ee94;offset<0x36eec8;offset+=4)std::fprintf(output,"%s%u",offset==0x36ee94?"":",",read<unsigned>(self+offset));
        std::fputs("],\"objects\":[",output);
        for(unsigned i=0;i<count&&i<30000;++i){
            std::fputs(i?",[":"[",output);
            for(unsigned word=0;word<15;++word)std::fprintf(output,"%s%u",word?",":"",read<unsigned>(self+4+i*60+word*4));
            std::fputc(']',output);
        }
        std::fputs("],\"pickups\":[",output);
        const auto manager=read<unsigned>(read<unsigned>(self+0x36eec8)+0xc);
        auto node=manager?read<unsigned>(manager+0xff85c):0;
        for(unsigned index=0;node&&index<4096;++index){
            const auto item=read<unsigned>(node);
            std::fprintf(output,"%s[%u,%u,%.9g,%.9g]",index?",":"",item,read<unsigned>(item+0xcc8),read<float>(item+0xc80),read<float>(item+0xc84));
            node=read<unsigned>(node+4);
        }
        std::fputs("]}\n",output);std::fclose(output);
    }
    return result;
}
bool installCpuControlObserver(){
    auto* entry=reinterpret_cast<unsigned char*>(gameBase+0xfa3e0);
    const unsigned char signature[]={0x55,0x8b,0xec,0x83,0xe4,0xc0,0x83,0xec,0x74};
    if(std::memcmp(entry,signature,sizeof(signature)))return false;
    auto* relay=static_cast<unsigned char*>(VirtualAlloc(nullptr,11,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));
    if(!relay)return false;
    std::memcpy(relay,entry,6);relativeJump(relay+6,reinterpret_cast<unsigned>(entry+6));
    DWORD protection,ignored;
    if(!VirtualProtect(relay,11,PAGE_EXECUTE_READ,&ignored)||!VirtualProtect(entry,6,PAGE_EXECUTE_READWRITE,&protection))return false;
    originalCpuControl=reinterpret_cast<CpuObstacleCall>(relay);
    relativeJump(entry,reinterpret_cast<unsigned>(observeCpuControl));entry[5]=0x90;
    FlushInstructionCache(GetCurrentProcess(),entry,6);FlushInstructionCache(GetCurrentProcess(),relay,11);
    VirtualProtect(entry,6,protection,&ignored);return true;
}
HRESULT STDMETHODCALLTYPE observeDevice(IDirect3D9* self, UINT adapter, D3DDEVTYPE type, HWND window,
                                         DWORD behavior, D3DPRESENT_PARAMETERS* parameters,
                                         IDirect3DDevice9** device) {
    HRESULT result = originalCreateDevice(self, adapter, type, window, behavior, parameters, device);
    if (trace && parameters) {
        std::fprintf(trace, "{\"deviceCreated\":%s,\"result\":%ld,\"width\":%u,\"height\":%u,\"windowed\":%s,\"arguments\":[%u,%u,%lu,%u,%u,%u,%lu,%d,%u,%lu,%u,%d,%d]}\n",
            SUCCEEDED(result) ? "true" : "false", long(result),
            parameters->BackBufferWidth, parameters->BackBufferHeight,
            parameters->Windowed ? "true" : "false", adapter, unsigned(type), behavior,
            unsigned(parameters->BackBufferFormat), unsigned(parameters->SwapEffect),
            unsigned(parameters->MultiSampleType), parameters->MultiSampleQuality,
            parameters->EnableAutoDepthStencil, unsigned(parameters->AutoDepthStencilFormat),
            parameters->Flags, parameters->PresentationInterval, IsWindow(window), IsWindow(parameters->hDeviceWindow));
        std::fflush(trace);
    }
    if (SUCCEEDED(result) && device && *device && !originalPresent)
        patch(*reinterpret_cast<void***>(*device) + 17, observePresent, originalPresent);
    if(SUCCEEDED(result)&&device&&*device&&!originalDrawPrimitive&&
       (GetEnvironmentVariableA("TH19_CAPTURE_STATE_ONLY",nullptr,0)>0||GetEnvironmentVariableA("TH19_CAPTURE_RENDER_FROM",nullptr,0)>0)){
        auto** table=*reinterpret_cast<void***>(*device);
        const bool active=patch(table+81,observeDrawPrimitive,originalDrawPrimitive)&&
            patch(table+82,observeDrawIndexedPrimitive,originalDrawIndexedPrimitive)&&
            patch(table+83,observeDrawPrimitiveUP,originalDrawPrimitiveUP)&&
            patch(table+84,observeDrawIndexedPrimitiveUP,originalDrawIndexedPrimitiveUP);
        if(trace){std::fprintf(trace,"{\"stateOnlyDrawInstalled\":%s}\n",active?"true":"false");std::fflush(trace);}
    }
    return result;
}
}

#if defined(__MINGW32__)
#pragma comment(linker, "/export:Direct3DCreate9=observeCreate9@4")
#else
#pragma comment(linker, "/export:Direct3DCreate9=_observeCreate9@4")
#endif
extern "C" IDirect3D9* WINAPI observeCreate9(UINT sdkVersion) {
    wchar_t path[MAX_PATH]{};
    const UINT size = GetSystemDirectoryW(path, MAX_PATH);
    if (size == 0 || size + 10 >= MAX_PATH) return nullptr;
    wcscat_s(path, L"\\d3d9.dll");
    const auto real = LoadLibraryExW(path, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    const auto create = real ? reinterpret_cast<Create9>(GetProcAddress(real, "Direct3DCreate9")) : nullptr;
    if (!create || create == observeCreate9) return nullptr;
    IDirect3D9* d3d = GetEnvironmentVariableA("TH19_CAPTURE_SOFTWARE_D3D9",nullptr,0)>0
        ? createWarp9(real,sdkVersion) : create(sdkVersion);
    if (!d3d || originalCreateDevice) return d3d;
    gameBase = reinterpret_cast<std::uintptr_t>(GetModuleHandleW(nullptr));
    cpuDebugActive=GetEnvironmentVariableA("TH19_CAPTURE_CPU_DEBUG",nullptr,0)>0;
    if(cpuDebugActive)AddVectoredExceptionHandler(1,recordCaptureException);
    const unsigned char timer[] = {0x8b,0x41,0x0c,0x83,0xf8,0x01,0x72,0x09};
    if (std::memcmp(reinterpret_cast<const void*>(gameBase+0x26c0),timer,sizeof(timer))!=0 ||
        read<std::uint32_t>(gameBase+0x116cf3) != gameBase+0x1d1a54)
        return d3d;
    trace = _fsopen("native-present.jsonl", "wb", _SH_DENYWR);
    if (trace &&
        patch(*reinterpret_cast<void***>(d3d) + 16, observeDevice, originalCreateDevice)) {
        std::fputs("{\"observer\":\"th19-d3d9-pre-present\",\"stateCoverage\":[\"clock\",\"flags\",\"rng\"]}\n", trace);
        if(GetEnvironmentVariableA("TH19_CAPTURE_SKIP_INPUT_RESET",nullptr,0)>0&&
           GetEnvironmentVariableA("TH19_CAPTURE_CPU",nullptr,0)>0){
            // 0x4b8b80 resets devices through 0x4b8a40. On Server/RDP,
            // the keyboard Unacquire at 0x4b8adf can block indefinitely.
            // The CPU driver supplies both seats' input. Retain the devices
            // initialized before this observation provider was loaded.
            const bool active=installCpuInputResetCompatibility();
            std::fprintf(trace,"{\"inputResetSkipped\":%s}\n",active?"true":"false");
        }
        if(GetEnvironmentVariableA("TH19_CAPTURE_SILENT_AUDIO",nullptr,0)>0){
            // 0x4babe0 only enqueues sound requests (three stack arguments).
            // Without a DirectSound device, 0x4baeda skips draining the queue,
            // while the stage loader waits forever at 0x518be4. In explicit
            // silent captures, omit requests rather than bypassing stage load.
            auto* entry=reinterpret_cast<unsigned char*>(gameBase+0xbabe0);
            const unsigned char prolog[]={0x55,0x8b,0xec,0x56,0x68};
            DWORD protection=0,ignored=0;
            bool active=std::memcmp(entry,prolog,sizeof(prolog))==0&&
                read<unsigned>(gameBase+0xbabe5)==gameBase+0x22af28&&
                VirtualProtect(entry,3,PAGE_EXECUTE_READWRITE,&protection);
            if(active){
                entry[0]=0xc2;entry[1]=0x0c;entry[2]=0;
                FlushInstructionCache(GetCurrentProcess(),entry,3);
                VirtualProtect(entry,3,protection,&ignored);
            }
            std::fprintf(trace,"{\"silentAudioActive\":%s}\n",active?"true":"false");
            // 0x54cae2 / 0x54d6e2 pan Marisa's looping beam buffer directly. With no
            // DirectSound device, 0x6270a4 remains null; the queued-SFX shim
            // above is not involved. The next block at 0x54d6fa updates length.
            const unsigned char panTail[]={0x8b,0x08,0xf3,0x0f,0x2c,0xd0,0x52,0x50,0xff,0x51,0x40};
            unsigned panSkipped=0;
            for(unsigned rva:{0x14cae2u,0x14d6e2u}){
                auto* pan=reinterpret_cast<unsigned char*>(gameBase+rva);
                if(active&&pan[0]==0xa1&&
                   read<unsigned>(reinterpret_cast<unsigned>(pan+1))==gameBase+0x2270a4&&
                   std::memcmp(pan+13,panTail,sizeof(panTail))==0&&
                   VirtualProtect(pan,5,PAGE_EXECUTE_READWRITE,&protection)){
                    relativeJump(pan,gameBase+rva+24);
                    FlushInstructionCache(GetCurrentProcess(),pan,5);
                    VirtualProtect(pan,5,protection,&ignored);
                    ++panSkipped;
                }
            }
            std::fprintf(trace,"{\"silentAudioPanSkipped\":%u}\n",panSkipped);
        }
        patch(*reinterpret_cast<void***>(d3d) + 8, observeDisplayMode, originalDisplayMode);
        if (GetFileAttributesW(L".\\th19replayplayer_hook.dll") != INVALID_FILE_ATTRIBUTES) {
            const auto player = LoadLibraryW(L".\\th19replayplayer_hook.dll");
            std::fprintf(trace, "{\"cpuDriverLoaded\":%s,\"loadError\":%lu}\n",
                player ? "true" : "false", player ? 0ul : GetLastError());
        }
        wchar_t rngEnabled[2]{};
        if (GetEnvironmentVariableW(L"TH19_CAPTURE_RNG_CALLS", rngEnabled, _countof(rngEnabled)) == 1 &&
            rngEnabled[0] == L'1')
            std::fprintf(trace, "{\"rngObserverInstalled\":%s,\"base\":%u}\n",
                installRngObserver() ? "true" : "false", unsigned(gameBase));
        if (rngTrace) std::fprintf(trace,"{\"rectObserverInstalled\":%s}\n",installRectObserver()?"true":"false");
        if (rngTrace) std::fprintf(trace,"{\"impactObserverInstalled\":%s}\n",installImpactObserver()?"true":"false");
        if (rngTrace) std::fprintf(trace,"{\"shotObserverInstalled\":%s}\n",installShotObserver()?"true":"false");
        if (rngTrace&&cpuDebugActive) std::fprintf(trace,"{\"cpuObstacleObserverInstalled\":%s,\"cpuControlObserverInstalled\":%s}\n",installCpuObstacleObserver()?"true":"false",installCpuControlObserver()?"true":"false");
        if (rngTrace&&cpuDebugActive) std::fprintf(trace,"{\"fixedCutObserverInstalled\":%s}\n",installFixedCutObserver()?"true":"false");
        if (rngTrace&&cpuDebugActive) std::fprintf(trace,"{\"fixedCircleObserverInstalled\":%s}\n",installFixedCircleObserver()?"true":"false");
        if (rngTrace&&cpuDebugActive) std::fprintf(trace,"{\"fixedUpdateObserverInstalled\":%s}\n",installFixedUpdateObserver()?"true":"false");
        if (rngTrace&&cpuDebugActive) std::fprintf(trace,"{\"bulletFireObserverInstalled\":%s}\n",installBulletFireObserver()?"true":"false");
        if (rngTrace&&cpuDebugActive) std::fprintf(trace,"{\"fieldRayObserverInstalled\":%s}\n",installFieldRayObserver()?"true":"false");
        if (rngTrace&&cpuDebugActive) std::fprintf(trace,"{\"anmChildObserverInstalled\":%s}\n",installAnmChildObserver()?"true":"false");
        if (rngTrace&&cpuDebugActive) std::fprintf(trace,"{\"anmDetachedObserverInstalled\":%s}\n",installAnmDetachedObserver()?"true":"false");
        if(rngTrace&&cpuDebugActive)std::fprintf(trace,"{\"straightGrazeObserverInstalled\":%s}\n",installStraightGrazeObserver()?"true":"false");
        captureBulletPositions=GetEnvironmentVariableA("TH19_CAPTURE_BULLET_POSITIONS",nullptr,0)>0;
        if(const char* frames=std::getenv("TH19_CAPTURE_IMAGE_FRAMES")){
            while(*frames){
                char* end=nullptr;
                const unsigned long value=std::strtoul(frames,&end,10);
                if(end==frames||value>216000||(*end&&*end!=','))break;
                extraImageFrames.push_back(unsigned(value));
                frames=*end?end+1:end;
            }
        }
        std::fflush(trace);
    }
    return d3d;
}
