
// block 00462890
00462890  push       edi
00462891  xor        edi, edi
00462893  cmp        ecx, edi
00462895  je         0x462900

// block 00462897
00462897  lea        eax, [edx + 0x14]
0046289a  cmp        eax, edi
0046289c  je         0x4628ab

// block 0046289e
0046289e  mov        edi, edi

// block 004628a0
004628a0  cmp        dword ptr [eax], ecx
004628a2  je         0x4628bf

// block 004628a4
004628a4  mov        eax, dword ptr [eax + 4]
004628a7  cmp        eax, edi
004628a9  jne        0x4628a0

// block 004628ab
004628ab  lea        eax, [edx + 0x38]
004628ae  cmp        eax, edi
004628b0  je         0x462900

// block 004628b2
004628b2  cmp        dword ptr [eax], ecx
004628b4  je         0x4628bf

// block 004628b6
004628b6  mov        eax, dword ptr [eax + 4]
004628b9  cmp        eax, edi
004628bb  jne        0x4628b2

// block 004628bd
004628bd  pop        edi
004628be  ret        

// block 004628bf
004628bf  push       esi
004628c0  mov        esi, dword ptr [eax + 8]
004628c3  cmp        esi, edi
004628c5  je         0x4628ff

// block 004628c7
004628c7  mov        edx, dword ptr [eax + 4]
004628ca  cmp        edx, edi
004628cc  je         0x4628d1

// block 004628ce
004628ce  mov        dword ptr [edx + 8], esi

// block 004628d1
004628d1  mov        edx, dword ptr [eax + 8]
004628d4  cmp        edx, edi
004628d6  je         0x4628de

// block 004628d8
004628d8  mov        esi, dword ptr [eax + 4]
004628db  mov        dword ptr [edx + 4], esi

// block 004628de
004628de  mov        dword ptr [eax + 4], edi
004628e1  mov        dword ptr [eax + 8], edi
004628e4  test       byte ptr [ecx + 4], 1
004628e8  mov        dword ptr [ecx + 8], edi
004628eb  je         0x4628ff

// block 004628ed
004628ed  push       ecx
004628ee  mov        dword ptr [ecx + 8], edi
004628f1  mov        dword ptr [ecx + 0xc], edi
004628f4  mov        dword ptr [ecx + 0x10], edi
004628f7  call       0x46ca4f

// block 004628fc
004628fc  add        esp, 4

// block 004628ff
004628ff  pop        esi

// block 00462900
00462900  pop        edi
00462901  ret        
