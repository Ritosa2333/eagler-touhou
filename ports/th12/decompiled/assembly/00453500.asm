
// block 00453500
00453500  mov        eax, dword ptr [0x4d0e6c]
00453505  push       esi
00453506  push       edi
00453507  xor        edi, edi
00453509  cmp        eax, edi
0045350b  je         0x45351c

// block 0045350d
0045350d  push       eax
0045350e  call       0x46c941

// block 00453513
00453513  add        esp, 4
00453516  mov        dword ptr [0x4d0e6c], edi

// block 0045351c
0045351c  mov        esi, 0x4d0e70

// block 00453521
00453521  mov        eax, dword ptr [esi]
00453523  cmp        eax, edi
00453525  je         0x453531

// block 00453527
00453527  mov        ecx, dword ptr [eax]
00453529  mov        edx, dword ptr [ecx + 8]
0045352c  push       eax
0045352d  call       edx

// block 0045352f
0045352f  mov        dword ptr [esi], edi

// block 00453531
00453531  add        esi, 0x18
00453534  cmp        esi, 0x4d1410
0045353a  jl         0x453521

// block 0045353c
0045353c  mov        esi, 0x4d1410

// block 00453541
00453541  mov        eax, dword ptr [esi]
00453543  cmp        eax, edi
00453545  je         0x453552

// block 00453547
00453547  push       eax
00453548  call       0x46c941

// block 0045354d
0045354d  add        esp, 4
00453550  mov        dword ptr [esi], edi

// block 00453552
00453552  add        esi, 4
00453555  cmp        esi, 0x4d14d4
0045355b  jl         0x453541

// block 0045355d
0045355d  cmp        dword ptr [0x4cf4f8], edi
00453563  je         0x45360e

// block 00453569
00453569  mov        eax, dword ptr [0x4cf4f4]
0045356e  push       1
00453570  push       eax
00453571  call       dword ptr [0x498280]

// block 00453577
00453577  mov        esi, 0x4cf4e8
0045357c  call       0x453c30

// block 00453581
00453581  mov        eax, dword ptr [0x4cf4f0]
00453586  mov        dword ptr [0x4cf4e8], edi
0045358c  mov        ecx, dword ptr [eax]
0045358e  mov        edx, dword ptr [ecx + 0x48]
00453591  push       eax
00453592  call       edx

// block 00453594
00453594  mov        eax, dword ptr [0x4cf4f0]
00453599  cmp        eax, edi
0045359b  je         0x4535ab

// block 0045359d
0045359d  mov        ecx, dword ptr [eax]
0045359f  mov        edx, dword ptr [ecx + 8]
004535a2  push       eax
004535a3  call       edx

// block 004535a5
004535a5  mov        dword ptr [0x4cf4f0], edi

// block 004535ab
004535ab  mov        ecx, dword ptr [0x4d4754]
004535b1  cmp        ecx, edi
004535b3  je         0x4535c3

// block 004535b5
004535b5  mov        eax, dword ptr [ecx]
004535b7  mov        edx, dword ptr [eax]
004535b9  push       1
004535bb  call       edx

// block 004535bd
004535bd  mov        dword ptr [0x4d4754], edi

// block 004535c3
004535c3  mov        eax, dword ptr [0x4cf4f8]
004535c8  cmp        eax, edi
004535ca  je         0x4535ed

// block 004535cc
004535cc  mov        esi, eax
004535ce  mov        eax, dword ptr [eax]
004535d0  cmp        eax, edi
004535d2  je         0x4535de

// block 004535d4
004535d4  mov        ecx, dword ptr [eax]
004535d6  mov        edx, dword ptr [ecx + 8]
004535d9  push       eax
004535da  call       edx

// block 004535dc
004535dc  mov        dword ptr [esi], edi

// block 004535de
004535de  push       esi
004535df  call       0x46ca4f

// block 004535e4
004535e4  add        esp, 4
004535e7  mov        dword ptr [0x4cf4f8], edi

// block 004535ed
004535ed  mov        esi, 0x4d0da8

// block 004535f2
004535f2  mov        eax, dword ptr [esi]
004535f4  cmp        eax, edi
004535f6  je         0x453603

// block 004535f8
004535f8  push       eax
004535f9  call       0x46c941

// block 004535fe
004535fe  add        esp, 4
00453601  mov        dword ptr [esi], edi

// block 00453603
00453603  add        esi, 4
00453606  cmp        esi, 0x4d0de8
0045360c  jl         0x4535f2

// block 0045360e
0045360e  pop        edi
0045360f  xor        eax, eax
00453611  pop        esi
00453612  ret        
