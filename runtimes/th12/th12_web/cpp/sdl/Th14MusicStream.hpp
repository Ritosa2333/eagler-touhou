#pragma once

namespace th11::sdl {
// Reconstruct the retail raw PCM file address space from its lossless tracks.
// Ordinary reads still use the existing decoder and its original loop points.
struct Th14MusicStream {
    ma_data_source_base base{};
    ma_decoder* primary;
    const std::vector<u8>& formats;
    const u8* current;
    ma_decoder preceding{};
    bool initialized=false,preceding_ready=false,exceptional=false;
    i32 selected=-1;
    u32 physical=0,remaining=0;

    static u32 word(const u8* p){u32 v;std::memcpy(&v,p,4);return v;}
    static u16 half(const u8* p){u16 v;std::memcpy(&v,p,2);return v;}
    u32 align()const{return half(current+44);}
    void close_preceding(){if(preceding_ready)ma_decoder_uninit(&preceding);preceding_ready=false;selected=-1;}
    u32 subframe_relative()const{
        u32 value=0u-align();if(value>=word(current+28))value+=word(current+24)-word(current+28);return value;
    }
    u32 subframe_address()const{
        const u32 address=word(current+16)+subframe_relative();
        if(address<=0x7fffffffu)return address;
        // Continue44aef0 queues the saved filename and waits for46e5e0.
        // Its481a60 reset clears reader9c;480cf0 fills the new4s buffer.
        // The final signed SetFilePointer fails, retaining that fresh cursor.
        const u32 bytes=word(current+36)*align()*4,total=word(current+28),intro=word(current+24);
        const u32 cursor=bytes<=total?bytes:intro+(bytes-total-1)%(total-intro)+1;
        return word(current+16)+cursor;
    }
    bool restore_supported()const{
        const u32 address=subframe_address();
        for(size_t n=0;n+52<=formats.size();n+=52){const auto* p=formats.data()+n;if(address>=word(p+16)&&u64(address)<u64(word(p+16))+word(p+28))return true;}
        return false;
    }
    void begin_subframe(){
        close_preceding();exceptional=true;physical=subframe_address();remaining=word(current+28)-subframe_relative();
    }
    ma_result resume_loop(){
        close_preceding();exceptional=false;
        return ma_data_source_seek_to_pcm_frame(primary,word(current+24)/align());
    }
    ma_result select(ma_decoder*& decoder,u32& available){
        for(size_t n=0;n+52<=formats.size();n+=52){const auto* p=formats.data()+n;
            const u32 offset=word(p+16),total=word(p+28);
            if(physical<offset||u64(physical)>=u64(offset)+total)continue;
            if(half(p+44)!=align()||half(p+34)!=half(current+34)||word(p+36)!=word(current+36))return MA_INVALID_DATA;
            if(selected!=i32(n/52)){
                close_preceding();
                if(p==current)decoder=primary;
                else{
                    const auto end=std::find(p,p+16,u8(0));std::string name(reinterpret_cast<const char*>(p),size_t(end-p));
                    const auto dot=name.find_last_of('.');if(dot==std::string::npos)return MA_INVALID_DATA;
                    name.resize(dot);const auto path=std::string("/music/")+name+".flac";
                    auto config=ma_decoder_config_init(ma_format_f32,half(p+34),word(p+36));
                    const auto result=ma_decoder_init_file(path.c_str(),&config,&preceding);if(result!=MA_SUCCESS)return result;
                    preceding_ready=true;decoder=&preceding;
                }
                const auto result=ma_data_source_seek_to_pcm_frame(decoder,(physical-offset)/align());if(result!=MA_SUCCESS)return result;
                selected=i32(n/52);
            }else decoder=p==current?primary:&preceding;
            available=(total-(physical-offset))/align();return MA_SUCCESS;
        }
        return MA_INVALID_DATA;
    }
    static ma_result read(ma_data_source* source,void* output,ma_uint64 count,ma_uint64* done){
        auto& s=*reinterpret_cast<Th14MusicStream*>(source);*done=0;
        while(*done<count){
            auto* out=static_cast<float*>(output)+*done*half(s.current+34);
            if(!s.exceptional){ma_uint64 got=0;const auto result=ma_data_source_read_pcm_frames(s.primary,out,count-*done,&got);*done+=got;return result;}
            if(!s.remaining){const auto result=s.resume_loop();if(result!=MA_SUCCESS)return result;continue;}
            ma_decoder* decoder=nullptr;u32 available=0;const auto result=s.select(decoder,available);if(result!=MA_SUCCESS)return result;
            const auto take=std::min({count-*done,ma_uint64(available),ma_uint64(s.remaining/s.align())});if(!take)return MA_INVALID_DATA;
            ma_uint64 got=0;const auto decoded=ma_data_source_read_pcm_frames(decoder,out,take,&got);
            s.physical+=u32(got)*s.align();s.remaining-=u32(got)*s.align();*done+=got;
            if(got!=take)return decoded==MA_SUCCESS?MA_INVALID_DATA:decoded;
        }
        return MA_SUCCESS;
    }
    static ma_result seek(ma_data_source* source,ma_uint64 frame){
        auto& s=*reinterpret_cast<Th14MusicStream*>(source);
        const auto result=ma_data_source_seek_to_pcm_frame(s.primary,frame);
        if(result==MA_SUCCESS){s.exceptional=false;s.close_preceding();}return result;
    }
    static ma_result format(ma_data_source* source,ma_format* f,ma_uint32* c,ma_uint32* r,ma_channel* map,size_t capacity){return ma_data_source_get_data_format(reinterpret_cast<Th14MusicStream*>(source)->primary,f,c,r,map,capacity);}
    static ma_result cursor(ma_data_source* source,ma_uint64* frame){auto& s=*reinterpret_cast<Th14MusicStream*>(source);if(!s.exceptional)return ma_data_source_get_cursor_in_pcm_frames(s.primary,frame);*frame=u32(s.physical-word(s.current+16))/s.align();return MA_SUCCESS;}
    static ma_result length(ma_data_source* source,ma_uint64* frames){return ma_data_source_get_length_in_pcm_frames(reinterpret_cast<Th14MusicStream*>(source)->primary,frames);}
    static ma_result looping(ma_data_source* source,ma_bool32 enabled){return ma_data_source_set_looping(reinterpret_cast<Th14MusicStream*>(source)->primary,enabled);}
    Th14MusicStream(ma_decoder& decoder,const std::vector<u8>& layouts,const u8* entry):primary(&decoder),formats(layouts),current(entry){
        static const ma_data_source_vtable vtable{read,seek,format,cursor,length,looping,MA_DATA_SOURCE_SELF_MANAGED_RANGE_AND_LOOP_POINT};
        auto config=ma_data_source_config_init();config.vtable=&vtable;initialized=ma_data_source_init(&config,&base)==MA_SUCCESS;
    }
    ~Th14MusicStream(){close_preceding();if(initialized)ma_data_source_uninit(&base);}
};
}
