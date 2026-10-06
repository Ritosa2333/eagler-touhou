
// block 00449fa0
00449fa0  push       ebp
00449fa1  push       esi
00449fa2  push       0x24
00449fa4  call       0x46c9ea

// block 00449fa9
00449fa9  xor        ebp, ebp
00449fab  add        esp, 4
00449fae  cmp        eax, ebp
00449fb0  je         0x449fce

// block 00449fb2
00449fb2  and        dword ptr [eax + 4], 0xfffffffe
00449fb6  mov        dword ptr [eax + 8], ebp
00449fb9  mov        dword ptr [eax + 0xc], ebp
00449fbc  mov        dword ptr [eax + 0x10], ebp
00449fbf  mov        dword ptr [eax], ebp
00449fc1  mov        dword ptr [eax + 0x14], eax
00449fc4  mov        dword ptr [eax + 0x18], ebp
00449fc7  mov        dword ptr [eax + 0x1c], ebp
00449fca  mov        esi, eax
00449fcc  jmp        0x449fd0

// block 00449fce
00449fce  xor        esi, esi

// block 00449fd0
00449fd0  mov        eax, dword ptr [esi + 4]
00449fd3  and        eax, 0xfffffffd
00449fd6  push       ebx
00449fd7  or         eax, 1
00449fda  mov        ebx, 0x13
00449fdf  mov        dword ptr [esi + 8], 0x44a860
00449fe6  mov        dword ptr [esi + 0xc], ebp
00449fe9  mov        dword ptr [esi + 0x10], ebp
00449fec  mov        dword ptr [esi + 0x20], edi
00449fef  mov        dword ptr [esi + 4], eax
00449ff2  call       0x462380

// block 00449ff7
00449ff7  push       0x24
00449ff9  mov        dword ptr [edi + 8], esi
00449ffc  call       0x46c9ea

// block 0044a001
0044a001  add        esp, 4
0044a004  cmp        eax, ebp
0044a006  je         0x44a024

// block 0044a008
0044a008  and        dword ptr [eax + 4], 0xfffffffe
0044a00c  mov        dword ptr [eax + 8], ebp
0044a00f  mov        dword ptr [eax + 0xc], ebp
0044a012  mov        dword ptr [eax + 0x10], ebp
0044a015  mov        dword ptr [eax], ebp
0044a017  mov        dword ptr [eax + 0x14], eax
0044a01a  mov        dword ptr [eax + 0x18], ebp
0044a01d  mov        dword ptr [eax + 0x1c], ebp
0044a020  mov        esi, eax
0044a022  jmp        0x44a026

// block 0044a024
0044a024  xor        esi, esi

// block 0044a026
0044a026  mov        ecx, dword ptr [esi + 4]
0044a029  and        ecx, 0xfffffffd
0044a02c  or         ecx, 1
0044a02f  mov        ebx, 0x3c
0044a034  mov        dword ptr [esi + 8], 0x44a870
0044a03b  mov        dword ptr [esi + 0xc], ebp
0044a03e  mov        dword ptr [esi + 0x10], ebp
0044a041  mov        dword ptr [esi + 0x20], edi
0044a044  mov        dword ptr [esi + 4], ecx
0044a047  call       0x462420

// block 0044a04c
0044a04c  push       0x24
0044a04e  mov        dword ptr [edi + 0xc], esi
0044a051  call       0x46c9ea

// block 0044a056
0044a056  add        esp, 4
0044a059  cmp        eax, ebp
0044a05b  je         0x44a079

// block 0044a05d
0044a05d  and        dword ptr [eax + 4], 0xfffffffe
0044a061  mov        dword ptr [eax + 8], ebp
0044a064  mov        dword ptr [eax + 0xc], ebp
0044a067  mov        dword ptr [eax + 0x10], ebp
0044a06a  mov        dword ptr [eax], ebp
0044a06c  mov        dword ptr [eax + 0x14], eax
0044a06f  mov        dword ptr [eax + 0x18], ebp
0044a072  mov        dword ptr [eax + 0x1c], ebp
0044a075  mov        esi, eax
0044a077  jmp        0x44a07b

// block 0044a079
0044a079  xor        esi, esi

// block 0044a07b
0044a07b  mov        edx, dword ptr [esi + 4]
0044a07e  and        edx, 0xfffffffd
0044a081  or         edx, 1
0044a084  mov        ebx, 0x16
0044a089  mov        dword ptr [esi + 8], 0x44a880
0044a090  mov        dword ptr [esi + 0xc], ebp
0044a093  mov        dword ptr [esi + 0x10], ebp
0044a096  mov        dword ptr [esi + 0x20], edi
0044a099  mov        dword ptr [esi + 4], edx
0044a09c  call       0x462420

// block 0044a0a1
0044a0a1  fldz       
0044a0a3  mov        dword ptr [edi + 0x24], esi
0044a0a6  mov        eax, dword ptr [edi + 0x20]
0044a0a9  pop        ebx
0044a0aa  test       al, 1
0044a0ac  jne        0x44a0c8

// block 0044a0ae
0044a0ae  or         eax, 1
0044a0b1  fst        dword ptr [edi + 0x18]
0044a0b4  mov        dword ptr [edi + 0x14], ebp
0044a0b7  mov        dword ptr [edi + 0x10], 0xfff0bdc1
0044a0be  mov        dword ptr [edi + 0x1c], 0x4b2ed0
0044a0c5  mov        dword ptr [edi + 0x20], eax

// block 0044a0c8
0044a0c8  pop        esi
0044a0c9  fstp       dword ptr [edi + 0x18]
0044a0cc  mov        dword ptr [edi + 0x14], ebp
0044a0cf  mov        dword ptr [edi + 0x10], 0xffffffff
0044a0d6  xor        eax, eax
0044a0d8  pop        ebp
0044a0d9  ret        
