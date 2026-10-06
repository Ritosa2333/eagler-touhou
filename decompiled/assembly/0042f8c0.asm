
// block 0042f8c0
0042f8c0  push       ebx
0042f8c1  push       ebp
0042f8c2  push       esi
0042f8c3  push       edi
0042f8c4  xor        edi, edi
0042f8c6  mov        esi, 0xfffffffe
0042f8cb  push       0x24
0042f8cd  mov        dword ptr [0x4cee3c], esi
0042f8d3  mov        dword ptr [0x4cee40], edi
0042f8d9  mov        dword ptr [0x4cee48], edi
0042f8df  call       0x46c9ea

// block 0042f8e4
0042f8e4  add        esp, 4
0042f8e7  cmp        eax, edi
0042f8e9  je         0x42f904

// block 0042f8eb
0042f8eb  and        dword ptr [eax + 4], esi
0042f8ee  mov        dword ptr [eax + 8], edi
0042f8f1  mov        dword ptr [eax + 0xc], edi
0042f8f4  mov        dword ptr [eax + 0x10], edi
0042f8f7  mov        dword ptr [eax], edi
0042f8f9  mov        dword ptr [eax + 0x14], eax
0042f8fc  mov        dword ptr [eax + 0x18], edi
0042f8ff  mov        dword ptr [eax + 0x1c], edi
0042f902  jmp        0x42f906

// block 0042f904
0042f904  xor        eax, eax

// block 0042f906
0042f906  mov        ebp, 3
0042f90b  or         dword ptr [eax + 4], ebp
0042f90e  lea        ebx, [ebp - 2]
0042f911  mov        esi, eax
0042f913  mov        dword ptr [eax + 8], 0x42f000
0042f91a  mov        dword ptr [eax + 0x10], edi
0042f91d  mov        dword ptr [eax + 0x20], 0x4ce8e8
0042f924  mov        dword ptr [eax + 0xc], 0x42f560
0042f92b  call       0x462380

// block 0042f930
0042f930  cmp        eax, edi
0042f932  jne        0x42fb9d

// block 0042f938
0042f938  push       0x24
0042f93a  call       0x46c9ea

// block 0042f93f
0042f93f  add        esp, 4
0042f942  cmp        eax, edi
0042f944  je         0x42f962

// block 0042f946
0042f946  and        dword ptr [eax + 4], 0xfffffffe
0042f94a  mov        dword ptr [eax + 8], edi
0042f94d  mov        dword ptr [eax + 0xc], edi
0042f950  mov        dword ptr [eax + 0x10], edi
0042f953  mov        dword ptr [eax], edi
0042f955  mov        dword ptr [eax + 0x14], eax
0042f958  mov        dword ptr [eax + 0x18], edi
0042f95b  mov        dword ptr [eax + 0x1c], edi
0042f95e  mov        esi, eax
0042f960  jmp        0x42f964

// block 0042f962
0042f962  xor        esi, esi

// block 0042f964
0042f964  or         dword ptr [esi + 4], ebp
0042f967  mov        dword ptr [esi + 8], 0x42f0b0
0042f96e  mov        dword ptr [esi + 0xc], edi
0042f971  mov        dword ptr [esi + 0x10], edi
0042f974  mov        dword ptr [esi + 0x20], 0x4ce8e8
0042f97b  call       0x462420

// block 0042f980
0042f980  push       0x24
0042f982  call       0x46c9ea

// block 0042f987
0042f987  add        esp, 4
0042f98a  cmp        eax, edi
0042f98c  je         0x42f9a8

// block 0042f98e
0042f98e  and        dword ptr [eax + 4], 0xfffffffe
0042f992  mov        dword ptr [eax + 8], edi
0042f995  mov        dword ptr [eax + 0xc], edi
0042f998  mov        dword ptr [eax + 0x10], edi
0042f99b  mov        dword ptr [eax], edi
0042f99d  mov        dword ptr [eax + 0x14], eax
0042f9a0  mov        dword ptr [eax + 0x18], edi
0042f9a3  mov        dword ptr [eax + 0x1c], edi
0042f9a6  jmp        0x42f9aa

// block 0042f9a8
0042f9a8  xor        eax, eax

