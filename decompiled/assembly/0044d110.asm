
// block 0044d110
0044d110  push       esi
0044d111  mov        esi, ecx
0044d113  mov        dword ptr [esi], 0x4a3738
0044d119  call       0x464c40

// block 0044d11e
0044d11e  test       byte ptr [esp + 8], 1
0044d123  je         0x44d12e

// block 0044d125
0044d125  push       esi
0044d126  call       0x46ca4f

// block 0044d12b
0044d12b  add        esp, 4

// block 0044d12e
0044d12e  mov        eax, esi
0044d130  pop        esi
0044d131  ret        4
