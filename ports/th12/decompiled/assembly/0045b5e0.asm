
// block 0045b5e0
0045b5e0  mov        eax, esi
0045b5e2  call       0x45b210

// block 0045b5e7
0045b5e7  test       eax, eax
0045b5e9  je         0x45b5f1

// block 0045b5eb
0045b5eb  or         eax, 0xffffffff
0045b5ee  ret        4

// block 0045b5f1
0045b5f1  mov        ecx, dword ptr [esp + 4]
0045b5f5  push       0
0045b5f7  mov        eax, esi
0045b5f9  call       0x459e50

// block 0045b5fe
0045b5fe  ret        4
