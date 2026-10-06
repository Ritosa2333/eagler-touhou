
// block 004588f0
004588f0  push       ebx
004588f1  mov        ebx, eax
004588f3  push       esi
004588f4  mov        esi, ecx
004588f6  test       ebx, ebx
004588f8  je         0x45895b

// block 004588fa
004588fa  mov        ecx, dword ptr [0x4ce8cc]

// block 00458900
00458900  mov        eax, dword ptr [esi + 0x3f4]
00458906  test       eax, eax
00458908  je         0x458950

// block 0045890a
0045890a  mov        eax, dword ptr [eax]
0045890c  test       eax, eax
0045890e  jl         0x458950

// block 00458910
00458910  cmp        dword ptr [ecx + eax*4 + 0x4b50c0], 0
00458918  je         0x458950

// block 0045891a
0045891a  mov        eax, dword ptr [ecx + eax*4 + 0x4b50c0]
00458921  xor        edx, edx
00458923  cmp        dword ptr [eax + 0x120], edx
00458929  setne      dl
0045892c  mov        eax, edx
0045892e  test       eax, eax
00458930  je         0x458950

// block 00458932
00458932  mov        eax, dword ptr [esi + 0x494]
00458938  test       eax, eax
0045893a  je         0x458949

// block 0045893c
0045893c  movsx      edx, di
0045893f  mov        ecx, esi
00458941  call       eax

// block 00458943
00458943  mov        ecx, dword ptr [0x4ce8cc]

// block 00458949
00458949  mov        word ptr [esi + 0x3c4], di

// block 00458950
00458950  add        esi, 0x4b4
00458956  sub        ebx, 1
00458959  jne        0x458900

// block 0045895b
0045895b  pop        esi
0045895c  pop        ebx
0045895d  ret        
