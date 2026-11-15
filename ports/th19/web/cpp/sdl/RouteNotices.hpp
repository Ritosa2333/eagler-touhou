#pragma once
#include <vector>
#include <initializer_list>

// Original 518e80 calls 413390 in this order. A notification is not
// necessarily one character: IDs 1, 9 and 10 each announce multiple grants.
inline std::vector<int> native_route_notices(int character, int clear_count, bool all_vs) {
    std::vector<int> ids;
    auto add=[&](std::initializer_list<int> values){ids.insert(ids.end(),values);};
    if(clear_count>=10)add({9});
    if(clear_count>=13)add({10});
    if(clear_count>=16)add({11});
    switch(character){
    case 0:add({1,17,21,22});break;
    case 1:add({2,5,3,15,19,22});break;
    case 2:add({2,4,8,14,18,22});break;
    case 3:add({6,7,17,20,16});break; // native case 3 falls through case 7
    case 4:add({7,14,18});break;
    case 5:add({8,19,14});break;
    case 6:add({7,19,15});break;
    case 7:add({20,16});break;
    case 8:add({18,15});break;
    case 9:add({16,20});break;
    case 13:add({13});break;
    case 17:add({12});break;
    }
    if(all_vs)add({23});
    return ids;
}
