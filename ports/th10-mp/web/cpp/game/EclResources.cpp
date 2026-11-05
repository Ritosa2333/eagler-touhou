#include "EclResources.hpp"
namespace th10 {
namespace {template<class T>T read(const u8* at){T value;__builtin_memcpy(&value,at,sizeof(value));return value;}}
// Constructor in 0x40d280 and table teardown in 0x40d680.
void EclProgram::initialize(void* type_table) noexcept {__builtin_memset(this,0,sizeof(*this));original_virtual_table=type_table;}
void EclProgram::release_lookup(EclResourceEnvironment& env,void* type_table){original_virtual_table=type_table;if(sorted_subroutines){env.release(sorted_subroutines);sorted_subroutines=nullptr;}}
void EclProgram::release_files(EclResourceEnvironment& env){for(auto* file:loaded_files)if(file)env.release(file);}
// 0x450220. The first file already supplies sorted names. Later files merge
// into that table, placing a new definition before existing equal names.
i32 EclProgram::attach_file(u8* data,EclResourceEnvironment& env){
    if(file_count<0||file_count>=32||!data)__builtin_trap();loaded_files[file_count]=data;
    if(read<u32>(data)!=0x54504353||read<u16>(data+4)!=1){loaded_files[file_count]=nullptr;return -1;}
    const auto count=read<u16>(data+0x10);const auto* offsets=data+0x24+read<u16>(data+6);auto* names=reinterpret_cast<const char*>(offsets+count*4);auto* previous=sorted_subroutines;
    subroutine_count=wrapping_add(subroutine_count,count);sorted_subroutines=static_cast<EclSubroutine*>(env.allocate(static_cast<u32>(subroutine_count)*sizeof(EclSubroutine)));
    if(!previous){
        for(i32 index=0;index<subroutine_count;++index){sorted_subroutines[index]={names,loaded_files[file_count]+read<u32>(offsets)};names+=std::strlen(names)+1;offsets+=4;}
    }else{
        i32 populated=wrapping_add(subroutine_count,-static_cast<i32>(count));__builtin_memcpy(sorted_subroutines,previous,static_cast<u32>(populated)*sizeof(EclSubroutine));env.release(previous);
        for(u32 item=0;item<count;++item){
            i32 index=0;while(index<populated&&std::strcmp(names,sorted_subroutines[index].name)>0)++index;
            for(i32 move=subroutine_count-1;move>index;--move)sorted_subroutines[move]=sorted_subroutines[move-1];
            sorted_subroutines[index]={names,loaded_files[file_count]+read<u32>(offsets)};names+=std::strlen(names)+1;offsets+=4;++populated;
        }
    }
    const auto index=file_count;file_count=wrapping_add(file_count,1);const auto* file=loaded_files[index];if(read<u16>(file+6))env.process_header(*this,file+0x24);return index;
}
// 0x40cd20. A header callback's error is intentionally not the file result.
i32 EclProgram::load(const char* name,EclResourceEnvironment& env){return attach_file(env.read_file(name),env)<0?-1:0;}
// 0x40d400. Animation includes precede recursively loaded ECL files. Included
// names are zero-terminated byte strings; only the second header is aligned.
i32 EnemyScriptResourceEnvironment::process_header(EclProgram& program,const u8* data){
    if(read<u32>(data)!=0x4d494e41)return 0;auto* names=reinterpret_cast<const char*>(data+8);
    for(u32 index=0;index<read<u32>(data+4);++index){
        auto* file=load_animation(index+9,names);enemy_animations[index+1]=file;if(!file){missing_animation();return -1;}names+=std::strlen(names)+1;
    }
    auto relative=static_cast<u32>(reinterpret_cast<const u8*>(names)-data);relative=(relative+3)&~3u;const auto* included=data+relative;
    if(read<u32>(included)!=0x494c4345)return 0;names=reinterpret_cast<const char*>(included+8);
    for(u32 index=0;index<read<u32>(included+4);++index){program.load(names,*this);names+=std::strlen(names)+1;}return 0;
}
}
