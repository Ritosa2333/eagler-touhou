
// block 0044f400
0044f400  push       ecx
0044f401  push       ebx
0044f402  mov        ebx, dword ptr [0x4ce8cc]
0044f408  push       ebp
0044f409  push       esi
0044f40a  push       edi
0044f40b  add        ebx, 0x4b50c0
0044f411  mov        dword ptr [esp + 0x10], 0x20
0044f419  lea        esp, [esp]

// block 0044f420
0044f420  mov        eax, dword ptr [ebx]
0044f422  test       eax, eax
0044f424  je         0x44f492

// block 0044f426
0044f426  xor        edi, edi
0044f428  cmp        dword ptr [eax + 0x10c], edi
0044f42e  jle        0x44f492

// block 0044f430
0044f430  xor        ebp, ebp

// block 0044f432
0044f432  mov        eax, dword ptr [eax + 0x120]
0044f438  test       byte ptr [eax + ebp + 0x10], 1
0044f43d  lea        esi, [eax + ebp]
0044f440  je         0x44f484

// block 0044f442
0044f442  or         dword ptr [esi + 0x10], 1
0044f446  mov        edx, dword ptr [0x4ce9e4]
0044f44c  mov        eax, dword ptr [0x4ce8f0]
0044f451  mov        ecx, dword ptr [eax]
0044f453  push       0
0044f455  push       esi
0044f456  push       0
0044f458  push       edx
0044f459  mov        edx, dword ptr [0x4ce9e0]
0044f45f  push       1
0044f461  push       1
0044f463  push       edx
0044f464  mov        edx, dword ptr [0x4ce9dc]
0044f46a  push       edx
0044f46b  push       eax
0044f46c  mov        eax, dword ptr [ecx + 0x5c]
0044f46f  call       eax

// block 0044f471
0044f471  xor        ecx, ecx
0044f473  cmp        dword ptr [0x4ce9e4], 0x16
0044f47a  sete       cl
0044f47d  lea        ecx, [ecx + ecx + 2]
0044f481  mov        dword ptr [esi + 0xc], ecx

// block 0044f484
0044f484  mov        eax, dword ptr [ebx]
0044f486  inc        edi
0044f487  add        ebp, 0x14
0044f48a  cmp        edi, dword ptr [eax + 0x10c]
0044f490  jl         0x44f432

// block 0044f492
0044f492  add        ebx, 4
0044f495  sub        dword ptr [esp + 0x10], 1
0044f49a  jne        0x44f420

// block 0044f49c
0044f49c  pop        edi
0044f49d  pop        esi
0044f49e  pop        ebp
0044f49f  pop        ebx
0044f4a0  pop        ecx
0044f4a1  ret        
