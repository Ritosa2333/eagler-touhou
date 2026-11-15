#pragma once
#include <algorithm>
#include <vector>
namespace th19 {
// Recovered from th19.exe 0x515200 and front.anm script 17.
// Award rules belong to character gameplay code, not to the HUD.
inline std::vector<int> bombHudSprites(int stock,int fragments) {
    std::vector<int> ids(std::clamp(stock,0,7),19);
    if(ids.size()<7 && fragments>0)ids.push_back(16+std::clamp(fragments,1,2));
    return ids;
}
}
