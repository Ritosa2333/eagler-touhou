
// block 00461f10
00461f10  mov        ecx, dword ptr [eax]
00461f12  mov        edx, dword ptr [0x4ce8cc]
00461f18  push       ecx
00461f19  call       0x461920

// block 00461f1e
00461f1e  test       eax, eax
00461f20  je         0x461f2f

// block 00461f22
00461f22  mov        ecx, dword ptr [esp + 8]
00461f26  mov        edx, dword ptr [esp + 4]
00461f2a  call       0x454b80

// block 00461f2f
00461f2f  ret        8
