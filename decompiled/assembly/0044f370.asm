
// block 0044f370
0044f370  push       ecx
0044f371  push       ebx
0044f372  push       ebp
0044f373  push       esi
0044f374  push       edi
0044f375  mov        edi, dword ptr [0x4ce8cc]
0044f37b  add        edi, 0x4b50c0
0044f381  mov        dword ptr [esp + 0x10], 0x20
0044f389  mov        ebx, 1
0044f38e  mov        edi, edi

// block 0044f390
0044f390  mov        eax, dword ptr [edi]
0044f392  test       eax, eax
0044f394  je         0x44f3e8

// block 0044f396
0044f396  xor        ebp, ebp
0044f398  cmp        dword ptr [eax + 0x10c], ebp
0044f39e  jle        0x44f3e8

// block 0044f3a0
0044f3a0  xor        esi, esi

// block 0044f3a2
0044f3a2  mov        ecx, dword ptr [eax + 0x120]
0044f3a8  add        ecx, esi
0044f3aa  test       byte ptr [ecx + 0x10], bl
0044f3ad  je         0x44f3d9

// block 0044f3af
0044f3af  cmp        dword ptr [ecx], 0
0044f3b2  je         0x44f3d9

// block 0044f3b4
0044f3b4  mov        edx, dword ptr [eax + 0x120]
0044f3ba  mov        ecx, dword ptr [edx + esi]
0044f3bd  lea        eax, [edx + esi]
0044f3c0  mov        edx, dword ptr [ecx]
0044f3c2  mov        eax, ecx
0044f3c4  mov        ecx, dword ptr [edx + 8]
0044f3c7  push       eax
0044f3c8  call       ecx

// block 0044f3ca
0044f3ca  mov        edx, dword ptr [edi]
0044f3cc  mov        eax, dword ptr [edx + 0x120]
0044f3d2  mov        dword ptr [esi + eax], 0

// block 0044f3d9
0044f3d9  mov        eax, dword ptr [edi]
0044f3db  add        ebp, ebx
0044f3dd  add        esi, 0x14
0044f3e0  cmp        ebp, dword ptr [eax + 0x10c]
0044f3e6  jl         0x44f3a2

// block 0044f3e8
0044f3e8  add        edi, 4
0044f3eb  sub        dword ptr [esp + 0x10], ebx
0044f3ef  jne        0x44f390

// block 0044f3f1
0044f3f1  pop        edi
0044f3f2  pop        esi
0044f3f3  pop        ebp
0044f3f4  pop        ebx
0044f3f5  pop        ecx
0044f3f6  ret        
