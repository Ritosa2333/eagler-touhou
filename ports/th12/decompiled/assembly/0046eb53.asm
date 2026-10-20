
// block 0046eb53
0046eb53  push       ebx
0046eb54  mov        ebx, dword ptr [0x498094]
0046eb5a  push       esi
0046eb5b  mov        esi, 0x4ad2b8
0046eb60  push       edi

// block 0046eb61
0046eb61  mov        edi, dword ptr [esi]
0046eb63  test       edi, edi
0046eb65  je         0x46eb7a

// block 0046eb67
0046eb67  cmp        dword ptr [esi + 4], 1
0046eb6b  je         0x46eb7a

// block 0046eb6d
0046eb6d  push       edi
0046eb6e  call       ebx

// block 0046eb70
0046eb70  push       edi
0046eb71  call       0x46c941

// block 0046eb76
0046eb76  and        dword ptr [esi], 0
0046eb79  pop        ecx

// block 0046eb7a
0046eb7a  add        esi, 8
0046eb7d  cmp        esi, 0x4ad3d8
0046eb83  jl         0x46eb61

// block 0046eb85
0046eb85  mov        esi, 0x4ad2b8
0046eb8a  pop        edi

// block 0046eb8b
0046eb8b  mov        eax, dword ptr [esi]
0046eb8d  test       eax, eax
0046eb8f  je         0x46eb9a

// block 0046eb91
0046eb91  cmp        dword ptr [esi + 4], 1
0046eb95  jne        0x46eb9a

// block 0046eb97
0046eb97  push       eax
0046eb98  call       ebx

// block 0046eb9a
0046eb9a  add        esi, 8
0046eb9d  cmp        esi, 0x4ad3d8
0046eba3  jl         0x46eb8b

// block 0046eba5
0046eba5  pop        esi
0046eba6  pop        ebx
0046eba7  ret        
