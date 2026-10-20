
// block 0040a250
0040a250  mov        ecx, dword ptr [esp + 0xc]
0040a254  sub        esp, 0x10
0040a257  push       ebx
0040a258  push       ebp
0040a259  mov        ebp, dword ptr [esp + 0x20]
0040a25d  push       esi
0040a25e  push       edi
0040a25f  mov        edi, dword ptr [esp + 0x24]
0040a263  mov        ebx, dword ptr [edi + 0x10]
0040a266  xor        edx, edx
0040a268  jmp        0x40a270

// block 0040a270
0040a270  cmp        word ptr [ebx + 0x532], 0
0040a278  je         0x40a336

// block 0040a27e
0040a27e  add        ebx, 0x9f8
0040a284  mov        eax, 5
0040a289  cmp        word ptr [ebx + 0x532], ax
0040a290  jne        0x40a295

// block 0040a292
0040a292  lea        ebx, [edi + 0x64]

// block 0040a295
0040a295  cmp        word ptr [ebx + 0x532], 0
0040a29d  je         0x40a326

// block 0040a2a3
0040a2a3  add        ebx, 0x9f8
0040a2a9  cmp        word ptr [ebx + 0x532], ax
0040a2b0  jne        0x40a2b5

// block 0040a2b2
0040a2b2  lea        ebx, [edi + 0x64]

// block 0040a2b5
0040a2b5  cmp        word ptr [ebx + 0x532], 0
0040a2bd  je         0x40a329

// block 0040a2bf
0040a2bf  add        ebx, 0x9f8
0040a2c5  cmp        word ptr [ebx + 0x532], ax
0040a2cc  jne        0x40a2d1

// block 0040a2ce
0040a2ce  lea        ebx, [edi + 0x64]

// block 0040a2d1
0040a2d1  cmp        word ptr [ebx + 0x532], 0
0040a2d9  je         0x40a32e

// block 0040a2db
0040a2db  add        ebx, 0x9f8
0040a2e1  cmp        word ptr [ebx + 0x532], ax
0040a2e8  jne        0x40a2ed

// block 0040a2ea
0040a2ea  lea        ebx, [edi + 0x64]

// block 0040a2ed
0040a2ed  cmp        word ptr [ebx + 0x532], 0
0040a2f5  je         0x40a333

// block 0040a2f7
0040a2f7  add        ebx, 0x9f8
0040a2fd  cmp        word ptr [ebx + 0x532], ax
0040a304  jne        0x40a309

// block 0040a306
0040a306  lea        ebx, [edi + 0x64]

// block 0040a309
0040a309  add        edx, eax
0040a30b  cmp        edx, 0x7d0
0040a311  jl         0x40a270

// block 0040a317
0040a317  mov        eax, 1
0040a31c  pop        edi
0040a31d  pop        esi
0040a31e  pop        ebp
0040a31f  pop        ebx
0040a320  add        esp, 0x10
0040a323  ret        0x14

// block 0040a326
0040a326  inc        edx
0040a327  jmp        0x40a336

// block 0040a329
0040a329  add        edx, 2
0040a32c  jmp        0x40a336

// block 0040a32e
0040a32e  add        edx, 3
0040a331  jmp        0x40a336

// block 0040a333
0040a333  add        edx, 4

// block 0040a336
0040a336  cmp        edx, 0x7d0
0040a33c  jl         0x40a34d

// block 0040a33e
0040a33e  mov        eax, 1
0040a343  pop        edi
0040a344  pop        esi
0040a345  pop        ebp
0040a346  pop        ebx
0040a347  add        esp, 0x10
0040a34a  ret        0x14

// block 0040a34d
0040a34d  movzx      eax, word ptr [ebp + 0x1fa]
0040a354  fldz       
0040a356  cmp        ax, 1
0040a35a  fstp       dword ptr [esp + 0x28]
0040a35e  jle        0x40a37b

// block 0040a360
0040a360  fld        dword ptr [ebp + 0x18]
0040a363  cwde       
0040a364  fld        dword ptr [ebp + 0x1c]
0040a367  mov        dword ptr [esp + 0x10], eax
0040a36b  fsubr      st(1)
0040a36d  fimul      dword ptr [esp + 0x30]
0040a371  fild       dword ptr [esp + 0x10]
0040a375  fdivp      st(1)
0040a377  fsubp      st(1)
0040a379  jmp        0x40a37e

