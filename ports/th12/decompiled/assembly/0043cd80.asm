
// block 0043cd80
0043cd80  mov        eax, dword ptr [ecx + 0x1c]
0043cd83  xor        edx, edx
0043cd85  push       esi
0043cd86  cmp        eax, edx
0043cd88  je         0x43cd90

// block 0043cd8a
0043cd8a  mov        esi, dword ptr [ecx + 0x20]
0043cd8d  mov        dword ptr [eax + 8], esi

// block 0043cd90
0043cd90  mov        eax, dword ptr [ecx + 0x20]
0043cd93  cmp        eax, edx
0043cd95  je         0x43cd9d

// block 0043cd97
0043cd97  mov        esi, dword ptr [ecx + 0x1c]
0043cd9a  mov        dword ptr [eax + 4], esi

// block 0043cd9d
0043cd9d  mov        dword ptr [ecx + 0x20], edx
0043cda0  mov        dword ptr [ecx + 0x1c], edx
0043cda3  pop        esi
0043cda4  ret        
