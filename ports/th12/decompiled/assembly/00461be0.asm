
// block 00461be0
00461be0  mov        eax, dword ptr [esi + 0x8856b8]
00461be6  push       edi
00461be7  mov        edi, 0x10000000
00461bec  test       eax, eax
00461bee  je         0x461c09

// block 00461bf0
00461bf0  mov        ecx, dword ptr [eax + 4]
00461bf3  mov        eax, dword ptr [eax]
00461bf5  cmp        dword ptr [eax + 0x3f8], edx
00461bfb  jne        0x461c03

// block 00461bfd
00461bfd  or         dword ptr [eax + 0x47c], edi

// block 00461c03
00461c03  mov        eax, ecx
00461c05  test       ecx, ecx
00461c07  jne        0x461bf0

// block 00461c09
00461c09  mov        eax, dword ptr [esi + 0x8856c0]
00461c0f  test       eax, eax
00461c11  je         0x461c2c

// block 00461c13
00461c13  mov        ecx, dword ptr [eax + 4]
00461c16  mov        eax, dword ptr [eax]
00461c18  cmp        dword ptr [eax + 0x3f8], edx
00461c1e  jne        0x461c26

// block 00461c20
00461c20  or         dword ptr [eax + 0x47c], edi

// block 00461c26
00461c26  mov        eax, ecx
00461c28  test       ecx, ecx
00461c2a  jne        0x461c13

// block 00461c2c
00461c2c  pop        edi
00461c2d  ret        
