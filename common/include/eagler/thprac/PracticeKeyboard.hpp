#pragma once
#include <imgui.h>

namespace eagler::thprac {
// VK indices match the purple UI. Feed characters independently of the game
// menu: advanced tools and pause-page numeric fields use TempInputText too.
template <class Sample> void keyboard_frame(ImGuiIO& io, Sample sample) {
    const int map[] = {9,37,39,38,40,33,34,36,35,45,46,8,32,13,27,13,65,67,86,88,89,90};
    static_assert(sizeof(map)/sizeof(*map) == ImGuiKey_COUNT);
    for (int i=0;i<ImGuiKey_COUNT;++i) io.KeyMap[i]=map[i];
    struct Key { int vk, scan; char character; };
    const Key keys[] = {
        {48,39,'0'},{49,30,'1'},{50,31,'2'},{51,32,'3'},{52,33,'4'},
        {53,34,'5'},{54,35,'6'},{55,36,'7'},{56,37,'8'},{57,38,'9'},
        {96,98,'0'},{97,89,'1'},{98,90,'2'},{99,91,'3'},{100,92,'4'},
        {101,93,'5'},{102,94,'6'},{103,95,'7'},{104,96,'8'},{105,97,'9'},
        {189,45,'-'},{190,55,'.'},{109,86,'-'},{110,99,'.'},
        {8,42,0},{9,43,0},{13,40,0},{27,41,0},{32,44,0},
        {33,75,0},{34,78,0},{35,77,0},{36,74,0},{37,80,0},{38,82,0},
        {39,79,0},{40,81,0},{45,73,0},{46,76,0},
        {65,4,0},{67,6,0},{86,25,0},{88,27,0},{89,28,0},{90,29,0}
    };
    io.KeyCtrl=sample(224)||sample(228);
    io.KeyShift=sample(225)||sample(229);
    io.KeyAlt=sample(226)||sample(230);
    io.KeySuper=sample(227)||sample(231);
    for (const auto& key:keys) {
        const bool down=sample(key.scan) || (key.vk==13 && sample(88));
        io.KeysDown[key.vk]=down;
        if (key.character && down && io.KeysDownDuration[key.vk]<0 &&
            !io.KeyCtrl && !io.KeyAlt && !io.KeySuper)
            io.AddInputCharacter(static_cast<unsigned>(key.character));
    }
}
}
