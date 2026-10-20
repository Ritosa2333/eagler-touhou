
// block 00461ea0
00461ea0  mov        ecx, dword ptr [eax]
00461ea2  mov        edx, dword ptr [0x4ce8cc]
00461ea8  push       ecx
00461ea9  call       0x461920

// block 00461eae
00461eae  test       eax, eax
00461eb0  je         0x461ec1

// block 00461eb2
00461eb2  mov        ecx, dword ptr [esp + 4]
00461eb6  mov        edx, dword ptr [eax + 0x3f8]
00461ebc  call       0x454b80

// block 00461ec1
00461ec1  ret        4
