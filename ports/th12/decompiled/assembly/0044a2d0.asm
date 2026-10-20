
// block 0044a2d0
0044a2d0  push       ebx
0044a2d1  push       ebp
0044a2d2  push       esi
0044a2d3  mov        esi, eax
0044a2d5  xor        ebp, ebp
0044a2d7  cmp        dword ptr [esi + 0x38], ebp
0044a2da  je         0x44a4b2

// block 0044a2e0
0044a2e0  mov        eax, dword ptr [esi + 0x14]
0044a2e3  and        eax, 0x8000000f
0044a2e8  jns        0x44a2ef

// block 0044a2ea
0044a2ea  dec        eax
0044a2eb  or         eax, 0xfffffff0
0044a2ee  inc        eax

// block 0044a2ef
0044a2ef  cmp        eax, 1
0044a2f2  jne        0x44a2f7

// block 0044a2f4
0044a2f4  push       eax
0044a2f5  jmp        0x44a2fd

// block 0044a2f7
0044a2f7  cmp        eax, 9
0044a2fa  jne        0x44a302

// block 0044a2fc
0044a2fc  push       ebp

// block 0044a2fd
0044a2fd  call       0x421690

// block 0044a302
0044a302  mov        ecx, dword ptr [esi + 0x38]
0044a305  cmp        ecx, ebp
0044a307  je         0x44a326

// block 0044a309
0044a309  mov        eax, dword ptr [0x4b43dc]
0044a30e  mov        eax, dword ptr [eax + 0x68]
0044a311  cmp        eax, ebp
0044a313  je         0x44a326

// block 0044a315
0044a315  mov        edx, dword ptr [eax]
0044a317  cmp        dword ptr [edx + 0x27b4], ecx
0044a31d  je         0x44a395

// block 0044a31f
0044a31f  mov        eax, dword ptr [eax + 4]
0044a322  cmp        eax, ebp
0044a324  jne        0x44a315

// block 0044a326
0044a326  mov        dword ptr [esi + 0x38], ebp
0044a329  mov        dword ptr [esi + 0x34], ebp
0044a32c  mov        dword ptr [0x4b0c54], ebp
0044a332  mov        dword ptr [0x4b0c50], ebp
0044a338  mov        dword ptr [0x4b0c4c], ebp
0044a33e  mov        dword ptr [0x4b0c58], ebp
0044a344  mov        dword ptr [0x4b0c5c], ebp
0044a34a  mov        eax, 0x36
0044a34f  mov        dword ptr [esi + 0x2c], ebp
0044a352  call       0x453f10

// block 0044a357
0044a357  mov        eax, dword ptr [0x4b43e4]
0044a35c  push       -1
0044a35e  push       eax
0044a35f  call       0x4214b0

// block 0044a364
0044a364  mov        ecx, dword ptr [esi + 0x3c]
0044a367  push       ecx
0044a368  call       0x461a70

// block 0044a36d
0044a36d  mov        dword ptr [esi + 0x3c], ebp
0044a370  mov        edx, dword ptr [esi + 0x40]
0044a373  push       edx
0044a374  mov        ebx, 1
0044a379  call       0x461970

// block 0044a37e
0044a37e  mov        eax, dword ptr [esi + 0x44]
0044a381  push       eax
0044a382  call       0x461970

// block 0044a387
0044a387  add        esi, 0x10
0044a38a  call       0x464a80

// block 0044a38f
0044a38f  pop        esi
0044a390  pop        ebp
0044a391  mov        eax, ebx
0044a393  pop        ebx
0044a394  ret        

// block 0044a395
0044a395  mov        ecx, dword ptr [0x4b43e4]
0044a39b  cmp        dword ptr [ecx + 0x6d30], ebp
0044a3a1  je         0x44a3af

// block 0044a3a3
0044a3a3  mov        eax, dword ptr [esi + 0x34]
0044a3a6  add        eax, 0x2648
0044a3ab  or         dword ptr [eax + 0x14], 2

// block 0044a3af
0044a3af  mov        eax, dword ptr [esi + 0x28]
0044a3b2  mov        ebx, dword ptr [esi + 0x14]
0044a3b5  push       edi
0044a3b6  lea        edi, [eax - 0x12c]
0044a3bc  cmp        ebx, edi
0044a3be  je         0x44a3dc

// block 0044a3c0
0044a3c0  call       0x449f20

// block 0044a3c5
0044a3c5  test       eax, eax
0044a3c7  jne        0x44a3dc

// block 0044a3c9
0044a3c9  cmp        ebx, edi
0044a3cb  jge        0x44a3e9

