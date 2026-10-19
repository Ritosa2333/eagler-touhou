#include "MarisaState.hpp"
namespace th075 {
u8 recognize_marisa_command(const InputHistory& history,i8 facing){
    struct Motion {const char* pattern;u8 command,length,window;};
    static constexpr Motion motions[]={
        {"236236D",113,7,30},{"4136B",51,5,15},{"4136A",50,5,15},
        {"623B",31,4,15},{"623A",30,4,15},
        {"214D",23,4,15},{"214C",22,4,15},{"214B",21,4,15},{"214A",20,4,15},
        {"236D",13,4,15},{"236C",12,4,15},{"236B",11,4,15},{"236A",10,4,15},
        {"2N2D",103,4,15},{"2N2C",102,4,15},{"2N2B",101,4,15},{"2N2A",100,4,15},
        {"N29",5,3,10},{"N39",5,3,10},{"N19",5,3,10},
        {"N27",4,3,10},{"N37",4,3,10},{"N17",4,3,10},
        {"N28",3,3,10},{"N38",3,3,10},{"N18",3,3,10},
        {"NRNR",1,4,15},{"NLNL",2,4,15}
    };
    for(const auto& m:motions)if(history.match(m.pattern,m.length,m.window,facing))return m.command;
    return 0;
}
}
