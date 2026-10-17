#pragma once
// Test-only instruction preparation; production executes EnemyRuntime directly.
namespace enemy_vm_test {
namespace e=th20::source::ecl;
struct NoEngine final:e::Engine {
    std::int32_t read_integer(std::int32_t) override{throw std::logic_error("Unexpected external ECL integer");}
    std::uint32_t& integer_destination(std::int32_t) override{throw std::logic_error("Unexpected external ECL integer destination");}
    float read_float(std::int32_t) override{throw std::logic_error("Unexpected external ECL float");}
    std::uint32_t& float_destination(std::int32_t) override{throw std::logic_error("Unexpected external ECL float destination");}
    int execute_entity_opcode(e::Runtime&,e::Instruction) override{throw std::logic_error("Unexpected entity instruction");}
};
struct Manager final:gp::ScriptManager {
    std::int32_t read_integer(std::int32_t) override{throw std::logic_error("Unexpected external enemy integer");}
    std::uint32_t* integer_destination(std::int32_t) override{throw std::logic_error("Unexpected external enemy integer destination");}
    float read_float(std::int32_t) override{throw std::logic_error("Unexpected external enemy float");}
    std::uint32_t* float_destination(std::int32_t) override{throw std::logic_error("Unexpected external enemy float destination");}
    int execute_opcode() override{throw std::logic_error("Unexpected entity instruction");}
};
void put32(std::vector<std::uint8_t>& bytes,std::size_t offset,std::uint32_t value){std::memcpy(bytes.data()+offset,&value,4);}
void put16(std::vector<std::uint8_t>& bytes,std::size_t offset,std::uint16_t value){std::memcpy(bytes.data()+offset,&value,2);}
std::vector<std::uint8_t> instruction(unsigned op,std::vector<std::uint32_t> arguments={},unsigned mask=0,unsigned rank=255,int time=0,unsigned drop=0){
    std::vector<std::uint8_t> bytes(16+arguments.size()*4);put32(bytes,0,time);put16(bytes,4,std::uint16_t(op));put16(bytes,6,std::uint16_t(bytes.size()));put16(bytes,8,std::uint16_t(mask));
    bytes[10]=std::uint8_t(rank);bytes[11]=std::uint8_t(arguments.size());bytes[12]=std::uint8_t(drop);for(std::size_t i=0;i<arguments.size();++i)put32(bytes,16+i*4,arguments[i]);return bytes;
}
void append(std::vector<std::uint8_t>& a,const std::vector<std::uint8_t>& b){a.insert(a.end(),b.begin(),b.end());}
struct State {
    gp::ScriptLoader scripts;
    std::vector<std::vector<std::uint8_t>> code;
    Manager manager;
    explicit State(EnemyFixture& host,e::Runtime& input):scripts(host){
        for(auto& sub:input.program->subroutines){code.emplace_back(std::size_t(16),std::uint8_t(0));append(code.back(),sub.bytes);}
        for(std::size_t i=0;i<code.size();++i)scripts.records.push_back({input.program->subroutines[i].name.c_str(),code[i].data()});
        scripts.subroutine_count=std::uint32_t(code.size());manager.loader=&scripts;manager.reset();auto& r=manager.main;
        r.time=input.time;r.subroutine=input.subroutine;r.instruction_offset=input.instruction_offset;r.async_id=input.async_id;r.field_2c=input.async_field_2c;r.rank=input.rank_mask;r.flags=input.async_flags;
        std::memset(r.padding_31,0,3);r.stack.words.assign(input.stack.words.begin(),input.stack.words.end());r.stack.pointer=input.stack.pointer;r.stack.frame_base=input.stack.frame_base;
        r.interpolators.assign(input.interpolators.begin(),input.interpolators.end());
    }
};
bool same_runtime(const gp::EnemyRuntime& a,const gp::EnemyRuntime& b){
    return std::memcmp(&a,&b,12)==0&&a.stack.pointer==b.stack.pointer&&a.stack.frame_base==b.stack.frame_base&&a.stack.words==b.stack.words
      &&a.async_id==b.async_id&&a.field_2c==b.field_2c&&a.rank==b.rank&&a.flags==b.flags
      &&a.stack.words.capacity()==b.stack.words.capacity()&&a.interpolators.capacity()==b.interpolators.capacity()
      &&a.interpolators.size()==b.interpolators.size()&&(!a.interpolators.size()||std::memcmp(a.interpolators.data(),b.interpolators.data(),a.interpolators.size()*56)==0);
}
}
