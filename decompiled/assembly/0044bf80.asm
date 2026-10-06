
// block 0044bf80
0044bf80  mov        eax, dword ptr [ecx + 4]
0044bf83  cmp        eax, -1
0044bf86  jne        0x44bf8d

// block 0044bf88
0044bf88  xor        al, al
0044bf8a  ret        8

// block 0044bf8d
0044bf8d  mov        ecx, dword ptr [esp + 8]
0044bf91  mov        edx, dword ptr [esp + 4]
0044bf95  push       ecx
0044bf96  push       0
0044bf98  push       edx
0044bf99  push       eax
0044bf9a  call       dword ptr [0x4980a0]

// block 0044bfa0
0044bfa0  mov        al, 1
0044bfa2  ret        8
