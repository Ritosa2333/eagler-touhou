
// block 0047e4f1
0047e4f1  mov        edi, edi
0047e4f3  push       ebp
0047e4f4  mov        ebp, esp
0047e4f6  push       esi
0047e4f7  mov        esi, ecx
0047e4f9  cmp        dword ptr [esi], 0
0047e4fc  je         0x47e507

// block 0047e4fe
0047e4fe  push       3
0047e500  call       0x47e2f7

// block 0047e505
0047e505  jmp        0x47e568

// block 0047e507
0047e507  push       edi
0047e508  mov        edi, dword ptr [ebp + 8]
0047e50b  test       edi, edi
0047e50d  je         0x47e563

// block 0047e50f
0047e50f  mov        eax, dword ptr [ebp + 0xc]
0047e512  test       eax, eax
0047e514  je         0x47e563

// block 0047e516
0047e516  sub        eax, 0
0047e519  je         0x47e55d

// block 0047e51b
0047e51b  dec        eax
0047e51c  push       0
0047e51e  mov        ecx, 0x4b42f8
0047e523  je         0x47e53d

// block 0047e525
0047e525  push       0xc
0047e527  call       0x47dc29

// block 0047e52c
0047e52c  test       eax, eax
0047e52e  je         0x47e555

// block 0047e530
0047e530  push       dword ptr [ebp + 0xc]
0047e533  mov        ecx, eax
0047e535  push       edi
0047e536  call       0x47e3b8

// block 0047e53b
0047e53b  jmp        0x47e557

// block 0047e53d
0047e53d  push       8
0047e53f  call       0x47dc29

// block 0047e544
0047e544  test       eax, eax
0047e546  je         0x47e555

// block 0047e548
0047e548  mov        cl, byte ptr [edi]
0047e54a  mov        dword ptr [eax], 0x49ddbc
0047e550  mov        byte ptr [eax + 4], cl
0047e553  jmp        0x47e557

// block 0047e555
0047e555  xor        eax, eax

// block 0047e557
0047e557  mov        dword ptr [esi], eax
0047e559  test       eax, eax
0047e55b  jne        0x47e567

// block 0047e55d
0047e55d  mov        byte ptr [esi + 4], 3
0047e561  jmp        0x47e567

// block 0047e563
0047e563  mov        byte ptr [esi + 4], 2

// block 0047e567
0047e567  pop        edi

// block 0047e568
0047e568  pop        esi
0047e569  pop        ebp
0047e56a  ret        8
