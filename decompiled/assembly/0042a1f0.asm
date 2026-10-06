
// block 0042a1f0
0042a1f0  push       ebp
0042a1f1  mov        ebp, esp
0042a1f3  and        esp, 0xfffffff8
0042a1f6  sub        esp, 0x33c
0042a1fc  mov        eax, dword ptr [0x4ad138]
0042a201  xor        eax, esp
0042a203  mov        dword ptr [esp + 0x338], eax
0042a20a  push       ebx
0042a20b  push       esi
0042a20c  push       edi
0042a20d  xor        edi, edi
0042a20f  mov        esi, ecx
0042a211  mov        dword ptr [esp + 0x44], esi
0042a215  cmp        dword ptr [ebp + 0x14], edi
0042a218  je         0x42a23b

// block 0042a21a
0042a21a  cmp        dword ptr [esi + 0x44c], edi
0042a220  je         0x42a23b

// block 0042a222
0042a222  xor        eax, eax
0042a224  pop        edi
0042a225  pop        esi
0042a226  pop        ebx
0042a227  mov        ecx, dword ptr [esp + 0x338]
0042a22e  xor        ecx, esp
0042a230  call       0x46c932

// block 0042a235
0042a235  mov        esp, ebp
0042a237  pop        ebp
0042a238  ret        0x10

// block 0042a23b
0042a23b  mov        eax, dword ptr [esi + 0x50]
0042a23e  fld        dword ptr [0x4a3e54]
0042a244  mov        ecx, dword ptr [esi + 0x54]
0042a247  fstp       dword ptr [esp + 0x10]
0042a24b  mov        edx, dword ptr [esi + 0x58]
0042a24e  push       0x100
0042a253  mov        dword ptr [esp + 0x4c], eax
0042a257  lea        eax, [esp + 0x244]
0042a25e  push       edi
0042a25f  push       eax
0042a260  mov        dword ptr [esp + 0x34], edi
0042a264  mov        dword ptr [esp + 0x58], ecx
0042a268  mov        dword ptr [esp + 0x5c], edx
0042a26c  call       0x477420

// block 0042a271
0042a271  fldz       
0042a273  fstp       dword ptr [esp + 0x4c]
0042a277  add        esp, 4
0042a27a  fld        dword ptr [0x4a3e54]
0042a280  lea        ecx, [esp + 0x40]
0042a284  fstp       dword ptr [esp + 4]
0042a288  fld        dword ptr [esi + 0x68]
0042a28b  fstp       dword ptr [esp]
0042a28e  call       0x42e880

// block 0042a293
0042a293  fld        dword ptr [esp + 0x38]
0042a297  fld        st(0)
0042a299  fadd       dword ptr [esi + 0x50]
0042a29c  fstp       dword ptr [esp + 0x2c]
0042a2a0  mov        ecx, dword ptr [esp + 0x2c]
0042a2a4  fld        dword ptr [esi + 0x54]
0042a2a7  mov        dword ptr [esp + 0x14], ecx
0042a2ab  fld        dword ptr [esp + 0x3c]
0042a2af  fld        st(0)
0042a2b1  faddp      st(2)
0042a2b3  fxch       st(1)
0042a2b5  fstp       dword ptr [esp + 0x30]
0042a2b9  mov        edx, dword ptr [esp + 0x30]
0042a2bd  fld        dword ptr [esi + 0x58]
0042a2c0  mov        dword ptr [esp + 0x18], edx
0042a2c4  fld        dword ptr [esp + 0x40]
0042a2c8  fld        st(0)
0042a2ca  faddp      st(2)
0042a2cc  fxch       st(1)
0042a2ce  fstp       dword ptr [esp + 0x34]
0042a2d2  mov        eax, dword ptr [esp + 0x34]
0042a2d6  fldz       
0042a2d8  mov        dword ptr [esp + 0x1c], eax
0042a2dc  fstp       dword ptr [esp + 0x1c]
0042a2e0  fld        st(2)
0042a2e2  faddp      st(3)
0042a2e4  fxch       st(2)
0042a2e6  fstp       dword ptr [esp + 0x38]
0042a2ea  fadd       st(0), st(0)
0042a2ec  fstp       dword ptr [esp + 0x3c]
0042a2f0  fadd       st(0), st(0)
0042a2f2  fstp       dword ptr [esp + 0x40]
0042a2f6  fld        dword ptr [ebp + 0xc]
0042a2f9  fmul       st(0), st(0)
0042a2fb  fstp       dword ptr [ebp + 0xc]
0042a2fe  fld        dword ptr [esi + 0x6c]
0042a301  fcomp      qword ptr [0x4a3d68]
0042a307  fnstsw     ax
0042a309  test       ah, 1
0042a30c  jne        0x42a5f3

