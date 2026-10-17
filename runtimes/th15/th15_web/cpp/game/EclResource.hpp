#pragma once
#include "Types.hpp"
#include <memory>
#include <string>
#include <vector>
namespace th15 {
struct EclInstruction {
    i32 time;u16 opcode,length,references;u8 difficulty,parameter_count,stack_adjustment,reserved[3];
    template<class T=u32>T argument(u32 index=0)const noexcept{T v;std::memcpy(&v,reinterpret_cast<const u8*>(this)+16+index*4,sizeof v);return v;}
};
static_assert(sizeof(EclInstruction)==16);
struct EclSubroutine {std::string name;u32 offset=0,size=0;};
class EclResource {
public:
    std::vector<u8> bytes;
    std::vector<EclSubroutine> subroutines;
    std::vector<std::string> animations,includes;
    bool open(const u8*,u32);
};
class EclResourceProvider {
public:
    virtual ~EclResourceProvider()=default;
    virtual bool read(const std::string& name,std::vector<u8>& data)=0;
    virtual bool animation(u32 slot,const std::string& name)=0;
};
class EclProgram {
public:
    struct Definition {const EclResource* file;u32 index;};
    std::vector<std::unique_ptr<EclResource>> files;
    std::vector<Definition> definitions;
    std::string error;
    // First-file order and subsequent insertion before equal names match
    // TH15 0x48f130. Lookup deliberately uses the original binary-search midpoint.
    i32 attach(const u8*,u32,EclResourceProvider* provider=nullptr);
    bool load(const std::string&,EclResourceProvider&);
    const EclInstruction* find(const char*)const noexcept;
    i32 find_index(const char*)const noexcept;
    const EclSubroutine& subroutine(u32 index)const noexcept{return definitions[index].file->subroutines[definitions[index].index];}
};
}
