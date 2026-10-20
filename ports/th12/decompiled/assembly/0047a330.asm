
// block 0047a330
0047a330  push       ebp
0047a331  mov        ebp, esp
0047a333  push       edi
0047a334  push       esi
0047a335  mov        esi, dword ptr [ebp + 0xc]
0047a338  mov        ecx, dword ptr [ebp + 0x10]
0047a33b  mov        edi, dword ptr [ebp + 8]
0047a33e  mov        eax, ecx
0047a340  mov        edx, ecx
0047a342  add        eax, esi
0047a344  cmp        edi, esi
0047a346  jbe        0x47a350

// block 0047a348
0047a348  cmp        edi, eax
0047a34a  jb         0x47a4f4

// block 0047a350
0047a350  cmp        ecx, 0x100
0047a356  jb         0x47a377

// block 0047a358
0047a358  cmp        dword ptr [0x4d52dc], 0
0047a35f  je         0x47a377

// block 0047a361
0047a361  push       edi
0047a362  push       esi
0047a363  and        edi, 0xf
0047a366  and        esi, 0xf
0047a369  cmp        edi, esi
0047a36b  pop        esi
0047a36c  pop        edi
0047a36d  jne        0x47a377

// block 0047a36f
0047a36f  pop        esi
0047a370  pop        edi
0047a371  pop        ebp
0047a372  jmp        0x48c4e8

// block 0047a377
0047a377  test       edi, 3
0047a37d  jne        0x47a394

// block 0047a37f
0047a37f  shr        ecx, 2
0047a382  and        edx, 3
0047a385  cmp        ecx, 8
0047a388  jb         0x47a3b4

// block 0047a38a
0047a38a  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0047a38c
0047a38c  jmp        dword ptr [edx*4 + 0x47a4a4]

// block 0047a394
0047a394  mov        eax, edi
0047a396  mov        edx, 3
0047a39b  sub        ecx, 4
0047a39e  jb         0x47a3ac

// block 0047a3a0
0047a3a0  and        eax, 3
0047a3a3  add        ecx, eax
0047a3a5  jmp        dword ptr [eax*4 + 0x47a3b8]

// block 0047a3ac
0047a3ac  jmp        dword ptr [ecx*4 + 0x47a4b4]

// block 0047a3b4
0047a3b4  jmp        dword ptr [ecx*4 + 0x47a438]

// block 0047a3c8
0047a3c8  and        edx, ecx
0047a3ca  mov        al, byte ptr [esi]
0047a3cc  mov        byte ptr [edi], al
0047a3ce  mov        al, byte ptr [esi + 1]
0047a3d1  mov        byte ptr [edi + 1], al
0047a3d4  mov        al, byte ptr [esi + 2]
0047a3d7  shr        ecx, 2
0047a3da  mov        byte ptr [edi + 2], al
0047a3dd  add        esi, 3
0047a3e0  add        edi, 3
0047a3e3  cmp        ecx, 8
0047a3e6  jb         0x47a3b4

// block 0047a3e8
0047a3e8  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0047a3ea
0047a3ea  jmp        dword ptr [edx*4 + 0x47a4a4]

// block 0047a3f4
0047a3f4  and        edx, ecx
0047a3f6  mov        al, byte ptr [esi]
0047a3f8  mov        byte ptr [edi], al
0047a3fa  mov        al, byte ptr [esi + 1]
0047a3fd  shr        ecx, 2
0047a400  mov        byte ptr [edi + 1], al
0047a403  add        esi, 2
0047a406  add        edi, 2
0047a409  cmp        ecx, 8
0047a40c  jb         0x47a3b4

// block 0047a40e
0047a40e  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0047a410
0047a410  jmp        dword ptr [edx*4 + 0x47a4a4]

// block 0047a418
0047a418  and        edx, ecx
0047a41a  mov        al, byte ptr [esi]
0047a41c  mov        byte ptr [edi], al
0047a41e  add        esi, 1
0047a421  shr        ecx, 2
0047a424  add        edi, 1
0047a427  cmp        ecx, 8
0047a42a  jb         0x47a3b4

// block 0047a42c
0047a42c  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0047a42e
0047a42e  jmp        dword ptr [edx*4 + 0x47a4a4]

// block 0047a458
0047a458  mov        eax, dword ptr [esi + ecx*4 - 0x1c]
0047a45c  mov        dword ptr [edi + ecx*4 - 0x1c], eax

