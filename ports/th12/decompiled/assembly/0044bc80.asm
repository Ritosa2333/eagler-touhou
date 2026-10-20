
// block 0044bc80
0044bc80  mov        eax, dword ptr [edx]
0044bc82  push       esi
0044bc83  lea        esi, [eax + 1]

// block 0044bc86
0044bc86  mov        cl, byte ptr [eax]
0044bc88  inc        eax
0044bc89  test       cl, cl
0044bc8b  jne        0x44bc86

// block 0044bc8d
0044bc8d  sub        eax, esi
0044bc8f  lea        ecx, [eax + 1]
0044bc92  mov        eax, ecx
0044bc94  and        eax, 0x80000003
0044bc99  jns        0x44bca0

// block 0044bc9b
0044bc9b  dec        eax
0044bc9c  or         eax, 0xfffffffc
0044bc9f  inc        eax

// block 0044bca0
0044bca0  je         0x44bcab

// block 0044bca2
0044bca2  mov        esi, 4
0044bca7  sub        esi, eax
0044bca9  add        ecx, esi

// block 0044bcab
0044bcab  add        dword ptr [edx], ecx
0044bcad  mov        eax, dword ptr [edx]
0044bcaf  pop        esi
0044bcb0  ret        
