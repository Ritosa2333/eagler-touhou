#include "../../src/message.hpp"
message_system::File file;message_system::VM vm;int snapshot[10];std::vector<message_system::TextAction> actions;std::string json;
extern "C" {
int test_load(const uint8_t* p,int size){return file.load(p,size);}
int test_start(int index){return vm.start(file,index);}
void test_tick(int held,int pressed){actions.clear();vm.tick(held,pressed,[](auto){actions.insert(actions.end(),vm.textActions.begin(),vm.textActions.end());});}
const int* test_state(){snapshot[0]=vm.active?0:-1;snapshot[1]=vm.clock;snapshot[2]=vm.wait;snapshot[3]=int(vm.pc)-int(file.entries[vm.entry].offset);snapshot[4]=vm.flags;snapshot[5]=vm.typingDelay;snapshot[6]=vm.failed;snapshot[7]=vm.line;snapshot[8]=vm.textInitialized;snapshot[9]=vm.speaker;return snapshot;}
const char* test_text(){
    json="[";for(const auto& action:actions){if(json.size()>1)json+=",";const auto& r=action.record;std::string hex;static constexpr char digits[]="0123456789abcdef";for(unsigned char c:r.bytes){hex+=digits[c>>4];hex+=digits[c&15];}
        json+="{\"slot\":"+std::to_string(action.slot)+",\"textHex\":\""+hex+"\",\"offset\":"+std::to_string(r.offset)+",\"spacing\":"+std::to_string(r.spacing)+",\"style\":"+std::to_string(r.style)+",\"color\":"+std::to_string(r.color)+",\"interrupt\":"+std::to_string(action.interrupt)+"}";
    }json+="]";return json.c_str();
}
}
