
// block 0044c920
0044c920  push       esi
0044c921  push       0x2000
0044c926  xor        esi, esi
0044c928  push       esi
0044c929  push       0x4cc550
0044c92e  call       0x477420

// block 0044c933
0044c933  add        esp, 0xc
0044c936  mov        eax, 0x4b4544
0044c93b  jmp        0x44c940

// block 0044c940
0044c940  mov        dword ptr [eax - 4], esi
0044c943  mov        dword ptr [eax], esi
0044c945  mov        dword ptr [eax + 4], esi
0044c948  add        eax, 0xc
0044c94b  cmp        eax, 0x4cc550
0044c950  jl         0x44c940

// block 0044c952
0044c952  pop        esi
0044c953  ret        
