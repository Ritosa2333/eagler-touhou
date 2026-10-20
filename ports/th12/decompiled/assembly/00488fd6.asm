
// block 00488fd6
00488fd6  mov        edi, edi
00488fd8  push       ebp
00488fd9  mov        ebp, esp
00488fdb  mov        eax, dword ptr [ebp + 0xc]
00488fde  mov        ecx, dword ptr [ebp + 8]
00488fe1  push       3
00488fe3  pop        edx
00488fe4  sub        ecx, eax
00488fe6  push       esi

// block 00488fe7
00488fe7  mov        esi, dword ptr [eax]
00488fe9  mov        dword ptr [ecx + eax], esi
00488fec  add        eax, 4
00488fef  dec        edx
00488ff0  jne        0x488fe7

// block 00488ff2
00488ff2  pop        esi
00488ff3  pop        ebp
00488ff4  ret        
