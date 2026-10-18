#include "Screenshot.hpp"
namespace th10 {
i32 Screenshot::capture(ApplicationState& app,ScreenshotState& state,const u32& format,const char* filename){
    auto& env=environment;while(state.worker)env.sleep(10);
    auto* surface=env.back_buffer(app.device);state.header={0x4d42,54,0,0,54};
    char* target=state.filename;do{*target++=*filename;}while(*filename++);
    if(format!=22&&format!=23){env.report(ScreenshotError::UnsupportedFormat);return 1;}
    if(format==23)env.report(ScreenshotError::Format16);
    else{
        state.bitmap_info=static_cast<u32*>(env.allocate(44));
        if(state.bitmap_info){
            std::memset(state.bitmap_info,0,44);state.pixels=static_cast<u8*>(env.allocate(640*480*3));
            if(state.pixels){
                state.header.size+=640*480*3;state.bitmap_info[3]=(state.bitmap_info[3]&0xffffu)|0x180000u;state.bitmap_info[0]=40;state.bitmap_info[1]=640;state.bitmap_info[2]=480;state.bitmap_info[3]=(state.bitmap_info[3]&0xffff0000u)|1;state.bitmap_info[4]=0;
                const auto lock=env.lock_surface(surface);
                for(i32 row=479;row>=0;--row){const auto* input=lock.pixels+static_cast<i32>(static_cast<u32>(lock.pitch)*static_cast<u32>(row));auto* output=state.pixels+(479-row)*1920;for(u32 column=0;column<640;++column){u16 low;std::memcpy(&low,input,2);std::memcpy(output,&low,2);output[2]=input[2];input+=4;output+=3;}}
                env.unlock_surface(surface);env.global->worker=env.begin_writer();
            }else env.report(ScreenshotError::Allocation);
        }else env.report(ScreenshotError::Allocation);
    }
    if(surface)env.release_surface(surface);return 0;
}
void Screenshot::write(u32 actual){
    auto& env=environment;auto& state=*env.global;env.open_output(state.filename);
    const auto write_chunk=[&](const u8* bytes,u32 size){if(*env.output_handle!=0xffffffffu){env.write_output(bytes,size,actual);if(actual!=size)env.close_output();}};
    write_chunk(reinterpret_cast<const u8*>(&state.header),14);write_chunk(reinterpret_cast<const u8*>(state.bitmap_info),40);write_chunk(state.pixels,640*480*3);
    if(*env.output_handle!=0xffffffffu)env.close_output();
    if(state.bitmap_info){env.free_bytes(state.bitmap_info);state.bitmap_info=nullptr;}if(state.pixels){env.free_bytes(state.pixels);state.pixels=nullptr;}state.worker=0;
}
}
