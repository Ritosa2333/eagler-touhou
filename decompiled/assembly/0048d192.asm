
// block 0048d192
0048d192  mov        edi, edi
0048d194  push       ebp
0048d195  mov        ebp, esp
0048d197  push       esi
0048d198  mov        esi, dword ptr [ebp + 8]
0048d19b  test       esi, esi
0048d19d  jne        0x48d1a8

// block 0048d19f
0048d19f  push       esi
0048d1a0  call       0x48d1da

// block 0048d1a5
0048d1a5  pop        ecx
0048d1a6  jmp        0x48d1d7

// block 0048d1a8
0048d1a8  push       esi
0048d1a9  call       0x48d12a

// block 0048d1ae
0048d1ae  pop        ecx
0048d1af  test       eax, eax
0048d1b1  je         0x48d1b8

// block 0048d1b3
0048d1b3  or         eax, 0xffffffff
0048d1b6  jmp        0x48d1d7

// block 0048d1b8
0048d1b8  test       dword ptr [esi + 0xc], 0x4000
0048d1bf  je         0x48d1d5

// block 0048d1c1
0048d1c1  push       esi
0048d1c2  call       0x47c1da

// block 0048d1c7
0048d1c7  push       eax
0048d1c8  call       0x48ec6e

// block 0048d1cd
0048d1cd  pop        ecx
0048d1ce  neg        eax
0048d1d0  pop        ecx
0048d1d1  sbb        eax, eax
0048d1d3  jmp        0x48d1d7

// block 0048d1d5
0048d1d5  xor        eax, eax

// block 0048d1d7
0048d1d7  pop        esi
0048d1d8  pop        ebp
0048d1d9  ret        