// block 0040a37b
0040a37b  fld        dword ptr [ebp + 0x18]

// block 0040a37e
0040a37e  movzx      esi, word ptr [ebp + 0x1fc]
0040a385  fstp       dword ptr [esp + 0x10]
0040a389  movzx      eax, si
0040a38c  cmp        eax, 8
0040a38f  ja         0x40a618

// block 0040a395
0040a395  jmp        dword ptr [eax*4 + 0x40aa28]

// block 0040a39c
0040a39c  test       byte ptr [ebp + 0x1f8], 1
0040a3a3  je         0x40a3ba

// block 0040a3a5
0040a3a5  lea        eax, [ecx + 1]
0040a3a8  cdq        
0040a3a9  sub        eax, edx
0040a3ab  sar        eax, 1
0040a3ad  mov        dword ptr [esp + 0x2c], eax
0040a3b1  fild       dword ptr [esp + 0x2c]
0040a3b5  fmul       dword ptr [ebp + 0x14]
0040a3b8  jmp        0x40a3d7

// block 0040a3ba
0040a3ba  mov        eax, ecx
0040a3bc  cdq        
0040a3bd  sub        eax, edx
0040a3bf  sar        eax, 1
0040a3c1  mov        dword ptr [esp + 0x2c], eax
0040a3c5  fild       dword ptr [esp + 0x2c]
0040a3c9  fmul       dword ptr [ebp + 0x14]
0040a3cc  fld        dword ptr [ebp + 0x14]
0040a3cf  fmul       qword ptr [0x4a3cf8]
0040a3d5  faddp      st(1)

// block 0040a3d7
0040a3d7  fadd       qword ptr [0x4a3d58]
0040a3dd  fstp       dword ptr [esp + 0x28]
0040a3e1  test       cl, 1
0040a3e4  je         0x40a3f4

// block 0040a3e6
0040a3e6  fld        dword ptr [esp + 0x28]
0040a3ea  fmul       qword ptr [0x4a4168]
0040a3f0  fstp       dword ptr [esp + 0x28]

// block 0040a3f4
0040a3f4  test       si, si
0040a3f7  jne        0x40a405

// block 0040a3f9
0040a3f9  fld        dword ptr [esp + 0x28]
0040a3fd  fadd       dword ptr [esp + 0x34]
0040a401  fstp       dword ptr [esp + 0x28]

// block 0040a405
0040a405  fld        dword ptr [ebp + 0x10]
0040a408  fadd       dword ptr [esp + 0x28]
0040a40c  fstp       dword ptr [esp + 0x28]
0040a410  jmp        0x40a618

// block 0040a415
0040a415  fld        dword ptr [esp + 0x34]
0040a419  fadd       qword ptr [0x4a3d58]
0040a41f  fstp       dword ptr [esp + 0x28]

// block 0040a423
0040a423  fild       dword ptr [esp + 0x2c]
0040a427  movsx      ecx, word ptr [ebp + 0x1f8]
0040a42e  mov        dword ptr [esp + 0x2c], ecx
0040a432  fmul       qword ptr [0x4a3cd0]
0040a438  fild       dword ptr [esp + 0x2c]
0040a43c  fdivp      st(1)
0040a43e  fadd       dword ptr [esp + 0x28]
0040a442  fstp       dword ptr [esp + 0x2c]
0040a446  fld        dword ptr [esp + 0x2c]
0040a44a  fld        dword ptr [ebp + 0x14]
0040a44d  fimul      dword ptr [esp + 0x30]
0040a451  fadd       dword ptr [ebp + 0x10]
0040a454  faddp      st(1)
0040a456  fstp       dword ptr [esp + 0x28]
0040a45a  jmp        0x40a618

// block 0040a45f
0040a45f  fld        dword ptr [esp + 0x34]
0040a463  fadd       qword ptr [0x4a3d58]
0040a469  fstp       dword ptr [esp + 0x28]