// block 0042a312
0042a312  fld        qword ptr [0x4a3d70]
0042a318  jmp        0x42a328

// block 0042a320
0042a320  fstp       st(3)
0042a322  fstp       st(2)
0042a324  fstp       st(1)
0042a326  fstp       st(0)

// block 0042a328
0042a328  mov        eax, dword ptr [ebp + 8]
0042a32b  fld        dword ptr [eax + 4]
0042a32e  fld        dword ptr [esp + 0x18]
0042a332  fld        st(0)
0042a334  fsubp      st(2)
0042a336  fld        dword ptr [eax]
0042a338  fld        dword ptr [esp + 0x14]
0042a33c  fld        st(0)
0042a33e  fsubp      st(2)
0042a340  fld        st(1)
0042a342  fmulp      st(2)
0042a344  fld        st(3)
0042a346  fmulp      st(4)
0042a348  fxch       st(1)
0042a34a  faddp      st(3)
0042a34c  fxch       st(2)
0042a34e  fstp       dword ptr [esp + 0x20]
0042a352  fld        dword ptr [esp + 0x20]
0042a356  fld        dword ptr [ebp + 0xc]
0042a359  fcompp     
0042a35b  fnstsw     ax
0042a35d  test       ah, 5
0042a360  jnp        0x42a4ae

// block 0042a366
0042a366  inc        dword ptr [esp + 0x28]
0042a36a  test       byte ptr [ebp + 0x10], 1
0042a36e  mov        byte ptr [esp + edi + 0x240], 1
0042a376  je         0x42a3de

// block 0042a378
0042a378  fld        st(1)
0042a37a  fadd       st(3)
0042a37c  fcomp      qword ptr [0x4a3d60]
0042a382  fnstsw     ax
0042a384  test       ah, 0x41
0042a387  jnp        0x42a3de

// block 0042a389
0042a389  fxch       st(1)
0042a38b  fsub       st(2)
0042a38d  fcomp      qword ptr [0x4a3d28]
0042a393  fnstsw     ax
0042a395  test       ah, 1
0042a398  je         0x42a3e0

// block 0042a39a
0042a39a  fld        st(0)
0042a39c  fadd       st(2)
0042a39e  fcomp      qword ptr [0x4a3d58]
0042a3a4  fnstsw     ax
0042a3a6  test       ah, 0x41
0042a3a9  jnp        0x42a3e0

// block 0042a3ab
0042a3ab  fsubrp     st(1)
0042a3ad  fcomp      qword ptr [0x4a3d50]
0042a3b3  fnstsw     ax
0042a3b5  test       ah, 1
0042a3b8  je         0x42a3e4

// block 0042a3ba
0042a3ba  fld        dword ptr [0x4a41f4]
0042a3c0  sub        esp, 8
0042a3c3  fstp       dword ptr [esp + 4]
0042a3c7  lea        ecx, [esp + 0x1c]
0042a3cb  fld        dword ptr [0x4a4060]
0042a3d1  fstp       dword ptr [esp]
0042a3d4  push       ecx
0042a3d5  push       9
0042a3d7  call       0x4273f0

// block 0042a3dc
0042a3dc  jmp        0x42a3e4

// block 0042a3de
0042a3de  fstp       st(1)

// block 0042a3e0
0042a3e0  fstp       st(0)
0042a3e2  fstp       st(0)

