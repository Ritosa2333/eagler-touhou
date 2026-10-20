
// block 00461a70
00461a70  mov        eax, dword ptr [esp + 4]
00461a74  mov        edx, dword ptr [0x4ce8cc]
00461a7a  push       eax
00461a7b  call       0x461920

// block 00461a80
00461a80  test       eax, eax
00461a82  je         0x461aaf

// block 00461a84
00461a84  mov        edx, 0x10000000
00461a89  or         dword ptr [eax + 0x47c], edx
00461a8f  cmp        dword ptr [eax + 0x18], 0
00461a93  jne        0x461aaf

// block 00461a95
00461a95  mov        eax, dword ptr [eax + 0x14]
00461a98  test       eax, eax
00461a9a  je         0x461aaf

// block 00461a9c
00461a9c  lea        esp, [esp]

// block 00461aa0
00461aa0  mov        ecx, dword ptr [eax]
00461aa2  or         dword ptr [ecx + 0x47c], edx
00461aa8  mov        eax, dword ptr [eax + 4]
00461aab  test       eax, eax
00461aad  jne        0x461aa0

// block 00461aaf
00461aaf  ret        4