// block 0040a46d
0040a46d  movsx      edx, word ptr [ebp + 0x1f8]
0040a474  mov        dword ptr [esp + 0x34], edx
0040a478  fild       dword ptr [esp + 0x34]
0040a47c  fld        qword ptr [0x4a3cd8]
0040a482  fdiv       st(1)
0040a484  fadd       dword ptr [esp + 0x28]
0040a488  fstp       dword ptr [esp + 0x34]
0040a48c  fild       dword ptr [esp + 0x2c]
0040a490  fmul       qword ptr [0x4a3cd0]
0040a496  fdivrp     st(1)
0040a498  fadd       dword ptr [esp + 0x34]
0040a49c  fstp       dword ptr [esp + 0x2c]
0040a4a0  fld        dword ptr [esp + 0x2c]
0040a4a4  fld        dword ptr [ebp + 0x14]
0040a4a7  fimul      dword ptr [esp + 0x30]
0040a4ab  fadd       dword ptr [ebp + 0x10]
0040a4ae  faddp      st(1)
0040a4b0  fstp       dword ptr [esp + 0x28]
0040a4b4  jmp        0x40a618

// block 0040a4b9
0040a4b9  fld        dword ptr [ebp + 0x10]
0040a4bc  mov        esi, 0x4ce568
0040a4c1  fsub       dword ptr [ebp + 0x14]
0040a4c4  fstp       dword ptr [esp + 0x30]
0040a4c8  call       0x464440

// block 0040a4cd
0040a4cd  mov        dword ptr [esp + 0x2c], eax
0040a4d1  fild       dword ptr [esp + 0x2c]
0040a4d5  test       eax, eax
0040a4d7  jge        0x40a4df

// block 0040a4d9
0040a4d9  fadd       dword ptr [0x4a3e10]

// block 0040a4df
0040a4df  fmul       qword ptr [0x4a3eb0]
0040a4e5  fstp       dword ptr [esp + 0x2c]
0040a4e9  fld        dword ptr [esp + 0x2c]
0040a4ed  fmul       dword ptr [esp + 0x30]
0040a4f1  fstp       dword ptr [esp + 0x2c]
0040a4f5  fld        dword ptr [ebp + 0x14]
0040a4f8  fadd       dword ptr [esp + 0x2c]
0040a4fc  fstp       dword ptr [esp + 0x28]
0040a500  jmp        0x40a618

// block 0040a505
0040a505  fld        dword ptr [ebp + 0x18]
0040a508  mov        esi, 0x4ce568
0040a50d  fsub       dword ptr [ebp + 0x1c]
0040a510  fstp       dword ptr [esp + 0x28]
0040a514  call       0x464440

// block 0040a519
0040a519  mov        dword ptr [esp + 0x34], eax
0040a51d  fild       dword ptr [esp + 0x34]
0040a521  test       eax, eax
0040a523  jge        0x40a52b

// block 0040a525
0040a525  fadd       dword ptr [0x4a3e10]

// block 0040a52b
0040a52b  fmul       qword ptr [0x4a3eb0]
0040a531  movsx      eax, word ptr [ebp + 0x1f8]
0040a538  fstp       dword ptr [esp + 0x34]
0040a53c  fld        dword ptr [esp + 0x34]
0040a540  fmul       dword ptr [esp + 0x28]
0040a544  fstp       dword ptr [esp + 0x34]
0040a548  fld        dword ptr [esp + 0x34]
0040a54c  fadd       dword ptr [ebp + 0x1c]
0040a54f  fstp       dword ptr [esp + 0x10]
0040a553  fild       dword ptr [esp + 0x2c]
0040a557  mov        dword ptr [esp + 0x2c], eax
0040a55b  fmul       qword ptr [0x4a3cd0]
0040a561  fild       dword ptr [esp + 0x2c]
0040a565  fdivp      st(1)
0040a567  fadd       qword ptr [0x4a3d58]
0040a56d  fstp       dword ptr [esp + 0x2c]
0040a571  fld        dword ptr [esp + 0x2c]
0040a575  fld        dword ptr [ebp + 0x14]
0040a578  fimul      dword ptr [esp + 0x30]
0040a57c  fadd       dword ptr [ebp + 0x10]
0040a57f  faddp      st(1)
0040a581  fstp       dword ptr [esp + 0x28]
0040a585  jmp        0x40a618