// block 0042f9aa
0042f9aa  or         dword ptr [eax + 4], ebp
0042f9ad  mov        ebx, 0xc
0042f9b2  mov        esi, eax
0042f9b4  mov        dword ptr [eax + 8], 0x42f200
0042f9bb  mov        dword ptr [eax + 0xc], edi
0042f9be  mov        dword ptr [eax + 0x10], edi
0042f9c1  mov        dword ptr [eax + 0x20], 0x4ce8e8
0042f9c8  call       0x462420

// block 0042f9cd
0042f9cd  push       0x24
0042f9cf  call       0x46c9ea

// block 0042f9d4
0042f9d4  add        esp, 4
0042f9d7  cmp        eax, edi
0042f9d9  je         0x42f9f5

// block 0042f9db
0042f9db  and        dword ptr [eax + 4], 0xfffffffe
0042f9df  mov        dword ptr [eax + 8], edi
0042f9e2  mov        dword ptr [eax + 0xc], edi
0042f9e5  mov        dword ptr [eax + 0x10], edi
0042f9e8  mov        dword ptr [eax], edi
0042f9ea  mov        dword ptr [eax + 0x14], eax
0042f9ed  mov        dword ptr [eax + 0x18], edi
0042f9f0  mov        dword ptr [eax + 0x1c], edi
0042f9f3  jmp        0x42f9f7

// block 0042f9f5
0042f9f5  xor        eax, eax

// block 0042f9f7
0042f9f7  or         dword ptr [eax + 4], ebp
0042f9fa  mov        ebx, 0xe
0042f9ff  mov        esi, eax
0042fa01  mov        dword ptr [eax + 8], 0x42f3b0
0042fa08  mov        dword ptr [eax + 0xc], edi
0042fa0b  mov        dword ptr [eax + 0x10], edi
0042fa0e  mov        dword ptr [eax + 0x20], 0x4ce8e8
0042fa15  call       0x462420

// block 0042fa1a
0042fa1a  push       0x24
0042fa1c  call       0x46c9ea

// block 0042fa21
0042fa21  add        esp, 4
0042fa24  cmp        eax, edi
0042fa26  je         0x42fa42

// block 0042fa28
0042fa28  and        dword ptr [eax + 4], 0xfffffffe
0042fa2c  mov        dword ptr [eax + 8], edi
0042fa2f  mov        dword ptr [eax + 0xc], edi
0042fa32  mov        dword ptr [eax + 0x10], edi
0042fa35  mov        dword ptr [eax], edi
0042fa37  mov        dword ptr [eax + 0x14], eax
0042fa3a  mov        dword ptr [eax + 0x18], edi
0042fa3d  mov        dword ptr [eax + 0x1c], edi
0042fa40  jmp        0x42fa44

// block 0042fa42
0042fa42  xor        eax, eax

// block 0042fa44
0042fa44  or         dword ptr [eax + 4], ebp
0042fa47  mov        ebx, 0x25
0042fa4c  mov        esi, eax
0042fa4e  mov        dword ptr [eax + 8], 0x42f290
0042fa55  mov        dword ptr [eax + 0xc], edi
0042fa58  mov        dword ptr [eax + 0x10], edi
0042fa5b  mov        dword ptr [eax + 0x20], 0x4ce8e8
0042fa62  call       0x462420

// block 0042fa67
0042fa67  push       0x24
0042fa69  call       0x46c9ea

// block 0042fa6e
0042fa6e  add        esp, 4
0042fa71  cmp        eax, edi
0042fa73  je         0x42fa8f

// block 0042fa75
0042fa75  and        dword ptr [eax + 4], 0xfffffffe
0042fa79  mov        dword ptr [eax + 8], edi
0042fa7c  mov        dword ptr [eax + 0xc], edi
0042fa7f  mov        dword ptr [eax + 0x10], edi
0042fa82  mov        dword ptr [eax], edi
0042fa84  mov        dword ptr [eax + 0x14], eax
0042fa87  mov        dword ptr [eax + 0x18], edi
0042fa8a  mov        dword ptr [eax + 0x1c], edi
0042fa8d  jmp        0x42fa91

// block 0042fa8f
0042fa8f  xor        eax, eax

// block 0042fa91
0042fa91  or         dword ptr [eax + 4], ebp
0042fa94  mov        ebx, 0x27
0042fa99  mov        esi, eax
0042fa9b  mov        dword ptr [eax + 8], 0x42f410
0042faa2  mov        dword ptr [eax + 0xc], edi
0042faa5  mov        dword ptr [eax + 0x10], edi
0042faa8  mov        dword ptr [eax + 0x20], 0x4ce8e8
0042faaf  call       0x462420

