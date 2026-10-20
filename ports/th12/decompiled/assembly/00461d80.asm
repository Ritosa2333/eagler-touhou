
// block 00461d80
00461d80  mov        ecx, dword ptr [eax]
00461d82  mov        edx, dword ptr [0x4ce8cc]
00461d88  push       ecx
00461d89  call       0x461920

// block 00461d8e
00461d8e  test       eax, eax
00461d90  je         0x461dbf

// block 00461d92
00461d92  mov        edx, 0xfffffffd
00461d97  and        dword ptr [eax + 0x47c], edx
00461d9d  cmp        dword ptr [eax + 0x18], 0
00461da1  jne        0x461dbf

// block 00461da3
00461da3  mov        eax, dword ptr [eax + 0x14]
00461da6  test       eax, eax
00461da8  je         0x461dbf

// block 00461daa
00461daa  lea        ebx, [ebx]

// block 00461db0
00461db0  mov        ecx, dword ptr [eax]
00461db2  and        dword ptr [ecx + 0x47c], edx
00461db8  mov        eax, dword ptr [eax + 4]
00461dbb  test       eax, eax
00461dbd  jne        0x461db0

// block 00461dbf
00461dbf  ret        