// block 0040a58a
0040a58a  fld        dword ptr [ebp + 0x10]
0040a58d  mov        esi, 0x4ce568
0040a592  fsub       dword ptr [ebp + 0x14]
0040a595  fstp       dword ptr [esp + 0x30]
0040a599  call       0x464440

// block 0040a59e
0040a59e  mov        dword ptr [esp + 0x2c], eax
0040a5a2  fild       dword ptr [esp + 0x2c]
0040a5a6  test       eax, eax
0040a5a8  jge        0x40a5b0

// block 0040a5aa
0040a5aa  fadd       dword ptr [0x4a3e10]

// block 0040a5b0
0040a5b0  fmul       qword ptr [0x4a3eb0]
0040a5b6  mov        esi, 0x4ce568
0040a5bb  fstp       dword ptr [esp + 0x2c]
0040a5bf  fld        dword ptr [esp + 0x2c]
0040a5c3  fmul       dword ptr [esp + 0x30]
0040a5c7  fstp       dword ptr [esp + 0x2c]
0040a5cb  fld        dword ptr [ebp + 0x14]
0040a5ce  fadd       dword ptr [esp + 0x2c]
0040a5d2  fstp       dword ptr [esp + 0x28]
0040a5d6  fld        dword ptr [ebp + 0x18]
0040a5d9  fsub       dword ptr [ebp + 0x1c]
0040a5dc  fstp       dword ptr [esp + 0x30]
0040a5e0  call       0x464440

// block 0040a5e5
0040a5e5  mov        dword ptr [esp + 0x2c], eax
0040a5e9  fild       dword ptr [esp + 0x2c]
0040a5ed  test       eax, eax
0040a5ef  jge        0x40a5f7

// block 0040a5f1
0040a5f1  fadd       dword ptr [0x4a3e10]

// block 0040a5f7
0040a5f7  fmul       qword ptr [0x4a3eb0]
0040a5fd  fstp       dword ptr [esp + 0x2c]
0040a601  fld        dword ptr [esp + 0x2c]
0040a605  fmul       dword ptr [esp + 0x30]
0040a609  fstp       dword ptr [esp + 0x2c]
0040a60d  fld        dword ptr [esp + 0x2c]
0040a611  fadd       dword ptr [ebp + 0x1c]
0040a614  fstp       dword ptr [esp + 0x10]

// block 0040a618
0040a618  fld        dword ptr [esp + 0x10]
0040a61c  sub        esp, 8
0040a61f  fstp       dword ptr [ebx + 0x4d4]
0040a625  fldz       
0040a627  fstp       dword ptr [esp + 4]
0040a62b  fld        dword ptr [esp + 0x30]
0040a62f  fstp       dword ptr [esp]
0040a632  call       0x464640

// block 0040a637
0040a637  fstp       dword ptr [esp + 0x2c]
0040a63b  fld        dword ptr [esp + 0x2c]
0040a63f  fst        dword ptr [ebx + 0x4d8]
0040a645  mov        ecx, dword ptr [ebp + 4]
0040a648  fldz       
0040a64a  mov        dword ptr [ebx + 0x4bc], ecx
0040a650  mov        edx, dword ptr [ebp + 8]
0040a653  mov        dword ptr [ebx + 0x4c0], edx
0040a659  mov        eax, dword ptr [ebp + 0xc]
0040a65c  mov        dword ptr [ebx + 0x4c4], eax
0040a662  fcom       dword ptr [ebp + 0x20]
0040a665  fnstsw     ax
0040a667  test       ah, 0x44
0040a66a  jnp        0x40a6a8

// block 0040a66c
0040a66c  fstp       st(0)
0040a66e  sub        esp, 8
0040a671  fld        dword ptr [ebp + 0x20]
0040a674  lea        ecx, [esp + 0x1c]
0040a678  fstp       dword ptr [esp + 4]
0040a67c  fstp       dword ptr [esp]
0040a67f  call       0x40d640