// block 0047a460
0047a460  mov        eax, dword ptr [esi + ecx*4 - 0x18]
0047a464  mov        dword ptr [edi + ecx*4 - 0x18], eax

// block 0047a468
0047a468  mov        eax, dword ptr [esi + ecx*4 - 0x14]
0047a46c  mov        dword ptr [edi + ecx*4 - 0x14], eax

// block 0047a470
0047a470  mov        eax, dword ptr [esi + ecx*4 - 0x10]
0047a474  mov        dword ptr [edi + ecx*4 - 0x10], eax

// block 0047a478
0047a478  mov        eax, dword ptr [esi + ecx*4 - 0xc]
0047a47c  mov        dword ptr [edi + ecx*4 - 0xc], eax

// block 0047a480
0047a480  mov        eax, dword ptr [esi + ecx*4 - 8]
0047a484  mov        dword ptr [edi + ecx*4 - 8], eax

// block 0047a488
0047a488  mov        eax, dword ptr [esi + ecx*4 - 4]
0047a48c  mov        dword ptr [edi + ecx*4 - 4], eax
0047a490  lea        eax, [ecx*4]
0047a497  add        esi, eax
0047a499  add        edi, eax

// block 0047a49b
0047a49b  jmp        dword ptr [edx*4 + 0x47a4a4]

// block 0047a4b4
0047a4b4  mov        eax, dword ptr [ebp + 8]
0047a4b7  pop        esi
0047a4b8  pop        edi
0047a4b9  leave      
0047a4ba  ret        

// block 0047a4bc
0047a4bc  mov        al, byte ptr [esi]
0047a4be  mov        byte ptr [edi], al
0047a4c0  mov        eax, dword ptr [ebp + 8]
0047a4c3  pop        esi
0047a4c4  pop        edi
0047a4c5  leave      
0047a4c6  ret        

// block 0047a4c8
0047a4c8  mov        al, byte ptr [esi]
0047a4ca  mov        byte ptr [edi], al
0047a4cc  mov        al, byte ptr [esi + 1]
0047a4cf  mov        byte ptr [edi + 1], al
0047a4d2  mov        eax, dword ptr [ebp + 8]
0047a4d5  pop        esi
0047a4d6  pop        edi
0047a4d7  leave      
0047a4d8  ret        

// block 0047a4dc
0047a4dc  mov        al, byte ptr [esi]
0047a4de  mov        byte ptr [edi], al
0047a4e0  mov        al, byte ptr [esi + 1]
0047a4e3  mov        byte ptr [edi + 1], al
0047a4e6  mov        al, byte ptr [esi + 2]
0047a4e9  mov        byte ptr [edi + 2], al
0047a4ec  mov        eax, dword ptr [ebp + 8]
0047a4ef  pop        esi
0047a4f0  pop        edi
0047a4f1  leave      
0047a4f2  ret        

// block 0047a4f4
0047a4f4  lea        esi, [ecx + esi - 4]
0047a4f8  lea        edi, [ecx + edi - 4]
0047a4fc  test       edi, 3
0047a502  jne        0x47a528

// block 0047a504
0047a504  shr        ecx, 2
0047a507  and        edx, 3
0047a50a  cmp        ecx, 8
0047a50d  jb         0x47a51c

// block 0047a50f
0047a50f  std        

// block 0047a510
0047a510  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0047a512
0047a512  cld        
0047a513  jmp        dword ptr [edx*4 + 0x47a640]

// block 0047a51c
0047a51c  neg        ecx
0047a51e  jmp        dword ptr [ecx*4 + 0x47a5f0]

// block 0047a528
0047a528  mov        eax, edi
0047a52a  mov        edx, 3
0047a52f  cmp        ecx, 4
0047a532  jb         0x47a540

// block 0047a534
0047a534  and        eax, 3
0047a537  sub        ecx, eax
0047a539  jmp        dword ptr [eax*4 + 0x47a544]

// block 0047a540
0047a540  jmp        dword ptr [ecx*4 + 0x47a640]

// block 0047a554
0047a554  mov        al, byte ptr [esi + 3]
0047a557  and        edx, ecx
0047a559  mov        byte ptr [edi + 3], al
0047a55c  sub        esi, 1
0047a55f  shr        ecx, 2
0047a562  sub        edi, 1
0047a565  cmp        ecx, 8
0047a568  jb         0x47a51c

// block 0047a56a
0047a56a  std        

// block 0047a56b
0047a56b  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0047a56d
0047a56d  cld        
0047a56e  jmp        dword ptr [edx*4 + 0x47a640]