// block 0042fab4
0042fab4  push       0x24
0042fab6  call       0x46c9ea

// block 0042fabb
0042fabb  add        esp, 4
0042fabe  cmp        eax, edi
0042fac0  je         0x42fadc

// block 0042fac2
0042fac2  and        dword ptr [eax + 4], 0xfffffffe
0042fac6  mov        dword ptr [eax + 8], edi
0042fac9  mov        dword ptr [eax + 0xc], edi
0042facc  mov        dword ptr [eax + 0x10], edi
0042facf  mov        dword ptr [eax], edi
0042fad1  mov        dword ptr [eax + 0x14], eax
0042fad4  mov        dword ptr [eax + 0x18], edi
0042fad7  mov        dword ptr [eax + 0x1c], edi
0042fada  jmp        0x42fade

// block 0042fadc
0042fadc  xor        eax, eax

// block 0042fade
0042fade  or         dword ptr [eax + 4], ebp
0042fae1  mov        ebx, 0x30
0042fae6  mov        esi, eax
0042fae8  mov        dword ptr [eax + 8], 0x42f320
0042faef  mov        dword ptr [eax + 0xc], edi
0042faf2  mov        dword ptr [eax + 0x10], edi
0042faf5  mov        dword ptr [eax + 0x20], 0x4ce8e8
0042fafc  call       0x462420

// block 0042fb01
0042fb01  push       0x24
0042fb03  call       0x46c9ea

// block 0042fb08
0042fb08  add        esp, 4
0042fb0b  cmp        eax, edi
0042fb0d  je         0x42fb29

// block 0042fb0f
0042fb0f  and        dword ptr [eax + 4], 0xfffffffe
0042fb13  mov        dword ptr [eax + 8], edi
0042fb16  mov        dword ptr [eax + 0xc], edi
0042fb19  mov        dword ptr [eax + 0x10], edi
0042fb1c  mov        dword ptr [eax], edi
0042fb1e  mov        dword ptr [eax + 0x14], eax
0042fb21  mov        dword ptr [eax + 0x18], edi
0042fb24  mov        dword ptr [eax + 0x1c], edi
0042fb27  jmp        0x42fb2b

// block 0042fb29
0042fb29  xor        eax, eax

// block 0042fb2b
0042fb2b  or         dword ptr [eax + 4], ebp
0042fb2e  mov        ebx, 0x31
0042fb33  mov        esi, eax
0042fb35  mov        dword ptr [eax + 8], 0x42f470
0042fb3c  mov        dword ptr [eax + 0xc], edi
0042fb3f  mov        dword ptr [eax + 0x10], edi
0042fb42  mov        dword ptr [eax + 0x20], 0x4ce8e8
0042fb49  call       0x462420

// block 0042fb4e
0042fb4e  push       0x24
0042fb50  call       0x46c9ea

// block 0042fb55
0042fb55  add        esp, 4
0042fb58  cmp        eax, edi
0042fb5a  je         0x42fb76

// block 0042fb5c
0042fb5c  and        dword ptr [eax + 4], 0xfffffffe
0042fb60  mov        dword ptr [eax + 8], edi
0042fb63  mov        dword ptr [eax + 0xc], edi
0042fb66  mov        dword ptr [eax + 0x10], edi
0042fb69  mov        dword ptr [eax], edi
0042fb6b  mov        dword ptr [eax + 0x14], eax
0042fb6e  mov        dword ptr [eax + 0x18], edi
0042fb71  mov        dword ptr [eax + 0x1c], edi
0042fb74  jmp        0x42fb78

// block 0042fb76
0042fb76  xor        eax, eax

// block 0042fb78
0042fb78  or         dword ptr [eax + 4], ebp
0042fb7b  mov        ebx, 0x47
0042fb80  mov        esi, eax
0042fb82  mov        dword ptr [eax + 8], 0x42f380
0042fb89  mov        dword ptr [eax + 0xc], edi
0042fb8c  mov        dword ptr [eax + 0x10], edi
0042fb8f  mov        dword ptr [eax + 0x20], 0x4ce8e8
0042fb96  call       0x462420

// block 0042fb9b
0042fb9b  xor        eax, eax

// block 0042fb9d
0042fb9d  pop        edi
0042fb9e  pop        esi
0042fb9f  pop        ebp
0042fba0  pop        ebx
0042fba1  ret        