// block 0040a684
0040a684  fld        dword ptr [ebx + 0x4bc]
0040a68a  fadd       dword ptr [esp + 0x14]
0040a68e  fstp       dword ptr [ebx + 0x4bc]
0040a694  fld        dword ptr [esp + 0x18]
0040a698  fadd       dword ptr [ebx + 0x4c0]
0040a69e  fstp       dword ptr [ebx + 0x4c0]
0040a6a4  fldz       
0040a6a6  jmp        0x40a6aa

// block 0040a6a8
0040a6a8  fstp       st(1)

// block 0040a6aa
0040a6aa  fld        dword ptr [0x4a4210]
0040a6b0  fstp       dword ptr [ebx + 0x4c4]
0040a6b6  fcom       dword ptr [edi + 0x60]
0040a6b9  fnstsw     ax
0040a6bb  test       ah, 5
0040a6be  jp         0x40a710

// block 0040a6c0
0040a6c0  mov        eax, dword ptr [0x4b4514]
0040a6c5  fld        dword ptr [ebx + 0x4c0]
0040a6cb  fsub       dword ptr [eax + 0x980]
0040a6d1  fld        dword ptr [ebx + 0x4bc]
0040a6d7  fsub       dword ptr [eax + 0x97c]
0040a6dd  fld        dword ptr [edi + 0x60]
0040a6e0  fld        st(1)
0040a6e2  fmulp      st(2)
0040a6e4  fld        st(2)
0040a6e6  fmulp      st(3)
0040a6e8  fxch       st(1)
0040a6ea  faddp      st(2)
0040a6ec  fxch       st(1)
0040a6ee  fstp       dword ptr [esp + 0x2c]
0040a6f2  fld        dword ptr [esp + 0x2c]
0040a6f6  fcompp     
0040a6f8  fnstsw     ax
0040a6fa  test       ah, 5
0040a6fd  jp         0x40a710

// block 0040a6ff
0040a6ff  fstp       st(0)
0040a701  mov        eax, 0xffffffff
0040a706  pop        edi
0040a707  pop        esi
0040a708  pop        ebp
0040a709  pop        ebx
0040a70a  add        esp, 0x10
0040a70d  ret        0x14

// block 0040a710
0040a710  mov        ecx, 1
0040a715  or         dword ptr [ebx], ecx
0040a717  mov        word ptr [ebx + 0x532], cx
0040a71e  mov        eax, dword ptr [ebx + 0x4f4]
0040a724  mov        esi, 0xfff0bdc1
0040a729  mov        ecx, 0x4b2ed0
0040a72e  test       al, 1
0040a730  jne        0x40a757

// block 0040a732
0040a732  or         eax, 1
0040a735  fst        dword ptr [ebx + 0x4ec]
0040a73b  mov        dword ptr [ebx + 0x4e8], 0
0040a745  mov        dword ptr [ebx + 0x4e4], esi
0040a74b  mov        dword ptr [ebx + 0x4f0], ecx
0040a751  mov        dword ptr [ebx + 0x4f4], eax

// block 0040a757
0040a757  or         edi, 0xffffffff
0040a75a  fst        dword ptr [ebx + 0x4ec]
0040a760  mov        dword ptr [ebx + 0x4e8], 0
0040a76a  mov        dword ptr [ebx + 0x4e4], edi
0040a770  mov        eax, dword ptr [ebx + 0x508]
0040a776  xor        edx, edx
0040a778  test       al, 1
0040a77a  jne        0x40a79d

// block 0040a77c
0040a77c  or         eax, 1
0040a77f  fst        dword ptr [ebx + 0x500]
0040a785  mov        dword ptr [ebx + 0x4fc], edx
0040a78b  mov        dword ptr [ebx + 0x4f8], esi
0040a791  mov        dword ptr [ebx + 0x504], ecx
0040a797  mov        dword ptr [ebx + 0x508], eax

// block 0040a79d
0040a79d  fstp       dword ptr [ebx + 0x500]
0040a7a3  sub        esp, 8
0040a7a6  fld        dword ptr [esp + 0x18]
0040a7aa  mov        dword ptr [ebx + 0x4fc], edx
0040a7b0  fstp       dword ptr [esp + 4]
0040a7b4  mov        dword ptr [ebx + 0x4f8], edi
0040a7ba  fld        dword ptr [esp + 0x30]
0040a7be  lea        ecx, [ebx + 0x4c8]
0040a7c4  fstp       dword ptr [esp]
0040a7c7  mov        dword ptr [ebx + 4], edx
0040a7ca  call       0x40d640

