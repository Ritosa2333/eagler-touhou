#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d9.h>
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
using CreateDevice = HRESULT (STDMETHODCALLTYPE*)(IDirect3D9*, UINT, D3DDEVTYPE, HWND,
    DWORD, D3DPRESENT_PARAMETERS*, IDirect3DDevice9**);
using Present = HRESULT (STDMETHODCALLTYPE*)(IDirect3DDevice9*, const RECT*, const RECT*,
    HWND, const RGNDATA*);
CreateDevice originalCreateDevice;
Present originalPresent;
std::uintptr_t gameBase;
FILE* trace;
unsigned presentIndex, generation, priorFrame;
std::uintptr_t priorOwner;
constexpr unsigned imageFrames[] = {80, 83, 84, 85, 86, 87, 88, 90, 100, 105, 111, 112, 113, 114, 120, 124, 125, 126, 127, 130, 140, 180, 300, 600, 1200};
bool savedImage[sizeof(imageFrames) / sizeof(imageFrames[0])];
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
    unsigned animation[8];
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

void* originalShot;
struct ShotGate {
    unsigned timer, seat, warmup, emitClock, seatFlags, gameFlags, stageReady, frozen;
};
ShotGate shotGateSamples[512];
unsigned shotGateCount, shotGateFlushed;
struct PlayerSample {unsigned owner, timer, x, y, controls, bulletManager;};
PlayerSample playerSamples[2];
bool captureBulletPositions;
struct ProjectileSample {unsigned timer,seat,projectile,vm,age,x,y,angle,speed,kind;};
ProjectileSample projectileSamples[4096];
unsigned projectileCount,projectileFlushed;
struct EnemySample {unsigned timer,id,x,y,flags,status;int hp;};
EnemySample enemySamples[1024];
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
    if(shotGateCount>=_countof(shotGateSamples)||!detailedFrame(timer,0,130)) return;
    auto& sample=shotGateSamples[shotGateCount++];
    const auto stage=read<unsigned>(gameBase+0x1d1a68);
    const auto director=read<unsigned>(gameBase+0x1d1a50);
    sample={timer,seat,read<unsigned>(shot+0x1282c),
        read<unsigned>(shot+0x12804),status?read<unsigned>(status+0x14):0,
        read<unsigned>(game+0x140),stage?read<unsigned>(stage+0x4b78):0,
        director?read<unsigned>(director+0x310):0};
    if(detailedFrame(timer,84,130)&&sample.seat==0&&ctx){
        auto manager=read<unsigned>(ctx+8);
        auto node=manager?read<unsigned>(manager+0x4b50):0;
        for(unsigned i=0;node&&i<24&&enemyCount<_countof(enemySamples);++i){
            const auto enemy=read<unsigned>(node);
            if(!enemy)break;
            // 0x509e68 subtracts damage from the +0x1230 subobject at +0x5008.
            enemySamples[enemyCount++]={timer,read<unsigned>(enemy+0x6858),read<unsigned>(enemy+0x1278),
                read<unsigned>(enemy+0x127c),read<unsigned>(enemy+0x639c),read<unsigned>(enemy+0x63a0),read<int>(enemy+0x6238)};
            node=read<unsigned>(node+4);
        }
    }
    if(detailedFrame(timer,86,130)&&sample.seat==0) {
        auto node=read<unsigned>(shot+0x12840);
        for(unsigned i=0;node&&i<96&&projectileCount<_countof(projectileSamples);++i) {
            const auto projectile=read<unsigned>(node);
            if(!projectile)break;
            projectileSamples[projectileCount++]={timer,sample.seat,projectile,read<unsigned>(projectile+0x14),
                read<unsigned>(projectile+0x1c),read<unsigned>(projectile+0x54),read<unsigned>(projectile+0x58),
                read<unsigned>(projectile+0x70),read<unsigned>(projectile+0x6c),read<unsigned>(projectile+0x9c)};
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
void __cdecl recordImpact(unsigned shot) {
    if(GetCurrentThreadId()!=captureThread || impactCount>=_countof(impactSamples))return;
    const auto game=read<unsigned>(gameBase+0x1d1a54);
    if(!game)return;
    const auto frame=read<unsigned>(game+0x14);
    if(!detailedFrame(frame,100,130))return;
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
        push ecx
        call recordImpact
        add esp, 4
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
    std::fputc('[', output);
    bool first = true;
    for (unsigned i=0; bullet && i<2000; ++i, bullet=read<unsigned>(bullet+4)) {
        const auto state = read<unsigned>(bullet+0x1044);
        if (state!=1 && state!=2) continue; // 0x4ee3e9 selects live bullets.
        std::fprintf(output,"%s[%u,%u,%u,%u]",first?"":",",
            unsigned(read<unsigned short>(bullet+0x1078)),unsigned(read<unsigned short>(bullet+0x107a)),
            read<unsigned>(bullet+0x650),read<unsigned>(bullet+0x654));
        first = false;
    }
    std::fputc(']', output);
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
            std::fprintf(trace,"%s[%u,%u,%u,%u,%u,%u,%d]",first?"":",",g.timer,g.id,g.x,g.y,g.flags,g.status,g.hp);
            first=false;
        }
        std::fputc(']',trace);
    }
    if(rngTrace&&projectileFlushed<projectileCount){
        std::fputs(",\"projectilesBeforeUpdate\":[",trace);
        bool first=true;
        for(;projectileFlushed<projectileCount;++projectileFlushed){
            const auto& g=projectileSamples[projectileFlushed];
            std::fprintf(trace,"%s[%u,%u,%u,%u,%u,%u,%u,%u,%u,%u]",first?"":",",
                g.timer,g.seat,g.projectile,g.vm,g.age,g.x,g.y,g.angle,g.speed,g.kind);
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

HRESULT STDMETHODCALLTYPE observePresent(IDirect3DDevice9* self, const RECT* source, const RECT* dest,
                                          HWND window, const RGNDATA* dirty) {
    capture(self);
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
            if(active&&read<unsigned>(owner+0x14)%60!=0)return D3D_OK;
        }
    }
    return originalPresent(self, source, dest, window, dirty);
}

HRESULT STDMETHODCALLTYPE observeDevice(IDirect3D9* self, UINT adapter, D3DDEVTYPE type, HWND window,
                                         DWORD behavior, D3DPRESENT_PARAMETERS* parameters,
                                         IDirect3DDevice9** device) {
    HRESULT result = originalCreateDevice(self, adapter, type, window, behavior, parameters, device);
    if (SUCCEEDED(result) && device && *device && !originalPresent)
        patch(*reinterpret_cast<void***>(*device) + 17, observePresent, originalPresent);
    return result;
}
}

