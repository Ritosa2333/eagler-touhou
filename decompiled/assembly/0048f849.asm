
// block 0048f849
0048f849  xor        eax, eax
0048f84b  test       bl, 0x10
0048f84e  je         0x48f851

// block 0048f850
0048f850  inc        eax

// block 0048f851
0048f851  test       bl, 8
0048f854  je         0x48f859

// block 0048f856
0048f856  or         eax, 4

// block 0048f859
0048f859  test       bl, 4
0048f85c  je         0x48f861

// block 0048f85e
0048f85e  or         eax, 8

// block 0048f861
0048f861  test       bl, 2
0048f864  je         0x48f869

// block 0048f866
0048f866  or         eax, 0x10

// block 0048f869
0048f869  test       bl, 1
0048f86c  je         0x48f871

// block 0048f86e
0048f86e  or         eax, 0x20

// block 0048f871
0048f871  test       ebx, 0x80000
0048f877  je         0x48f87c

// block 0048f879
0048f879  or         eax, 2

// block 0048f87c
0048f87c  mov        ecx, ebx
0048f87e  mov        edx, 0x300
0048f883  and        ecx, edx
0048f885  push       esi
0048f886  mov        esi, 0x200
0048f88b  je         0x48f8b0

// block 0048f88d
0048f88d  cmp        ecx, 0x100
0048f893  je         0x48f8ab

// block 0048f895
0048f895  cmp        ecx, esi
0048f897  je         0x48f8a4

// block 0048f899
0048f899  cmp        ecx, edx
0048f89b  jne        0x48f8b0

// block 0048f89d
0048f89d  or         eax, 0xc00
0048f8a2  jmp        0x48f8b0

// block 0048f8a4
0048f8a4  or         eax, 0x800
0048f8a9  jmp        0x48f8b0

// block 0048f8ab
0048f8ab  or         eax, 0x400

// block 0048f8b0
0048f8b0  mov        ecx, ebx
0048f8b2  and        ecx, 0x30000
0048f8b8  je         0x48f8c6

// block 0048f8ba
0048f8ba  cmp        ecx, 0x10000
0048f8c0  jne        0x48f8c8

// block 0048f8c2
0048f8c2  or         eax, esi
0048f8c4  jmp        0x48f8c8

// block 0048f8c6
0048f8c6  or         eax, edx

// block 0048f8c8
0048f8c8  pop        esi
0048f8c9  test       ebx, 0x40000
0048f8cf  je         0x48f8d6

// block 0048f8d1
0048f8d1  or         eax, 0x1000

// block 0048f8d6
0048f8d6  ret        