// block 0047a578
0047a578  mov        al, byte ptr [esi + 3]
0047a57b  and        edx, ecx
0047a57d  mov        byte ptr [edi + 3], al
0047a580  mov        al, byte ptr [esi + 2]
0047a583  shr        ecx, 2
0047a586  mov        byte ptr [edi + 2], al
0047a589  sub        esi, 2
0047a58c  sub        edi, 2
0047a58f  cmp        ecx, 8
0047a592  jb         0x47a51c

// block 0047a594
0047a594  std        

// block 0047a595
0047a595  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0047a597
0047a597  cld        
0047a598  jmp        dword ptr [edx*4 + 0x47a640]

// block 0047a5a0
0047a5a0  mov        al, byte ptr [esi + 3]
0047a5a3  and        edx, ecx
0047a5a5  mov        byte ptr [edi + 3], al
0047a5a8  mov        al, byte ptr [esi + 2]
0047a5ab  mov        byte ptr [edi + 2], al
0047a5ae  mov        al, byte ptr [esi + 1]
0047a5b1  shr        ecx, 2
0047a5b4  mov        byte ptr [edi + 1], al
0047a5b7  sub        esi, 3
0047a5ba  sub        edi, 3
0047a5bd  cmp        ecx, 8
0047a5c0  jb         0x47a51c

// block 0047a5c6
0047a5c6  std        

// block 0047a5c7
0047a5c7  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0047a5c9
0047a5c9  cld        
0047a5ca  jmp        dword ptr [edx*4 + 0x47a640]

// block 0047a5f4
0047a5f4  mov        eax, dword ptr [esi + ecx*4 + 0x1c]
0047a5f8  mov        dword ptr [edi + ecx*4 + 0x1c], eax

// block 0047a5fc
0047a5fc  mov        eax, dword ptr [esi + ecx*4 + 0x18]
0047a600  mov        dword ptr [edi + ecx*4 + 0x18], eax

// block 0047a604
0047a604  mov        eax, dword ptr [esi + ecx*4 + 0x14]
0047a608  mov        dword ptr [edi + ecx*4 + 0x14], eax

// block 0047a60c
0047a60c  mov        eax, dword ptr [esi + ecx*4 + 0x10]
0047a610  mov        dword ptr [edi + ecx*4 + 0x10], eax

// block 0047a614
0047a614  mov        eax, dword ptr [esi + ecx*4 + 0xc]
0047a618  mov        dword ptr [edi + ecx*4 + 0xc], eax

// block 0047a61c
0047a61c  mov        eax, dword ptr [esi + ecx*4 + 8]
0047a620  mov        dword ptr [edi + ecx*4 + 8], eax

// block 0047a624
0047a624  mov        eax, dword ptr [esi + ecx*4 + 4]
0047a628  mov        dword ptr [edi + ecx*4 + 4], eax
0047a62c  lea        eax, [ecx*4]
0047a633  add        esi, eax
0047a635  add        edi, eax

// block 0047a637
0047a637  jmp        dword ptr [edx*4 + 0x47a640]

// block 0047a650
0047a650  mov        eax, dword ptr [ebp + 8]
0047a653  pop        esi
0047a654  pop        edi
0047a655  leave      
0047a656  ret        

// block 0047a658
0047a658  mov        al, byte ptr [esi + 3]
0047a65b  mov        byte ptr [edi + 3], al
0047a65e  mov        eax, dword ptr [ebp + 8]
0047a661  pop        esi
0047a662  pop        edi
0047a663  leave      
0047a664  ret        

// block 0047a668
0047a668  mov        al, byte ptr [esi + 3]
0047a66b  mov        byte ptr [edi + 3], al
0047a66e  mov        al, byte ptr [esi + 2]
0047a671  mov        byte ptr [edi + 2], al
0047a674  mov        eax, dword ptr [ebp + 8]
0047a677  pop        esi
0047a678  pop        edi
0047a679  leave      
0047a67a  ret        

// block 0047a67c
0047a67c  mov        al, byte ptr [esi + 3]
0047a67f  mov        byte ptr [edi + 3], al
0047a682  mov        al, byte ptr [esi + 2]
0047a685  mov        byte ptr [edi + 2], al
0047a688  mov        al, byte ptr [esi + 1]
0047a68b  mov        byte ptr [edi + 1], al
0047a68e  mov        eax, dword ptr [ebp + 8]
0047a691  pop        esi
0047a692  pop        edi
0047a693  leave      
0047a694  ret        
