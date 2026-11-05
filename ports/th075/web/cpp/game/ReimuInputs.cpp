#include "ReimuInputs.hpp"
namespace th075 {
u8 recognize_reimu_command(const InputHistory& history,i8 facing){
    struct Motion {const char* pattern;u8 command,length,window;};
    // Order is part of the game's command priority, including overlapping
    // neutral/crouching jump motions and the direction-relative dash inputs.
    static constexpr Motion motions[]={
        {"421B",41,4,15},{"421A",40,4,15},{"623C",32,4,15},
        {"623B",31,4,15},{"623A",30,4,15},{"214B",21,4,15},
        {"214A",20,4,15},{"236D",13,4,15},{"236B",11,4,15},
        {"236A",10,4,15},{"2N2D",103,4,15},{"2N2C",102,4,15},
        {"2N2B",101,4,15},{"2N2A",100,4,15},{"NRNR",1,4,15},
        {"NLNL",2,4,15},{"N29",5,3,10},{"N39",5,3,10},
        {"N19",5,3,10},{"N27",4,3,10},{"N37",4,3,10},
        {"N17",4,3,10},{"N28",3,3,10},{"N38",3,3,10},{"N18",3,3,10}
    };
    for(const auto& motion:motions)
        if(history.match(motion.pattern,motion.length,motion.window,facing))return motion.command;
    return 0;
}
}
