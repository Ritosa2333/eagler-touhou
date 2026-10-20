
// block 00461350
00461350  mov        ecx, dword ptr [0x4ce8cc]
00461356  lea        edx, [ebx + 4]
00461359  mov        dword ptr [edx], ebx
0046135b  mov        dword ptr [edx + 4], 0
00461362  mov        dword ptr [edx + 8], 0
00461369  cmp        dword ptr [ecx + 0x8856c0], 0
00461370  jne        0x46137a

// block 00461372
00461372  mov        dword ptr [ecx + 0x8856c0], edx
00461378  jmp        0x46139a

// block 0046137a
0046137a  push       esi
0046137b  mov        esi, dword ptr [ecx + 0x8856c4]
00461381  push       edi
00461382  mov        edi, dword ptr [esi + 4]
00461385  test       edi, edi
00461387  je         0x461392

// block 00461389
00461389  mov        dword ptr [edx + 4], edi
0046138c  mov        edi, dword ptr [esi + 4]
0046138f  mov        dword ptr [edi + 8], edx

// block 00461392
00461392  mov        dword ptr [esi + 4], edx
00461395  pop        edi
00461396  mov        dword ptr [edx + 8], esi
00461399  pop        esi

// block 0046139a
0046139a  mov        dword ptr [ecx + 0x8856c4], edx
004613a0  mov        edx, 1
004613a5  add        dword ptr [ecx + 0x88ed48], edx
004613ab  cmp        dword ptr [ecx + 0x88ed48], 0
004613b2  jne        0x4613ba

// block 004613b4
004613b4  add        dword ptr [ecx + 0x88ed48], edx

// block 004613ba
004613ba  mov        edx, dword ptr [ecx + 0x88ed48]
004613c0  mov        dword ptr [ebx], edx
004613c2  mov        ecx, dword ptr [ecx + 0x88ed48]
004613c8  mov        dword ptr [eax], ecx
004613ca  ret        