// block 0042a3e4
0042a3e4  test       dword ptr [0x4cee78], 0x8000
0042a3ee  movsx      edx, word ptr [esi + 0x47a]
0042a3f5  mov        ecx, dword ptr [0x4b44f4]
0042a3fb  mov        ebx, dword ptr [ecx + 0x488]
0042a401  lea        eax, [edx + edx + 4]
0042a405  mov        dword ptr [esp + 0x20], eax
0042a409  je         0x42a41c

// block 0042a40b
0042a40b  push       0x4cf1d0
0042a410  call       dword ptr [0x498088]

// block 0042a416
0042a416  inc        byte ptr [0x4cf221]

// block 0042a41c
0042a41c  inc        dword ptr [ebx + 0x130]
0042a422  call       0x4621c0

// block 0042a427
0042a427  fld        dword ptr [esp + 0x14]
0042a42b  fadd       qword ptr [0x4a3d70]
0042a431  mov        edx, dword ptr [esp + 0x20]
0042a435  mov        esi, eax
0042a437  or         dword ptr [esi + 0x480], 1
0042a43e  fadd       qword ptr [0x4a3d28]
0042a444  mov        dword ptr [esi + 0x20], 0x17
0042a44b  push       edx
0042a44c  push       esi
0042a44d  fstp       dword ptr [esi + 0x430]
0042a453  mov        ecx, ebx
0042a455  fld        dword ptr [esp + 0x20]
0042a459  fadd       qword ptr [0x4a3d68]
0042a45f  fstp       dword ptr [esi + 0x434]
0042a465  fld        dword ptr [esp + 0x24]
0042a469  fstp       dword ptr [esi + 0x438]
0042a46f  call       0x454d10

// block 0042a474
0042a474  mov        ebx, esi
0042a476  lea        eax, [esp + 0x54]
0042a47a  call       0x461250

// block 0042a47f
0042a47f  test       dword ptr [0x4cee78], 0x8000
0042a489  je         0x42a49c

// block 0042a48b
0042a48b  push       0x4cf1d0
0042a490  call       dword ptr [0x49808c]

// block 0042a496
0042a496  dec        byte ptr [0x4cf221]

// block 0042a49c
0042a49c  fld        qword ptr [0x4a3d70]
0042a4a2  mov        esi, dword ptr [esp + 0x44]
0042a4a6  fld        dword ptr [esp + 0x14]
0042a4aa  fld        dword ptr [esp + 0x18]

// block 0042a4ae
0042a4ae  fld        dword ptr [esp + 0x38]
0042a4b2  inc        edi
0042a4b3  fld        st(0)
0042a4b5  faddp      st(3)
0042a4b7  fxch       st(2)
0042a4b9  fstp       dword ptr [esp + 0x14]
0042a4bd  fld        dword ptr [esp + 0x3c]
0042a4c1  fld        st(0)
0042a4c3  faddp      st(2)
0042a4c5  fxch       st(1)
0042a4c7  fstp       dword ptr [esp + 0x18]
0042a4cb  fld        dword ptr [esp + 0x1c]
0042a4cf  fld        dword ptr [esp + 0x40]
0042a4d3  fld        st(0)
0042a4d5  faddp      st(2)
0042a4d7  fxch       st(1)
0042a4d9  fstp       dword ptr [esp + 0x1c]
0042a4dd  fld        dword ptr [esp + 0x10]
0042a4e1  fld        qword ptr [0x4a3d68]
0042a4e7  fadd       st(1), st(0)
0042a4e9  fxch       st(1)
0042a4eb  fstp       dword ptr [esp + 0x10]
0042a4ef  fld        dword ptr [esp + 0x10]
0042a4f3  fadd       qword ptr [0x4a4078]
0042a4f9  fld        dword ptr [esi + 0x6c]
0042a4fc  fcompp     
0042a4fe  fnstsw     ax
0042a500  test       ah, 1
0042a503  je         0x42a320

// block 0042a509
0042a509  mov        eax, dword ptr [esp + 0x28]
0042a50d  fstp       st(4)
0042a50f  mov        dword ptr [esp + 0x20], edi
0042a513  test       eax, eax
0042a515  je         0x42a5eb

