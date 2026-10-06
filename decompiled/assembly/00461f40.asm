
// block 00461f40
00461f40  mov        ecx, dword ptr [eax]
00461f42  mov        edx, dword ptr [0x4ce8cc]
00461f48  push       ecx
00461f49  call       0x461920

// block 00461f4e
00461f4e  test       eax, eax
00461f50  je         0x461f61

// block 00461f52
00461f52  mov        ecx, dword ptr [esp + 4]
00461f56  mov        edx, dword ptr [eax + 0x3f8]
00461f5c  call       0x454fa0

// block 00461f61
00461f61  ret        4
