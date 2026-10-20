
// block 00449d80
00449d80  mov        cl, byte ptr [eax]
00449d82  cmp        cl, 0xa
00449d85  je         0x449d9d

// block 00449d87
00449d87  cmp        cl, 0xd
00449d8a  je         0x449d9d

// block 00449d8c
00449d8c  mov        ecx, dword ptr [edx]
00449d8e  test       ecx, ecx
00449d90  je         0x449dba

// block 00449d92
00449d92  dec        ecx
00449d93  inc        eax
00449d94  mov        dword ptr [edx], ecx
00449d96  mov        cl, byte ptr [eax]
00449d98  cmp        cl, 0xa
00449d9b  jne        0x449d87

// block 00449d9d
00449d9d  cmp        dword ptr [edx], 0
00449da0  je         0x449dba

// block 00449da2
00449da2  mov        cl, byte ptr [eax]
00449da4  cmp        cl, 0xa
00449da7  je         0x449dae

// block 00449da9
00449da9  cmp        cl, 0xd
00449dac  jne        0x449dba

// block 00449dae
00449dae  mov        ecx, dword ptr [edx]
00449db0  test       ecx, ecx
00449db2  je         0x449dba

// block 00449db4
00449db4  inc        eax
00449db5  dec        ecx
00449db6  mov        dword ptr [edx], ecx
00449db8  jmp        0x449da2

// block 00449dba
00449dba  ret        