#pragma comment(linker, "/export:Direct3DCreate9=_observeCreate9@4")
extern "C" IDirect3D9* WINAPI observeCreate9(UINT sdkVersion) {
    wchar_t path[MAX_PATH]{};
    const UINT size = GetSystemDirectoryW(path, MAX_PATH);
    if (size == 0 || size + 10 >= MAX_PATH) return nullptr;
    wcscat_s(path, L"\\d3d9.dll");
    const auto real = LoadLibraryExW(path, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    const auto create = real ? reinterpret_cast<Create9>(GetProcAddress(real, "Direct3DCreate9")) : nullptr;
    if (!create || create == observeCreate9) return nullptr;
    IDirect3D9* d3d = create(sdkVersion);
    if (!d3d || originalCreateDevice) return d3d;
    gameBase = reinterpret_cast<std::uintptr_t>(GetModuleHandleW(nullptr));
    const unsigned char timer[] = {0x8b,0x41,0x0c,0x83,0xf8,0x01,0x72,0x09};
    if (std::memcmp(reinterpret_cast<const void*>(gameBase+0x26c0),timer,sizeof(timer))!=0 ||
        read<std::uint32_t>(gameBase+0x116cf3) != gameBase+0x1d1a54)
        return d3d;
    trace = _fsopen("native-present.jsonl", "wb", _SH_DENYWR);
    if (trace &&
        patch(*reinterpret_cast<void***>(d3d) + 16, observeDevice, originalCreateDevice)) {
        std::fputs("{\"observer\":\"th19-d3d9-pre-present\",\"stateCoverage\":[\"clock\",\"flags\",\"rng\"]}\n", trace);
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
        captureBulletPositions=GetEnvironmentVariableA("TH19_CAPTURE_BULLET_POSITIONS",nullptr,0)>0;
        std::fflush(trace);
    }
    return d3d;
}
