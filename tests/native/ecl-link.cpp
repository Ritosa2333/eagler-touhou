#include "ecl/EclProgram.hpp"
#include "ecl/EclVm.hpp"
#include "../ecl-fixture.hpp"
#include <cmath>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

using namespace th128::ecl;
using namespace ecl_fixture;

Program parsed(const Bytes& bytes) {
    Program program;
    std::string error;
    require(program.load(bytes.data(),bytes.size(),error),error);
    return program;
}

void synthetic() {
    auto root=parsed(program({{"main",join({instruction(11,stringWords("included"),0,0,255,1),instruction(1)})}}, {}, {"dep.ecl"}));
    auto dependency=parsed(program({{"included",join({instruction(900,{std::uint32_t(-9999),floatBits(-9998.f)},0,3),instruction(10)})}}));
    auto original=root;
    std::string error;
    require(!root.linkIncludes({},error),"missing include must fail");
    require(!root.find("included") && root.find("main"),"failed link is transactional");
    require(root.linkIncludes({{"dep.ecl",dependency}},error),error);
    require(root.sourceCount()==2,"two linked sources");
    require(root.sourceBytes()==root.resource().bytes.size()+dependency.resource().bytes.size(),"linked byte count");
    require(root.find("included") && !original.find("included"),"immutable pre-link snapshot");
    int reads=0;
    Vm vm(root);
    require(vm.setReferenceResolver([&](std::int32_t address,bool floating)->std::optional<Value>{
        ++reads;
        if(address==-9999&&!floating)return Value::integer(123);
        if(address==-9998&&floating)return Value::floating(2.5f);
        return std::nullopt;
    },{},error),error);
    int events=0;
    require(vm.registerHostOpcode(900,[&](HostCall& call){
        std::int32_t integer=0;float scalar=0;
        if(!call.readInteger(0,integer)||!call.readFloat(1,scalar))return false;
        require(integer==123&&scalar==2.5f,"typed provider values");++events;return true;
    },error),error);
    require(vm.start("main"),"start linked root");
    require(vm.runTick().status==VmStatus::Finished&&events==1&&reads==2,"linked call resolves via bounded global provider");
    auto collision=parsed(program({{"main",instruction(1)}}));
    auto duplicate=original;
    require(!duplicate.linkIncludes({{"dep.ecl",collision}},error),"ambiguous subroutine collision must fail");
    require(!duplicate.find("included"),"collision does not partially publish");
    auto cycle=parsed(program({{"helper",instruction(10)}},{},{"dep.ecl"}));
    auto cyclic=original;
    require(!cyclic.linkIncludes({{"dep.ecl",cycle}},error),"include cycle must fail");
    for(int maximum:{63,64}) {
        std::vector<std::pair<std::string,Program>> chain;
        std::vector<std::string> direct;
        for(int index=0;index<=maximum;++index) {
            const auto name="d"+std::to_string(index)+".ecl";
            const std::vector<std::string> includes=index==0?std::vector<std::string>{}:std::vector<std::string>{"d"+std::to_string(index-1)+".ecl"};
            chain.emplace_back(name,parsed(program({{"s"+std::to_string(index),instruction(10)}},{},includes)));
            direct.push_back(name);
        }
        for(int reverse=0;reverse<2;++reverse) {
            auto depthRoot=parsed(program({{"main",instruction(1)}},{},direct));
            const auto bytes=depthRoot.sourceBytes();
            const bool linked=depthRoot.linkIncludes(chain,error);
            require(linked==(maximum==63),"include depth must not depend on root enumeration order");
            if(!linked)require(depthRoot.sourceBytes()==bytes&&depthRoot.sourceCount()==1&&!depthRoot.find("s0"),"depth rejection is transactional");
            std::reverse(direct.begin(),direct.end());
        }
    }
    auto writes=parsed(program({{"main",join({instruction(42,{10}),instruction(43,{std::uint32_t(-9000)},0,1),instruction(1)})}}));
    Vm writer(writes);
    int stored=0;
    require(writer.setReferenceResolver({},[&](std::int32_t address,Value value){stored=value.asInteger();return address==-9000;},error),error);
    require(writer.start("main")&&writer.runTick().status==VmStatus::Finished&&stored==10,"global stores are explicit");
    Vm denied(root);
    require(denied.setReferenceResolver([](std::int32_t,bool)->std::optional<Value>{return std::nullopt;},{},error),error);
    require(denied.registerHostOpcode(900,[](HostCall& call){std::int32_t value=0;return call.readInteger(0,value);},error),error);
    require(denied.start("main")&&denied.runTick().status==VmStatus::Faulted,"unknown global never defaults to zero");
    require(denied.error().code==ErrorCode::UnsupportedReference,"unknown provider failure is explicit");
    Vm thrown(root);
    require(thrown.setReferenceResolver([](std::int32_t,bool)->std::optional<Value>{throw std::runtime_error("provider fixture");},{},error),error);
    require(thrown.registerHostOpcode(900,[](HostCall& call){std::int32_t value=0;return call.readInteger(0,value);},error),error);
    require(thrown.start("main")&&thrown.runTick().status==VmStatus::Faulted&&thrown.error().code==ErrorCode::HostFailure,"provider exceptions become sticky diagnostics");
    Vm reentrant(root);
    require(reentrant.setReferenceResolver([&](std::int32_t,bool)->std::optional<Value>{reentrant.start("main");return Value::integer(1);},{},error),error);
    require(reentrant.registerHostOpcode(900,[](HostCall& call){std::int32_t value=0;return call.readInteger(0,value);},error),error);
    require(reentrant.start("main")&&reentrant.runTick().status==VmStatus::Faulted&&reentrant.error().code==ErrorCode::ReentrantCall,"reference providers cannot reenter VM");
    std::puts("PASS linked include transactions and typed global providers");
}

int main(int argc,char** argv) {
    try {
        synthetic();
        if(argc>1) {
            std::vector<std::pair<std::string,Program>> inputs;
            for(int index=1;index<argc;++index) {
                std::ifstream stream(argv[index],std::ios::binary);
                require(bool(stream),"actual ECL missing");
                Bytes bytes((std::istreambuf_iterator<char>(stream)),{});
                const std::string path=argv[index];
                const auto name=path.substr(path.find_last_of("/\\")+1);
                inputs.emplace_back(name,parsed(bytes));
            }
            int linked=0;
            for(const auto& [name,original]:inputs) {
                if(!original.find("main"))continue;
                auto candidate=original;std::string error;
                require(candidate.linkIncludes(inputs,error),name+": "+error);
                require(candidate.sourceCount()>=2&&candidate.find("main"),"actual linked source set");
                ++linked;
            }
            require(linked==16,"actual stage count");
            std::puts("PASS all 16 actual stages link their complete ECL include closure");
        }
        return 0;
    } catch(const std::exception& error) { std::fprintf(stderr,"FAIL ECL link: %s\n",error.what()); return 1; }
}
