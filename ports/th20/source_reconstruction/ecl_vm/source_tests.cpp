#include "vm.hpp"
#include <cmath>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <utility>
namespace e=th20::source::ecl;
namespace {
void require(bool value,const char* message){if(!value)throw std::runtime_error(message);}
std::vector<std::uint8_t> ins(unsigned op,std::vector<std::uint32_t> args={}){
    std::vector<std::uint8_t> data(16+args.size()*4);const auto length=static_cast<std::uint16_t>(data.size()),code=static_cast<std::uint16_t>(op);
    std::memcpy(data.data()+4,&code,2);std::memcpy(data.data()+6,&length,2);data[10]=255;data[11]=static_cast<std::uint8_t>(args.size());
    for(std::size_t i=0;i<args.size();++i)std::memcpy(data.data()+16+i*4,&args[i],4);return data;
}
void add(std::vector<std::uint8_t>& a,const std::vector<std::uint8_t>& b){a.insert(a.end(),b.begin(),b.end());}
struct TraceEngine:e::Engine {
    std::vector<std::pair<int,std::uint32_t>> trace;
    std::int32_t read_integer(std::int32_t)override{throw std::runtime_error("Unrequested engine variable");}
    std::uint32_t& integer_destination(std::int32_t)override{throw std::runtime_error("Unrequested engine destination");}
    float read_float(std::int32_t)override{throw std::runtime_error("Unrequested engine variable");}
    std::uint32_t& float_destination(std::int32_t)override{throw std::runtime_error("Unrequested engine destination");}
    std::int32_t execute_entity_opcode(e::Runtime& r,e::Instruction instruction)override{
        // An explicit test callback, never compiled as a production opcode.
        require(instruction.opcode()==65535,"Unexpected test callback opcode");trace.emplace_back(r.async_id,r.stack.local(0));return 0;
    }
};
}
int main(){try{
    TraceEngine engine;e::Program program;
    auto main=ins(40,{4});auto first=ins(15,{4,static_cast<unsigned>('b'),0x6969,11});first[11]=2;
    auto second=ins(16,{4,static_cast<unsigned>('c'),7,0x6969,22});second[11]=3;
    add(main,first);add(main,second);add(main,ins(23,{1}));add(main,ins(65535));add(main,ins(23,{1000}));add(main,ins(1));
    auto child=ins(40,{4});add(child,ins(65535));add(child,ins(10));
    program.subroutines={{"a",main},{"b",child},{"c",child}};
    e::Scheduler scheduler(program,engine);scheduler.main.subroutine=0;scheduler.main.instruction_offset=0;
    require(scheduler.tick(1)==0,"Main unexpectedly terminated");require(engine.trace.empty(),"New async tasks ran in spawning traversal");
    auto order=scheduler.task_order();require(order.size()==3&&order[1]->async_id==7&&order[2]->async_id==-1,"Async insertion order");
    require(order[1]->stack.absolute(20)==22&&order[2]->stack.absolute(20)==11,"Async converted argument placement");
    require(scheduler.find(7)==order[1],"Async identifier lookup");
    require(scheduler.tick(1)==0,"Main unexpectedly terminated on second tick");
    require(engine.trace.size()==3&&engine.trace[0].first==0&&engine.trace[1]==std::make_pair(7,22u)&&engine.trace[2]==std::make_pair(-1,11u),"Async callback order or argument values");
    require(scheduler.task_order().size()==1&&scheduler.current==&scheduler.main,"Ended tasks not removed or active context not restored");
    for(float value:{e::bits_float(0x7fc12345),e::bits_float(0xffc12345)}){
        require(std::isnan(e::math::sine(value))&&std::isnan(e::math::cosine(value))&&std::isnan(e::math::square_root(value)),"New CRT NaN result classification");
    }
    e::Stack stack;require(stack.enter_frame(252),"Valid stack frame rejected");stack.leave_frame();require(!stack.enter_frame(256),"Original 256-byte frame limit not enforced");
    auto shared=ins(50);shared[12]=16;add(shared,ins(23,{5}));add(shared,ins(1));
    e::Program first_view,second_view,owned;
    first_view.append_borrowed("main",shared.data(),shared.size());second_view.append_borrowed("main",shared.data(),shared.size());
    owned.subroutines={{"main",shared}};
    e::Scheduler first_run(first_view,engine),second_run(second_view,engine);
    first_run.main.subroutine=second_run.main.subroutine=0;first_run.main.instruction_offset=second_run.main.instruction_offset=0;
    first_run.main.stack.push(1,'i');first_run.main.stack.push(2,'i');
    require(first_run.tick(1)==0&&first_run.main.stack.peek(-8,'i')==3,"Borrowed script arithmetic differs");
    require(shared[12]==0&&second_run.main.current().stack_drop_bytes()==0,"VM write is not shared between players");
    require(owned.subroutines[0].bytes[12]==16&&first_view.subroutines[0].bytes.empty(),"Borrowed storage accidentally copied or changed owning storage");
    second_view.append_borrowed("aaa",shared.data(),shared.size());second_view.append_borrowed("aaa",shared.data()+16,shared.size()-16);
    require(second_view.find("aaa")==1&&second_view.subroutines[1].data()==shared.data()+16,"Borrowed duplicate-name ordering differs");
    std::cout<<"Source async ownership/traversal, callback args, frame limit and CRT NaN checks passed\n";return 0;
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