// block 0042a51b
0042a51b  cmp        eax, edi
0042a51d  jl         0x42a542

// block 0042a51f
0042a51f  fstp       st(2)
0042a521  mov        byte ptr [esi + 0x7c], 1
0042a525  fstp       st(2)
0042a527  fstp       st(0)
0042a529  fstp       st(0)
0042a52b  pop        edi
0042a52c  pop        esi
0042a52d  pop        ebx
0042a52e  mov        ecx, dword ptr [esp + 0x338]
0042a535  xor        ecx, esp
0042a537  call       0x46c932

// block 0042a53c
0042a53c  mov        esp, ebp
0042a53e  pop        ebp
0042a53f  ret        0x10

// block 0042a542
0042a542  fld        dword ptr [0x4a3e00]
0042a548  xor        ebx, ebx
0042a54a  test       edi, edi
0042a54c  jle        0x42a60e

// block 0042a552
0042a552  cmp        byte ptr [esp + ebx + 0x240], 0
0042a55a  je         0x42a561

// block 0042a55c
0042a55c  inc        ebx
0042a55d  cmp        ebx, edi
0042a55f  jl         0x42a552

// block 0042a561
0042a561  mov        dword ptr [esp + 0x10], ebx
0042a565  test       ebx, ebx
0042a567  je         0x42a60e

// block 0042a56d
0042a56d  fild       dword ptr [esp + 0x10]
0042a571  fstp       dword ptr [esp + 0x10]
0042a575  fld        dword ptr [esp + 0x10]
0042a579  fst        dword ptr [esp + 0x10]
0042a57d  fld        dword ptr [esp + 0x10]
0042a581  fld        st(0)
0042a583  fmulp      st(6)
0042a585  fxch       st(5)
0042a587  fstp       dword ptr [esp + 0x2c]
0042a58b  fld        st(3)
0042a58d  fmul       st(5)
0042a58f  fstp       dword ptr [esp + 0x30]
0042a593  fld        st(2)
0042a595  fmulp      st(5)
0042a597  fxch       st(4)
0042a599  fstp       dword ptr [esp + 0x34]
0042a59d  fld        dword ptr [esi + 0x50]
0042a5a0  fadd       dword ptr [esp + 0x2c]
0042a5a4  fstp       dword ptr [esi + 0x50]
0042a5a7  fld        dword ptr [esp + 0x30]
0042a5ab  fadd       dword ptr [esi + 0x54]
0042a5ae  fstp       dword ptr [esi + 0x54]
0042a5b1  fld        dword ptr [esp + 0x34]
0042a5b5  fadd       dword ptr [esi + 0x58]
0042a5b8  fstp       dword ptr [esi + 0x58]
0042a5bb  fxch       st(3)
0042a5bd  fmul       st(4)
0042a5bf  fld        dword ptr [esi + 0x6c]
0042a5c2  fsub       st(1)
0042a5c4  fstp       dword ptr [esp + 0x10]
0042a5c8  fld        dword ptr [esp + 0x10]
0042a5cc  fst        dword ptr [esi + 0x6c]
0042a5cf  fcom       st(4)
0042a5d1  fnstsw     ax
0042a5d3  test       ah, 0x41
0042a5d6  jne        0x42a5e3

// block 0042a5d8
0042a5d8  fstp       dword ptr [esi + 0x464]
0042a5de  fstp       dword ptr [esi + 0x78]
0042a5e1  jmp        0x42a610

// block 0042a5e3
0042a5e3  fstp       st(1)
0042a5e5  mov        byte ptr [esi + 0x7c], 1
0042a5e9  fstp       st(0)

// block 0042a5eb
0042a5eb  fstp       st(2)
0042a5ed  fstp       st(2)
0042a5ef  fstp       st(0)

// block 0042a5f1
0042a5f1  fstp       st(0)