// block 0044a3cd
0044a3cd  mov        edx, dword ptr [esi + 0x34]
0044a3d0  mov        dword ptr [edx + 0x127c], 0xfffffc19
0044a3da  jmp        0x44a3e9

// block 0044a3dc
0044a3dc  mov        eax, dword ptr [esi + 0x34]
0044a3df  mov        dword ptr [eax + 0x127c], 0x3e7

// block 0044a3e9
0044a3e9  mov        ecx, dword ptr [esi + 0x34]
0044a3ec  fld        dword ptr [ecx + 0x1074]
0044a3f2  mov        edi, dword ptr [0x4d1380]
0044a3f8  fmul       qword ptr [0x4a3d30]
0044a3fe  mov        ebx, dword ptr [edi]
0044a400  fdiv       qword ptr [0x4a3d28]
0044a406  call       0x4931e0

// block 0044a40b
0044a40b  mov        edx, dword ptr [ebx + 0x40]
0044a40e  push       eax
0044a40f  push       edi
0044a410  call       edx

// block 0044a412
0044a412  mov        eax, dword ptr [esi + 0x3c]
0044a415  mov        edi, dword ptr [0x4ce8cc]
0044a41b  push       eax
0044a41c  mov        edx, edi
0044a41e  call       0x461920

// block 0044a423
0044a423  cmp        eax, ebp
0044a425  jne        0x44a42a

// block 0044a427
0044a427  mov        dword ptr [esi + 0x3c], ebp

// block 0044a42a
0044a42a  mov        ecx, dword ptr [esi + 0x34]
0044a42d  fld        dword ptr [ecx + 0x1074]
0044a433  add        ecx, 0x1074
0044a439  fadd       qword ptr [0x4a3d70]
0044a43f  mov        edx, edi
0044a441  fadd       qword ptr [0x4a3d28]
0044a447  fstp       dword ptr [eax + 0x430]
0044a44d  fld        dword ptr [ecx + 4]
0044a450  fadd       qword ptr [0x4a3d68]
0044a456  fstp       dword ptr [eax + 0x434]
0044a45c  fld        dword ptr [ecx + 8]
0044a45f  fstp       dword ptr [eax + 0x438]
0044a465  mov        ecx, dword ptr [esi + 0x3c]
0044a468  push       ecx
0044a469  call       0x461920

// block 0044a46e
0044a46e  pop        edi
0044a46f  cmp        eax, ebp
0044a471  jne        0x44a476

// block 0044a473
0044a473  mov        dword ptr [esi + 0x3c], ebp

// block 0044a476
0044a476  add        eax, 0x10
0044a479  cmp        eax, ebp
0044a47b  je         0x44a497

// block 0044a47d
0044a47d  lea        ecx, [ecx]

// block 0044a480
0044a480  mov        edx, dword ptr [eax]
0044a482  mov        ecx, 0xce
0044a487  cmp        word ptr [edx + 0x3ea], cx
0044a48e  je         0x44a4ae

// block 0044a490
0044a490  mov        eax, dword ptr [eax + 4]
0044a493  cmp        eax, ebp
0044a495  jne        0x44a480

// block 0044a497
0044a497  xor        eax, eax

// block 0044a499
0044a499  fld        dword ptr [esi + 0x50]
0044a49c  or         dword ptr [eax + 0x47c], 4
0044a4a3  fmul       qword ptr [0x4a3cd0]
0044a4a9  fstp       dword ptr [eax + 0x24]
0044a4ac  jmp        0x44a4cd

// block 0044a4ae
0044a4ae  mov        eax, edx
0044a4b0  jmp        0x44a499

// block 0044a4b2
0044a4b2  cmp        dword ptr [0x4b0c5c], 3
0044a4b9  jne        0x44a4cd

// block 0044a4bb
0044a4bb  cmp        dword ptr [0x4b0c60], ebp
0044a4c1  jl         0x44a4cd

// block 0044a4c3
0044a4c3  mov        eax, dword ptr [0x4b0c64]
0044a4c8  call       0x44a8a0

// block 0044a4cd
0044a4cd  mov        eax, dword ptr [esi + 0x64]
0044a4d0  cmp        eax, ebp
0044a4d2  jle        0x44a4d8

// block 0044a4d4
0044a4d4  dec        eax
0044a4d5  mov        dword ptr [esi + 0x64], eax

// block 0044a4d8
0044a4d8  add        esi, 0x10
0044a4db  call       0x464a80

// block 0044a4e0
0044a4e0  pop        esi
0044a4e1  pop        ebp
0044a4e2  mov        eax, 1
0044a4e7  pop        ebx
0044a4e8  ret        