// block 0040a7cf
0040a7cf  mov        eax, dword ptr [ebp + 0x200]
0040a7d5  mov        dword ptr [ebx + 0x528], eax
0040a7db  mov        cx, word ptr [ebp + 2]
0040a7df  mov        word ptr [ebx + 0x9f6], cx
0040a7e6  mov        ecx, dword ptr [ebx]
0040a7e8  mov        ax, word ptr [ebp]
0040a7ec  and        ecx, 0xfffffff3
0040a7ef  or         ecx, 2
0040a7f2  lea        esi, [ebx + 8]
0040a7f5  mov        word ptr [ebx + 0x9f4], ax
0040a7fc  mov        dword ptr [ebx + 0x540], edx
0040a802  mov        dword ptr [ebx], ecx
0040a804  call       0x402520

// block 0040a809
0040a809  mov        ecx, dword ptr [esp + 0x24]
0040a80d  mov        dword ptr [ebx + 0x4b4], 0x40d430
0040a817  mov        dword ptr [ebx + 0x4b8], ebx
0040a81d  movsx      edx, word ptr [ebp]
0040a821  mov        ecx, dword ptr [ecx + 0x4debdc]
0040a827  imul       edx, edx, 0xd0
0040a82d  mov        edx, dword ptr [edx + 0x4af280]
0040a833  mov        eax, esi
0040a835  call       0x454ee0

// block 0040a83a
0040a83a  and        dword ptr [ebx], 0xffffffef
0040a83d  movsx      edx, word ptr [ebp]
0040a841  mov        ecx, dword ptr [ebx]
0040a843  imul       edx, edx, 0xd0
0040a849  mov        eax, dword ptr [edx + 0x4af34c]
0040a84f  cmp        eax, 4
0040a852  ja         0x40a8a1

// block 0040a854
0040a854  jmp        dword ptr [eax*4 + 0x40aa4c]

// block 0040a85b
0040a85b  movsx      eax, word ptr [ebp + 2]
0040a85f  lea        ecx, [eax + eax + 4]
0040a863  mov        dword ptr [ebx + 0x524], ecx
0040a869  jmp        0x40a8a1

// block 0040a86b
0040a86b  movsx      edx, word ptr [ebp + 2]
0040a86f  mov        eax, dword ptr [edx*4 + 0x4b0bb0]
0040a876  mov        dword ptr [ebx + 0x524], eax
0040a87c  jmp        0x40a8a1

// block 0040a87e
0040a87e  or         ecx, 0x10
0040a881  mov        dword ptr [ebx + 0x524], edi
0040a887  mov        dword ptr [ebx], ecx
0040a889  jmp        0x40a8a1

// block 0040a88b
0040a88b  mov        dword ptr [ebx + 0x524], 0x10
0040a895  jmp        0x40a8a1

// block 0040a897
0040a897  mov        dword ptr [ebx + 0x524], 6

// block 0040a8a1
0040a8a1  movsx      ecx, word ptr [ebp]
0040a8a5  imul       ecx, ecx, 0xd0
0040a8ab  mov        edx, dword ptr [ecx + 0x4af348]
0040a8b1  mov        dword ptr [ebx + 0x54c], edx
0040a8b7  mov        eax, dword ptr [ebp + 0x208]
0040a8bd  mov        dword ptr [ebx + 0x544], eax
0040a8c3  mov        dword ptr [ebx + 0x520], 0xa
0040a8cd  movsx      ecx, word ptr [ebp]
0040a8d1  imul       ecx, ecx, 0xd0
0040a8d7  lea        esi, [ebp + 0x24]
0040a8da  lea        edi, [ebx + 0x550]
0040a8e0  fld        dword ptr [ecx + 0x4af344]
0040a8e6  mov        ecx, 0x6c
0040a8eb  fstp       dword ptr [esp + 0x2c]
0040a8ef  fld        dword ptr [esp + 0x2c]
0040a8f3  fst        dword ptr [ebx + 0x4e0]
0040a8f9  fstp       dword ptr [ebx + 0x4dc]
0040a8ff  mov        edx, dword ptr [ebp + 0x200]
0040a905  mov        dword ptr [ebx + 0x52c], edx
0040a90b  mov        dword ptr [ebx + 0x528], 0
0040a915  mov        eax, dword ptr [ebp + 0x20c]
0040a91b  mov        dword ptr [ebx + 0x548], eax