// block 0042a5f3
0042a5f3  mov        ecx, dword ptr [esp + 0x344]
0042a5fa  mov        eax, dword ptr [esp + 0x28]
0042a5fe  pop        edi
0042a5ff  pop        esi
0042a600  pop        ebx
0042a601  xor        ecx, esp
0042a603  call       0x46c932

// block 0042a608
0042a608  mov        esp, ebp
0042a60a  pop        ebp
0042a60b  ret        0x10

// block 0042a60e
0042a60e  fstp       st(3)

// block 0042a610
0042a610  xor        eax, eax
0042a612  cmp        ebx, edi
0042a614  jge        0x42a5eb

// block 0042a616
0042a616  cmp        byte ptr [esp + ebx + 0x240], 0
0042a61e  jne        0x42a628

// block 0042a620
0042a620  inc        ebx
0042a621  inc        eax
0042a622  cmp        ebx, edi
0042a624  jl         0x42a616

// block 0042a626
0042a626  jmp        0x42a5eb

// block 0042a628
0042a628  cmp        ebx, edi
0042a62a  mov        dword ptr [esp + 0x10], eax
0042a62e  jge        0x42a5eb

// block 0042a630
0042a630  fild       dword ptr [esp + 0x10]
0042a634  fmul       st(4)
0042a636  fld        dword ptr [esi + 0x464]
0042a63c  fld        dword ptr [esi + 0x6c]
0042a63f  fsub       st(2)
0042a641  fsubp      st(1)
0042a643  fstp       dword ptr [esi + 0x464]
0042a649  fstp       dword ptr [esp + 0x10]
0042a64d  fld        dword ptr [esp + 0x10]
0042a651  fst        dword ptr [esi + 0x6c]
0042a654  fcomp      st(3)
0042a656  fnstsw     ax
0042a658  test       ah, 5
0042a65b  jp         0x42a661

// block 0042a65d
0042a65d  mov        byte ptr [esi + 0x7c], 1

// block 0042a661
0042a661  mov        edx, dword ptr [0x4b44f4]
0042a667  jmp        0x42a685

// block 0042a669
0042a669  fld        qword ptr [0x4a3d68]
0042a66f  mov        edi, dword ptr [esp + 0x20]
0042a673  fld        dword ptr [esp + 0x3c]
0042a677  mov        esi, dword ptr [esp + 0x44]
0042a67b  fld        dword ptr [esp + 0x40]
0042a67f  fxch       st(2)
0042a681  fxch       st(3)
0042a683  fxch       st(2)

// block 0042a685
0042a685  cmp        ebx, edi
0042a687  jge        0x42a5eb

// block 0042a68d
0042a68d  cmp        byte ptr [esp + ebx + 0x240], 0
0042a695  je         0x42a69a

// block 0042a697
0042a697  inc        ebx
0042a698  jmp        0x42a685

// block 0042a69a
0042a69a  cmp        ebx, edi
0042a69c  jge        0x42a5eb

// block 0042a6a2
0042a6a2  xor        eax, eax
0042a6a4  mov        dword ptr [esp + 0x24], ebx

// block 0042a6a8
0042a6a8  cmp        byte ptr [esp + ebx + 0x240], 0
0042a6b0  jne        0x42a6b8

// block 0042a6b2
0042a6b2  inc        ebx
0042a6b3  inc        eax
0042a6b4  cmp        ebx, edi
0042a6b6  jl         0x42a6a8

// block 0042a6b8
0042a6b8  mov        dword ptr [esp + 0x10], eax
0042a6bc  fild       dword ptr [esp + 0x10]
0042a6c0  add        esi, 0x454
0042a6c6  mov        ecx, 0x7a
0042a6cb  lea        edi, [esp + 0x58]
0042a6cf  fmulp      st(4)

// block 0042a6d1
0042a6d1  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0042a6d3
0042a6d3  fxch       st(3)
0042a6d5  fstp       dword ptr [esp + 0x6c]
0042a6d9  fld        dword ptr [esp + 0x6c]
0042a6dd  fst        dword ptr [esp + 0x68]
0042a6e1  fcomp      st(2)
0042a6e3  fnstsw     ax
0042a6e5  test       ah, 0x41
0042a6e8  jne        0x42a7de

