
// block 00461e00
00461e00  mov        eax, dword ptr [eax]
00461e02  mov        edx, dword ptr [0x4ce8cc]
00461e08  push       eax
00461e09  call       0x461920

// block 00461e0e
00461e0e  test       eax, eax
00461e10  je         0x461e2c

// block 00461e12
00461e12  mov        ecx, dword ptr [esi]
00461e14  mov        dword ptr [eax + 0x430], ecx
00461e1a  mov        edx, dword ptr [esi + 4]
00461e1d  mov        dword ptr [eax + 0x434], edx
00461e23  mov        ecx, dword ptr [esi + 8]
00461e26  mov        dword ptr [eax + 0x438], ecx

// block 00461e2c
00461e2c  ret        
