
// block 00461ce0
00461ce0  mov        ecx, dword ptr [eax]
00461ce2  mov        edx, dword ptr [0x4ce8cc]
00461ce8  push       ecx
00461ce9  call       0x461920

// block 00461cee
00461cee  test       eax, eax
00461cf0  je         0x461d3b

// block 00461cf2
00461cf2  mov        ecx, dword ptr [eax + 0x47c]
00461cf8  and        ecx, 0xfffbffff
00461cfe  mov        edx, 2
00461d03  or         ecx, edx
00461d05  cmp        dword ptr [eax + 0x18], 0
00461d09  mov        dword ptr [eax + 0x47c], ecx
00461d0f  jne        0x461d3b

// block 00461d11
00461d11  mov        eax, dword ptr [eax + 0x14]
00461d14  test       eax, eax
00461d16  je         0x461d3b

// block 00461d18
00461d18  jmp        0x461d20

// block 00461d20
00461d20  mov        ecx, dword ptr [eax]
00461d22  or         dword ptr [ecx + 0x47c], edx
00461d28  mov        ecx, dword ptr [eax]
00461d2a  and        dword ptr [ecx + 0x47c], 0xfffbffff
00461d34  mov        eax, dword ptr [eax + 4]
00461d37  test       eax, eax
00461d39  jne        0x461d20

// block 00461d3b
00461d3b  ret        