// block 0042a6ee
0042a6ee  fild       dword ptr [esp + 0x24]
0042a6f2  lea        edi, [edx + 0x468]
0042a6f8  fstp       dword ptr [esp + 0x24]
0042a6fc  fld        dword ptr [esp + 0x38]
0042a700  fld        dword ptr [esp + 0x24]
0042a704  mov        dword ptr [esp + 0x24], edx
0042a708  fld        st(0)
0042a70a  fmulp      st(2)
0042a70c  fxch       st(1)
0042a70e  fstp       dword ptr [esp + 0x2c]
0042a712  fld        st(0)
0042a714  fmulp      st(2)
0042a716  fxch       st(1)
0042a718  fstp       dword ptr [esp + 0x30]
0042a71c  fmulp      st(2)
0042a71e  fxch       st(1)
0042a720  fstp       dword ptr [esp + 0x34]
0042a724  fld        dword ptr [esp + 0x48]
0042a728  fadd       dword ptr [esp + 0x2c]
0042a72c  fstp       dword ptr [esp + 0x14]
0042a730  mov        eax, dword ptr [esp + 0x14]
0042a734  fld        dword ptr [esp + 0x4c]
0042a738  mov        dword ptr [esp + 0x58], eax
0042a73c  fadd       dword ptr [esp + 0x30]
0042a740  fstp       dword ptr [esp + 0x18]
0042a744  mov        ecx, dword ptr [esp + 0x18]
0042a748  fld        dword ptr [esp + 0x50]
0042a74c  mov        dword ptr [esp + 0x5c], ecx
0042a750  fadd       dword ptr [esp + 0x34]
0042a754  fstp       dword ptr [esp + 0x1c]
0042a758  mov        eax, dword ptr [esp + 0x1c]
0042a75c  mov        dword ptr [esp + 0x60], eax
0042a760  cmp        dword ptr [edi], 0x100
0042a766  jge        0x42a7e2

// block 0042a768
0042a768  inc        dword ptr [edx + 0x46c]
0042a76e  fstp       st(0)
0042a770  cmp        dword ptr [edx + 0x46c], 0x10000
0042a77a  lea        esi, [edx + 0x46c]
0042a780  jge        0x42a788

// block 0042a782
0042a782  mov        dword ptr [esi], 0x10000

// block 0042a788
0042a788  push       0xfa4
0042a78d  call       0x46c9ea

// block 0042a792
0042a792  add        esp, 4
0042a795  test       eax, eax
0042a797  je         0x42a7a0

// block 0042a799
0042a799  call       0x428520

// block 0042a79e
0042a79e  jmp        0x42a7a2

// block 0042a7a0
0042a7a0  xor        eax, eax

// block 0042a7a2
0042a7a2  mov        ecx, dword ptr [esi]
0042a7a4  mov        edx, dword ptr [esp + 0x24]
0042a7a8  mov        dword ptr [eax + 0x80], ecx
0042a7ae  mov        ecx, dword ptr [edx + 0x464]
0042a7b4  mov        dword ptr [eax + 4], ecx
0042a7b7  mov        dword ptr [ecx + 8], eax
0042a7ba  inc        dword ptr [edi]
0042a7bc  mov        dword ptr [edx + 0x464], eax
0042a7c2  mov        edx, dword ptr [eax]
0042a7c4  mov        edx, dword ptr [edx + 4]
0042a7c7  lea        ecx, [esp + 0x58]
0042a7cb  push       ecx
0042a7cc  mov        ecx, eax
0042a7ce  call       edx

// block 0042a7d0
0042a7d0  fld        dword ptr [0x4a3e00]
0042a7d6  mov        edx, dword ptr [0x4b44f4]
0042a7dc  jmp        0x42a7e2

// block 0042a7de
0042a7de  fstp       st(2)
0042a7e0  fstp       st(1)

// block 0042a7e2
0042a7e2  cmp        ebx, dword ptr [esp + 0x20]
0042a7e6  jl         0x42a669

// block 0042a7ec
0042a7ec  jmp        0x42a5f1
