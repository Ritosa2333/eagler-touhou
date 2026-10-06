
// block 0044be90
0044be90  push       esi
0044be91  mov        esi, ecx
0044be93  mov        eax, dword ptr [esi + 4]
0044be96  cmp        eax, -1
0044be99  je         0x44beb0

// block 0044be9b
0044be9b  push       eax
0044be9c  call       dword ptr [0x4980a4]

// block 0044bea2
0044bea2  mov        dword ptr [esi + 4], 0xffffffff
0044bea9  mov        dword ptr [esi + 8], 0

// block 0044beb0
0044beb0  pop        esi
0044beb1  ret        