// block 0040a921
0040a921  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0040a923
0040a923  lea        ecx, [eax + eax*2]
0040a926  mov        eax, dword ptr [ebx + 0x49c]
0040a92c  lea        ebp, [ebp + ecx*8]
0040a930  mov        edx, 2
0040a935  lea        esi, [ebx + 8]
0040a938  cmp        dword ptr [ebp + 0x34], edx
0040a93b  jne        0x40a9cc

// block 0040a941
0040a941  mov        dx, word ptr [ebp + 0x2c]
0040a945  add        dx, 7
0040a949  movzx      edi, dx
0040a94c  test       eax, eax
0040a94e  je         0x40a957

// block 0040a950
0040a950  movsx      edx, di
0040a953  mov        ecx, esi
0040a955  call       eax

// block 0040a957
0040a957  mov        word ptr [esi + 0x3c4], di
0040a95e  mov        eax, 2
0040a963  mov        word ptr [ebx + 0x532], ax
0040a96a  fld        dword ptr [ebx + 0x4c8]
0040a970  fld        qword ptr [0x4a4108]
0040a976  fmul       st(1), st(0)
0040a978  fxch       st(1)
0040a97a  fstp       dword ptr [esp + 0x14]
0040a97e  fld        dword ptr [ebx + 0x4cc]
0040a984  fmul       st(1)
0040a986  fstp       dword ptr [esp + 0x18]
0040a98a  fmul       dword ptr [ebx + 0x4d0]
0040a990  fstp       dword ptr [esp + 0x1c]
0040a994  fld        dword ptr [ebx + 0x4bc]
0040a99a  fsub       dword ptr [esp + 0x14]
0040a99e  fstp       dword ptr [ebx + 0x4bc]
0040a9a4  fld        dword ptr [ebx + 0x4c0]
0040a9aa  fsub       dword ptr [esp + 0x18]
0040a9ae  fstp       dword ptr [ebx + 0x4c0]
0040a9b4  fld        dword ptr [ebx + 0x4c4]
0040a9ba  fsub       dword ptr [esp + 0x1c]
0040a9be  fstp       dword ptr [ebx + 0x4c4]
0040a9c4  inc        dword ptr [ebx + 0x548]
0040a9ca  jmp        0x40a9e0

// block 0040a9cc
0040a9cc  test       eax, eax
0040a9ce  je         0x40a9d4

// block 0040a9d0
0040a9d0  mov        ecx, esi
0040a9d2  call       eax

// block 0040a9d4
0040a9d4  mov        ecx, 2
0040a9d9  mov        word ptr [esi + 0x3c4], cx

// block 0040a9e0
0040a9e0  mov        ecx, ebx
0040a9e2  call       0x40aa60

// block 0040a9e7
0040a9e7  lea        eax, [ebx + 8]
0040a9ea  push       eax
0040a9eb  call       0x455630

// block 0040a9f0
0040a9f0  mov        eax, dword ptr [esp + 0x24]
0040a9f4  add        ebx, 0x9f8
0040a9fa  cmp        word ptr [ebx + 0x532], 5
0040aa02  jne        0x40aa16

// block 0040aa04
0040aa04  lea        edx, [eax + 0x64]
0040aa07  mov        dword ptr [eax + 0x10], edx
0040aa0a  xor        eax, eax
0040aa0c  pop        edi
0040aa0d  pop        esi
0040aa0e  pop        ebp
0040aa0f  pop        ebx
0040aa10  add        esp, 0x10
0040aa13  ret        0x14

// block 0040aa16
0040aa16  pop        edi
0040aa17  pop        esi
0040aa18  mov        dword ptr [eax + 0x10], ebx
0040aa1b  pop        ebp
0040aa1c  xor        eax, eax
0040aa1e  pop        ebx
0040aa1f  add        esp, 0x10
0040aa22  ret        0x14
