#pragma once
#include <algorithm>
#include <cstdint>
namespace th19 {
// EXE 0x5c5c20, 32-byte character records: background at +0, MSG music ID at +28.
// 517790 chooses random side & 1; 518b2c reuses that side for BGM.
struct VsScene {int background,music;};
inline constexpr VsScene VS_SCENES[19]={
 {1,1}, // character 0
 {0,2}, // character 1
 {5,3}, // character 2
 {3,4}, // character 3
 {1,5}, // character 4
 {12,6}, // character 5
 {3,7}, // character 6
 {9,8}, // character 7
 {4,9}, // character 8
 {15,10}, // character 9
 {8,11}, // character 10
 {6,12}, // character 11
 {11,13}, // character 12
 {13,14}, // character 13
 {16,15}, // character 14
 {10,16}, // character 15
 {14,17}, // character 16
 {7,18}, // character 17
 {13,19}, // character 18
};
inline VsScene vsScene(int left,int right,unsigned draw){return VS_SCENES[std::clamp((draw&1)?right:left,0,18)];}
}
