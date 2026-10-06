
// block 0040f690
0040f690  mov        edx, dword ptr [ecx + 0x118]
0040f696  push       esi
0040f697  mov        esi, 1
0040f69c  push       edi
0040f69d  mov        dword ptr [ecx + 0x120], 0
0040f6a7  mov        dword ptr [ecx + 0x130], 0
0040f6b1  lea        eax, [ecx + 0x94]
0040f6b7  lea        edi, [esi + 0x1f]
0040f6ba  lea        ebx, [ebx]

// block 0040f6c0
0040f6c0  test       dl, 1
0040f6c3  je         0x40f6e2

// block 0040f6c5
0040f6c5  inc        dword ptr [eax]
0040f6c7  cmp        dword ptr [eax], 8
0040f6ca  jb         0x40f6d2

// block 0040f6cc
0040f6cc  or         dword ptr [ecx + 0x130], esi

// block 0040f6d2
0040f6d2  cmp        dword ptr [eax], 0x1a
0040f6d5  jb         0x40f6e8

// block 0040f6d7
0040f6d7  or         dword ptr [ecx + 0x120], esi
0040f6dd  add        dword ptr [eax], -8
0040f6e0  jmp        0x40f6e8

// block 0040f6e2
0040f6e2  mov        dword ptr [eax], 0

// block 0040f6e8
0040f6e8  add        eax, 4
0040f6eb  shr        edx, 1
0040f6ed  add        esi, esi
0040f6ef  sub        edi, 1
0040f6f2  jne        0x40f6c0

// block 0040f6f4
0040f6f4  mov        eax, dword ptr [ecx + 0x118]
0040f6fa  mov        edx, dword ptr [ecx + 0x11c]
0040f700  xor        edx, eax
0040f702  mov        esi, edx
0040f704  and        esi, eax
0040f706  not        eax
0040f708  and        eax, edx
0040f70a  pop        edi
0040f70b  mov        dword ptr [ecx + 0x124], esi
0040f711  mov        dword ptr [ecx + 0x128], eax
0040f717  pop        esi
0040f718  ret        
