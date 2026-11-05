#include "audio.hpp"
#include <algorithm>
#include <cstring>
#include <stdexcept>

namespace th20::source::audio {
DeviceOwner::~DeviceOwner() {
#ifdef TH_SDL3
    device=nullptr; // the software device is host-owned
#else
    if(device) {device->Release();device=nullptr;}
#endif
}
HRESULT DeviceOwner::initialize(HWND window,DWORD level,WORD channels,DWORD rate,WORD bits) {
#ifdef TH_SDL3
    device=&web::audio::device();
    if(!device->initialize(channels,rate,bits)) return E_FAIL;
    (void)window;(void)level; // the browser runtime is the sole audio owner
    set_primary_format(channels,rate,bits);return S_OK;
#else
    if(device) {device->Release();device=nullptr;}
    auto result=DirectSoundCreate8(nullptr,&device,nullptr);
    if(FAILED(result)) return result;
    result=device->SetCooperativeLevel(window,level);
    if(FAILED(result)) return result;
    set_primary_format(channels,rate,bits);return S_OK;
#endif
}
HRESULT DeviceOwner::set_primary_format(WORD channels,DWORD rate,WORD bits) {
    if(!device) return CO_E_NOTINITIALIZED;
    DSBUFFERDESC description{};description.dwSize=sizeof description;description.dwFlags=DSBCAPS_PRIMARYBUFFER;
    SoundBuffer* buffer=nullptr;
    auto result=device->CreateSoundBuffer(&description,&buffer,nullptr);
    if(FAILED(result) || !buffer) return result;
    WAVEFORMATEX format{};format.wFormatTag=WAVE_FORMAT_PCM;format.nChannels=channels;
    format.nSamplesPerSec=rate;format.wBitsPerSample=bits;
    format.nBlockAlign=static_cast<WORD>((bits>>3)*channels);
    format.nAvgBytesPerSec=format.nBlockAlign*rate;
    result=buffer->SetFormat(&format);
    // The original only releases on success; failure cleanup here prevents
    // leaking an OS buffer without claiming that allocation leak equivalent.
    buffer->Release();return result;
}
namespace {
struct Chunk {const std::uint8_t* data;std::uint32_t size;};
std::uint32_t word(const std::uint8_t* p) {std::uint32_t v;std::memcpy(&v,p,4);return v;}
std::optional<Chunk> find_chunk(const std::vector<std::uint8_t>& bytes,const char* name,std::uint32_t budget) {
    std::size_t offset=12;
    while(budget) {
        if(offset>bytes.size() || bytes.size()-offset<8) return std::nullopt;
        const auto length=word(bytes.data()+offset+4);
        if(length>bytes.size()-offset-8) return std::nullopt;
        if(std::memcmp(bytes.data()+offset,name,4)==0) return Chunk{bytes.data()+offset+8,length};
        // 0x4283b0 advances by size+8 without RIFF odd-byte padding.
        if(length>budget || budget-length<8) return std::nullopt;
        budget-=length+8;offset+=length+8;
    }
    return std::nullopt;
}
}
int SoundInf::load_wave(unsigned index,const char* name) {
    if(!context || !context->read_resource) throw std::logic_error("Audio requires a resource reader");
    if(index>=72) throw std::out_of_range("Source sound buffer index");
    const auto bytes=context->read_resource(name);
    // Resource absence returns -1 (0x42707f); post-load failures and success
    // return zero (0x42732e/0x42734e).
    if(!bytes) return -1;
    if(bytes->size()<12 || std::memcmp(bytes->data(),"RIFF",4) || std::memcmp(bytes->data()+8,"WAVE",4)) return 0;
    const auto riff_size=word(bytes->data()+4);
    const auto format_chunk=find_chunk(*bytes,"fmt ",riff_size-12);
    if(!format_chunk) return 0;
    if(static_cast<std::size_t>(format_chunk->data-bytes->data())+sizeof(WAVEFORMATEX)>bytes->size()) return 0;
    WAVEFORMATEX format;std::memcpy(&format,format_chunk->data,sizeof format);
    const auto data=find_chunk(*bytes,"data",riff_size-12);if(!data) return 0;
    DSBUFFERDESC description{};description.dwSize=sizeof description;description.dwFlags=0x80c8;
    description.dwBufferBytes=data->size;description.lpwfxFormat=&format;
    auto result=direct_sound->CreateSoundBuffer(&description,&source_buffers[index],nullptr);
    if(FAILED(result)) return 0;
    void* first=nullptr;void* second=nullptr;DWORD first_size=0,second_size=0;
    auto* buffer=source_buffers[index];
    result=buffer->Lock(0,data->size,&first,&first_size,&second,&second_size,0);
    if(FAILED(result)) return 0;
    std::memcpy(first,data->data,first_size);
    if(second_size) std::memcpy(second,data->data+first_size,second_size);
    buffer->Unlock(first,first_size,second,second_size);return 0;
}
int SoundInf::create_effect(EffectChannel& channel) {
    if(device_owner) {
        release_effect(channel);
        const auto index=channel.definition->file_index;
        if(index<0 || index>=72) throw std::out_of_range("Effect file index");
        if(duplicate_counts[index]==0) channel.buffer=source_buffers[index];
        else direct_sound->DuplicateSoundBuffer(source_buffers[index],&channel.buffer);
        ++duplicate_counts[index];
    }
    return 0;
}
}
