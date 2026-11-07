#pragma once
#include "Types.hpp"
#include "EclProgram.hpp"
#include <vector>
#include <array>
#include <string>
#include <memory>
#include <unordered_map>
namespace th19 {
class GameWorld;

// ECL expression-stack VM (th13 engine). Single flat value stack (two slots per
// value: [type, value]; type 0=int, 1=float). Synchronous `call` shares the stack;
// async `call` spawns a child VM that inherits the host game-object.
//
// A VM holds a *set* of EclPrograms (the root script plus its includes), so that
// `call` can resolve sub names across files (common.ecl -> wave01.ecl ...).
class EclVm {
public:
    EclVm()=default;
    EclVm(const EclVm&);
    EclVm& operator=(const EclVm&);
    EclVm(EclVm&&) noexcept;
    EclVm& operator=(EclVm&&) noexcept;
    // Retarget every async descendant after restoring into a different world.
    void restore_world(GameWorld* world);
    void attach_program(const EclProgram& program);   // append a program to the set
    void attach_world(GameWorld* w) noexcept {world_=w;}
    bool start(const char* sub_name);       // enter a sub at its first instruction
    bool step();                            // one frame; false => ended/error
    bool finished() const noexcept {return ended_;}
    bool error() const noexcept {return error_;}

    std::vector<std::unique_ptr<EclVm>> children;  // async children (call_async)

    u32 host_id=0;bool has_host=false;

    void push_int(i32 v);void push_float(float v);
    i32 pop_int();float pop_float();
    i32  var_int(i32 addr) const;float var_float(i32 addr) const;
    void set_var_int(i32 addr,i32 v);void set_var_float(i32 addr,float v);
    i32 current_sub() const noexcept {return sub_;}
    const char* current_sub_name() const noexcept {return prog_>=0&&sub_>=0?programs_[prog_]->subs[sub_].name.c_str():"";}
    u32 instruction_offset() const noexcept {return ip_;}

private:
    void rebind_tree(EclVm* root,GameWorld* world);
    struct Frame {i32 prog=-1;i32 sub=-1;u32 ip=0;i32 bp=0;double time=0;size_t locals_size=0;};
    std::vector<const EclProgram*> programs_;
    GameWorld* world_=nullptr;
    i32 prog_=-1;i32 sub_=-1;u32 ip_=0;i32 bp_=0;
    double time_=-1;                // first step executes script time zero
    bool waiting_=false;double wait_target_=0;u32 wait_size_=0;
    std::vector<i32> stack_;         // [type,value] flattened
    std::vector<Frame> frames_;      // synchronous call stack
    std::unordered_map<i32,i32> globals_;   // global vars keyed by raw negative addr
    std::vector<i32> locals_;
    bool ended_=false,error_=false;

    const EclProgram& prog() const {return *programs_[prog_];}
    const u8* stream() const {return prog().bytecode.data()+prog().subs[sub_].offset;}
    u32 stream_len() const {return prog().subs[sub_].instr_bytes;}
    i32 read_i32(u32 off);
    float read_f32(u32 off);
    bool decode_header(EclInstruction& ins) const;
    u32 param_off(u32 arg) const {return ip_+16+arg*4;}
    bool param_stack_ref(const EclInstruction& ins,u32 arg) const {return (ins.param_mask>>arg)&1;}
    i32 param_i32(const EclInstruction& ins,u32 arg);
    float param_f32(const EclInstruction& ins,u32 arg);
    bool resolve(const std::string& name,i32& out_prog,i32& out_sub) const;  // search all programs
    bool enter_sub(i32 prog_idx,i32 sub_idx);
    bool do_call(const std::string& sub_name,bool async,const EclInstruction* call_ins);
    bool do_return();
    struct FloatMotion {bool active=false;int dest=0,base=0,duration=0,elapsed=0,mode=0;float from=0,to=0;};
    std::array<FloatMotion,8> float_motions_{};
    void advance_float_motions();
    EclVm* tree_root_=nullptr;
    void stop_async_children();
    i32 async_id_=-1;
    bool stop_async_id(i32 id);
    bool step_self();
    bool game_object(const EclInstruction& ins);
};
// Enemy snapshots own independent VM trees; program bytecode remains immutable
// and must outlive both the running world and its retained checkpoints.
struct EclInstance:std::unique_ptr<EclVm> {
 using Base=std::unique_ptr<EclVm>;
 using Base::Base;using Base::operator=;
 EclInstance()=default;
 EclInstance(Base&& other):Base(std::move(other)){}
 EclInstance(const EclInstance& other):Base(other?std::make_unique<EclVm>(*other):nullptr){}
 EclInstance& operator=(const EclInstance& other){if(this!=&other){if(!other)reset();else if(*this)**this=*other;else Base::operator=(std::make_unique<EclVm>(*other));}return *this;}
 EclInstance(EclInstance&&) noexcept=default;
 EclInstance& operator=(EclInstance&&) noexcept=default;
};
}
