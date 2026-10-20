
// block 0048b102
0048b102  push       0x10
0048b104  push       0x4aaf78
0048b109  call       0x46fcec

// block 0048b10e
0048b10e  xor        eax, eax
0048b110  xor        esi, esi
0048b112  cmp        dword ptr [ebp + 8], esi
0048b115  setne      al
0048b118  cmp        eax, esi
0048b11a  jne        0x48b138

// block 0048b11c
0048b11c  call       0x46e96f

// block 0048b121
0048b121  mov        dword ptr [eax], 0x16
0048b127  push       esi
0048b128  push       esi
0048b129  push       esi
0048b12a  push       esi
0048b12b  push       esi
0048b12c  call       0x470f2d

// block 0048b131
0048b131  add        esp, 0x14
0048b134  xor        eax, eax
0048b136  jmp        0x48b176

// block 0048b138
0048b138  mov        edi, 0x7fff
0048b13d  push       edi
0048b13e  push       dword ptr [ebp + 8]
0048b141  call       0x48e41a

// block 0048b146
0048b146  pop        ecx
0048b147  pop        ecx
0048b148  cmp        eax, edi
0048b14a  sbb        eax, eax
0048b14c  neg        eax
0048b14e  je         0x48b11c

// block 0048b150
0048b150  push       7
0048b152  call       0x46ec9a

// block 0048b157
0048b157  pop        ecx
0048b158  mov        dword ptr [ebp - 4], esi
0048b15b  push       dword ptr [ebp + 8]
0048b15e  call       0x48af42

// block 0048b163
0048b163  pop        ecx
0048b164  mov        dword ptr [ebp - 0x1c], eax
0048b167  mov        dword ptr [ebp - 4], 0xfffffffe
0048b16e  call       0x48b17c

// block 0048b173
0048b173  mov        eax, dword ptr [ebp - 0x1c]

// block 0048b176
0048b176  call       0x46fd31

// block 0048b17b
0048b17b  ret        
