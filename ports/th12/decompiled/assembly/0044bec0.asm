
// block 0044bec0
0044bec0  push       ecx
0044bec1  cmp        dword ptr [ecx + 8], 0x80000000
0044bec8  mov        dword ptr [esp], 0
0044becf  je         0x44bed7

// block 0044bed1
0044bed1  xor        eax, eax
0044bed3  pop        ecx
0044bed4  ret        8

// block 0044bed7
0044bed7  mov        edx, dword ptr [esp + 0xc]
0044bedb  mov        ecx, dword ptr [ecx + 4]
0044bede  push       0
0044bee0  lea        eax, [esp + 4]
