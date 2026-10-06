
// block 0046a300
0046a300  push       ebp
0046a301  mov        ebp, esp
0046a303  and        esp, 0xfffffff8
0046a306  sub        esp, 0x20
0046a309  push       esi
0046a30a  push       edi
0046a30b  mov        edi, dword ptr [ecx + 0x478]
0046a311  mov        eax, dword ptr [edi + 0x10]
0046a314  cmp        eax, 0x12c
0046a319  jge        0x46a465

// block 0046a31f
0046a31f  mov        edx, dword ptr [ecx + 0x430]
0046a325  mov        dword ptr [edi], edx
0046a327  mov        edx, dword ptr [ecx + 0x434]
0046a32d  mov        dword ptr [edi + 4], edx
0046a330  mov        ecx, dword ptr [ecx + 0x438]
0046a336  lea        esi, [edi + 0xc]
0046a339  mov        dword ptr [edi + 8], ecx
0046a33c  mov        edx, dword ptr [esi + 4]
0046a33f  cmp        edx, dword ptr [esi]
0046a341  je         0x46a458

// block 0046a347
0046a347  and        eax, 0x80000003
0046a34c  jns        0x46a353

// block 0046a34e
0046a34e  dec        eax
0046a34f  or         eax, 0xfffffffc
0046a352  inc        eax

// block 0046a353
0046a353  jne        0x46a458

// block 0046a359
0046a359  fld        dword ptr [edi]
0046a35b  push       ecx
0046a35c  mov        eax, 0x1c
0046a361  fstp       dword ptr [esp]
0046a364  call       0x453e20

// block 0046a369
0046a369  mov        eax, dword ptr [edi]
0046a36b  mov        edx, dword ptr [edi + 8]
0046a36e  mov        dword ptr [esp + 0x10], eax
0046a372  fld        dword ptr [esp + 0x10]
0046a376  fadd       dword ptr [edi + 0x24]
0046a379  mov        ecx, dword ptr [edi + 4]
0046a37c  mov        dword ptr [esp + 0x18], edx
0046a380  mov        dword ptr [esp + 0x1c], eax
0046a384  fstp       dword ptr [esp + 0x10]
0046a388  mov        dword ptr [esp + 0x14], ecx
0046a38c  fld        dword ptr [0x4a4060]
0046a392  mov        dword ptr [esp + 0x20], ecx
0046a396  fstp       dword ptr [esp + 0x18]
0046a39a  mov        dword ptr [esp + 0x24], edx
0046a39e  fld        dword ptr [edi + 0x2c]
0046a3a1  fadd       dword ptr [edi + 0x24]
0046a3a4  fstp       dword ptr [esp + 0xc]
0046a3a8  fld        dword ptr [esp + 0xc]
0046a3ac  fst        dword ptr [edi + 0x24]
0046a3af  fabs       
0046a3b1  fstp       dword ptr [esp + 0xc]
0046a3b5  fld        dword ptr [esp + 0xc]
0046a3b9  fcomp      dword ptr [0x4a3e00]
0046a3bf  fnstsw     ax
0046a3c1  test       ah, 1
0046a3c4  jne        0x46a3d2

// block 0046a3c6
0046a3c6  fld        dword ptr [edi + 0x2c]
0046a3c9  fmul       qword ptr [0x4a4168]
0046a3cf  fstp       dword ptr [edi + 0x2c]

// block 0046a3d2
0046a3d2  lea        eax, [esp + 0x10]
0046a3d6  push       eax
0046a3d7  push       3
0046a3d9  lea        ecx, [esp + 0x14]
0046a3dd  push       ecx
0046a3de  call       0x40fbe0

// block 0046a3e3
0046a3e3  fld        dword ptr [edi + 0x20]
0046a3e6  mov        edx, dword ptr [edi]
0046a3e8  mov        ecx, dword ptr [edi + 8]
0046a3eb  mov        eax, dword ptr [edi + 4]
0046a3ee  mov        dword ptr [esp + 0x10], edx
0046a3f2  fadd       dword ptr [esp + 0x10]
0046a3f6  mov        dword ptr [esp + 0x18], ecx
0046a3fa  mov        dword ptr [esp + 0x14], eax
0046a3fe  fstp       dword ptr [esp + 0x10]
0046a402  mov        dword ptr [esp + 0x20], eax
0046a406  fld        dword ptr [0x4a4060]
0046a40c  mov        dword ptr [esp + 0x1c], edx
0046a410  fstp       dword ptr [esp + 0x18]
0046a414  mov        dword ptr [esp + 0x24], ecx
0046a418  fld        dword ptr [edi + 0x28]
0046a41b  fadd       dword ptr [edi + 0x20]
0046a41e  fstp       dword ptr [edi + 0x20]
0046a421  fld        dword ptr [edi + 0x24]
0046a424  fabs       
0046a426  fstp       dword ptr [esp + 0xc]
0046a42a  fld        dword ptr [esp + 0xc]
0046a42e  fcomp      dword ptr [0x4a3e00]
0046a434  fnstsw     ax
0046a436  test       ah, 1
0046a439  jne        0x46a447

// block 0046a43b
0046a43b  fld        dword ptr [edi + 0x28]
0046a43e  fmul       qword ptr [0x4a4168]
0046a444  fstp       dword ptr [edi + 0x28]

// block 0046a447
0046a447  lea        edx, [esp + 0x10]
0046a44b  push       edx
0046a44c  push       3
0046a44e  lea        eax, [esp + 0x14]
0046a452  push       eax
0046a453  call       0x40fbe0

// block 0046a458
0046a458  call       0x464a80

// block 0046a45d
0046a45d  xor        eax, eax
0046a45f  pop        edi
0046a460  pop        esi
0046a461  mov        esp, ebp
0046a463  pop        ebp
0046a464  ret        

// block 0046a465
0046a465  pop        edi
0046a466  mov        eax, 1
0046a46b  pop        esi
0046a46c  mov        esp, ebp
0046a46e  pop        ebp
0046a46f  ret        
