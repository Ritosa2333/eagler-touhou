
// block 0041f040
0041f040  push       ebp
0041f041  mov        ebp, esp
0041f043  and        esp, 0xfffffff8
0041f046  sub        esp, 0x2c
0041f049  push       ebx
0041f04a  push       esi
0041f04b  mov        esi, dword ptr [ebp + 8]
0041f04e  mov        eax, dword ptr [esi + 0x6cac]
0041f054  push       edi
0041f055  mov        edi, dword ptr [0x4ce8cc]
0041f05b  push       eax
0041f05c  mov        edx, edi
0041f05e  call       0x461920

// block 0041f063
0041f063  xor        ebx, ebx
0041f065  cmp        eax, ebx
0041f067  jne        0x41f07a

// block 0041f069
0041f069  mov        edi, dword ptr [0x4b43b8]
0041f06f  mov        dword ptr [esi + 0x6cac], ebx
0041f075  jmp        0x41f11a

// block 0041f07a
0041f07a  fld        dword ptr [0x4a40d4]
0041f080  mov        ecx, dword ptr [esi + 0x6cac]
0041f086  fstp       dword ptr [esp + 0x28]
0041f08a  push       ecx
0041f08b  fld        dword ptr [0x4a41fc]
0041f091  mov        edx, edi
0041f093  fstp       dword ptr [esp + 0x30]
0041f097  fldz       
0041f099  fstp       dword ptr [esp + 0x34]
0041f09d  call       0x461920

// block 0041f0a2
0041f0a2  cmp        eax, ebx
0041f0a4  jne        0x41f0ac

// block 0041f0a6
0041f0a6  mov        dword ptr [esi + 0x6cac], ebx

// block 0041f0ac
0041f0ac  mov        dl, byte ptr [eax + 0x3bf]
0041f0b2  mov        eax, dword ptr [0x4b43b8]
0041f0b7  mov        dword ptr [eax + 0x18fa4], ebx
0041f0bd  mov        dword ptr [eax + 0x18fa8], ebx
0041f0c3  mov        byte ptr [eax + 0x18f83], dl
0041f0c9  mov        dword ptr [eax + 0x18f9c], 1
0041f0d3  mov        dword ptr [eax + 0x18f98], 3
0041f0dd  mov        ecx, dword ptr [esi + 0x6d48]
0041f0e3  lea        ebx, [esp + 0x28]
0041f0e7  call       0x401610

// block 0041f0ec
0041f0ec  mov        edi, dword ptr [0x4b43b8]
0041f0f2  xor        eax, eax
0041f0f4  mov        dword ptr [edi + 0x18f98], eax
0041f0fa  mov        dword ptr [edi + 0x18f9c], eax
0041f100  mov        eax, 1
0041f105  mov        byte ptr [edi + 0x18f83], 0xff
0041f10c  mov        dword ptr [edi + 0x18fa4], eax
0041f112  mov        dword ptr [edi + 0x18fa8], eax
0041f118  xor        ebx, ebx

// block 0041f11a
0041f11a  cmp        dword ptr [esi + 0x6cb8], ebx
0041f120  je         0x41f33d

// block 0041f126
0041f126  fld        dword ptr [0x4a41f8]
0041f12c  mov        eax, dword ptr [esi + 0x6cb4]
0041f132  mov        edx, dword ptr [0x4ce8cc]
0041f138  fstp       dword ptr [esp + 0x1c]
0041f13c  fld        dword ptr [0x4a3d88]
0041f142  push       eax
0041f143  fstp       dword ptr [esp + 0x24]
0041f147  fldz       
0041f149  fstp       dword ptr [esp + 0x28]
0041f14d  call       0x461920

// block 0041f152
0041f152  mov        dword ptr [esp + 0x18], eax
0041f156  cmp        eax, ebx
0041f158  jne        0x41f160

// block 0041f15a
0041f15a  mov        dword ptr [esi + 0x6cb4], ebx

// block 0041f160
0041f160  mov        ecx, dword ptr [0x4b43cc]
0041f166  cmp        ecx, ebx
0041f168  jne        0x41f187

// block 0041f16a
0041f16a  mov        dword ptr [esi + 0x6cb8], ebx
0041f170  mov        ecx, dword ptr [esi + 0x6cb4]
0041f176  push       ecx
0041f177  call       0x461a70

// block 0041f17c
0041f17c  mov        dword ptr [esi + 0x6cb4], ebx
0041f182  jmp        0x41f33d

// block 0041f187
0041f187  cmp        eax, ebx
0041f189  jne        0x41f196

// block 0041f18b
0041f18b  mov        dword ptr [esi + 0x6cb8], ebx
0041f191  jmp        0x41f33d

// block 0041f196
0041f196  mov        dl, byte ptr [eax + 0x3bf]
0041f19c  mov        byte ptr [edi + 0x18f83], dl
0041f1a2  mov        dword ptr [edi + 0x18f9c], 1
0041f1ac  mov        dword ptr [edi + 0x18f98], 3
0041f1b6  mov        ecx, dword ptr [ecx + 0x94]
0041f1bc  mov        eax, 0x88888889
0041f1c1  imul       ecx
0041f1c3  add        edx, ecx
0041f1c5  sar        edx, 5
0041f1c8  mov        eax, edx
0041f1ca  shr        eax, 0x1f
0041f1cd  add        eax, edx
0041f1cf  cmp        eax, 0x3e8
0041f1d4  jl         0x41f1db

// block 0041f1d6
0041f1d6  mov        eax, 0x3e7

// block 0041f1db
0041f1db  push       eax
0041f1dc  push       0x49fd38
0041f1e1  lea        ebx, [esp + 0x24]
0041f1e5  mov        esi, edi
0041f1e7  call       0x4015c0

// block 0041f1ec
0041f1ec  fld        dword ptr [0x4a41f4]
0041f1f2  mov        esi, dword ptr [0x4b43b8]
0041f1f8  mov        eax, dword ptr [0x4b43cc]
0041f1fd  fst        dword ptr [esi + 0x18f84]
0041f203  fstp       dword ptr [esi + 0x18f88]
0041f209  mov        ecx, dword ptr [eax + 0x94]
0041f20f  mov        eax, 0x88888889
0041f214  fld        dword ptr [0x4a41f0]
0041f21a  imul       ecx
0041f21c  fstp       dword ptr [esp + 0x24]
0041f220  fld        dword ptr [0x4a41ec]
0041f226  fstp       dword ptr [esp + 0x28]
0041f22a  add        edx, ecx
0041f22c  sar        edx, 5
0041f22f  mov        eax, edx
0041f231  shr        eax, 0x1f
0041f234  add        eax, edx
0041f236  mov        edx, eax
0041f238  shl        edx, 4
0041f23b  sub        edx, eax
0041f23d  add        edx, edx
0041f23f  add        edx, edx
0041f241  sub        ecx, edx
0041f243  imul       ecx, ecx, 0x64
0041f246  mov        eax, 0x88888889
0041f24b  imul       ecx
0041f24d  add        edx, ecx
0041f24f  sar        edx, 5
0041f252  mov        eax, edx
0041f254  shr        eax, 0x1f
0041f257  add        eax, edx
0041f259  push       eax
0041f25a  push       0x49fd40
0041f25f  call       0x4015c0

// block 0041f264
0041f264  mov        esi, dword ptr [0x4b43b8]
0041f26a  fld        dword ptr [0x4a41f8]
0041f270  mov        ecx, dword ptr [esp + 0x28]
0041f274  fstp       dword ptr [esp + 0x2c]
0041f278  mov        dword ptr [esi + 0x18f80], 0xff808080
0041f282  fld        dword ptr [0x4a41e8]
0041f288  mov        dl, byte ptr [ecx + 0x3bf]
0041f28e  fstp       dword ptr [esp + 0x30]
0041f292  add        esp, 0x10
0041f295  fld1       
0041f297  lea        eax, [esp + 0x18]
0041f29b  fst        dword ptr [esi + 0x18f84]
0041f2a1  push       eax
0041f2a2  lea        ebx, [esp + 0x18]
0041f2a6  fstp       dword ptr [esi + 0x18f88]
0041f2ac  mov        byte ptr [esi + 0x18f83], dl
0041f2b2  call       0x41d0b0

// block 0041f2b7
0041f2b7  mov        ecx, dword ptr [esp + 0x18]
0041f2bb  push       ecx
0041f2bc  push       0x49fd38
0041f2c1  lea        ebx, [esp + 0x24]
0041f2c5  call       0x4015c0

// block 0041f2ca
0041f2ca  fld        dword ptr [0x4a41f4]
0041f2d0  mov        esi, dword ptr [0x4b43b8]
0041f2d6  mov        edx, dword ptr [esp + 0x1c]
0041f2da  fst        dword ptr [esi + 0x18f84]
0041f2e0  fstp       dword ptr [esi + 0x18f88]
0041f2e6  push       edx
0041f2e7  fld        dword ptr [0x4a41f0]
0041f2ed  push       0x49fd40
0041f2f2  fstp       dword ptr [esp + 0x2c]
0041f2f6  fld        dword ptr [0x4a41e4]
0041f2fc  fstp       dword ptr [esp + 0x30]
0041f300  call       0x4015c0

// block 0041f305
0041f305  mov        eax, dword ptr [0x4b43b8]
0041f30a  fld1       
0041f30c  add        esp, 0x10
0041f30f  fst        dword ptr [eax + 0x18f84]
0041f315  xor        ecx, ecx
0041f317  fstp       dword ptr [eax + 0x18f88]
0041f31d  mov        byte ptr [eax + 0x18f83], 0xff
0041f324  mov        dword ptr [eax + 0x18f98], ecx
0041f32a  mov        esi, dword ptr [ebp + 8]
0041f32d  mov        dword ptr [eax + 0x18f9c], ecx
0041f333  mov        dword ptr [eax + 0x18f80], 0xffffffff

// block 0041f33d
0041f33d  fldz       
0041f33f  fcomp      dword ptr [esi + 0x6ce8]
0041f345  fnstsw     ax
0041f347  test       ah, 5
0041f34a  jp         0x41f450

// block 0041f350
0041f350  fld        dword ptr [0x4a41e0]
0041f356  mov        ebx, 0xff000000
0041f35b  fstp       dword ptr [esp + 0x28]
0041f35f  lea        edi, [esp + 0x28]
0041f363  fld        dword ptr [esi + 0x6ce8]
0041f369  fmul       qword ptr [0x4a41d8]
0041f36f  fadd       qword ptr [0x4a41d0]
0041f375  fstp       dword ptr [esp + 0x30]
0041f379  fld        dword ptr [0x4a41cc]
0041f37f  fstp       dword ptr [esp + 0x2c]
0041f383  fld        dword ptr [0x4a41c8]
0041f389  fstp       dword ptr [esp + 0x34]
0041f38d  call       0x451f30

// block 0041f392
0041f392  fld        dword ptr [esp + 0x28]
0041f396  or         ebx, 0xffffffff
0041f399  fld1       
0041f39b  fsub       st(1), st(0)
0041f39d  fxch       st(1)
0041f39f  fstp       dword ptr [esp + 0x28]
0041f3a3  fld        dword ptr [esp + 0x30]
0041f3a7  fsub       st(1)
0041f3a9  fstp       dword ptr [esp + 0x30]
0041f3ad  fld        dword ptr [esp + 0x2c]
0041f3b1  fsub       st(1)
0041f3b3  fstp       dword ptr [esp + 0x2c]
0041f3b7  fsubr      dword ptr [esp + 0x34]
0041f3bb  fstp       dword ptr [esp + 0x34]
0041f3bf  call       0x451f30

// block 0041f3c4
0041f3c4  lea        ecx, [esi + 0x6cf8]
0041f3ca  mov        dword ptr [esp + 0x14], ecx
0041f3ce  mov        dword ptr [esp + 0x18], 4

// block 0041f3d6
0041f3d6  fldz       
0041f3d8  fcomp      dword ptr [ecx]
0041f3da  fnstsw     ax
0041f3dc  test       ah, 0x44
0041f3df  jnp        0x41f442

// block 0041f3e1
0041f3e1  fld        dword ptr [ecx]
0041f3e3  fld        dword ptr [esi + 0x6ce8]
0041f3e9  fcompp     
0041f3eb  fnstsw     ax
0041f3ed  test       ah, 0x41
0041f3f0  jne        0x41f3f6

// block 0041f3f2
0041f3f2  fld        dword ptr [ecx]
0041f3f4  jmp        0x41f3fc

// block 0041f3f6
0041f3f6  fld        dword ptr [esi + 0x6ce8]

// block 0041f3fc
0041f3fc  mov        ebx, dword ptr [ecx + 4]
0041f3ff  fstp       dword ptr [esp + 0x10]
0041f403  fld        dword ptr [0x4a4010]
0041f409  lea        edi, [esp + 0x28]
0041f40d  fstp       dword ptr [esp + 0x28]
0041f411  fld        dword ptr [esp + 0x10]
0041f415  fmul       qword ptr [0x4a41d8]
0041f41b  fadd       qword ptr [0x4a41c0]
0041f421  fstp       dword ptr [esp + 0x30]
0041f425  fld        dword ptr [0x4a41b8]
0041f42b  fstp       dword ptr [esp + 0x2c]
0041f42f  fld        dword ptr [0x4a3e00]
0041f435  fstp       dword ptr [esp + 0x34]
0041f439  call       0x451f30

// block 0041f43e
0041f43e  mov        ecx, dword ptr [esp + 0x14]

// block 0041f442
0041f442  add        ecx, 8
0041f445  sub        dword ptr [esp + 0x18], 1
0041f44a  mov        dword ptr [esp + 0x14], ecx
0041f44e  jne        0x41f3d6

// block 0041f450
0041f450  lea        edi, [esi + 0x54b8]
0041f456  mov        ebx, 4

// block 0041f45b
0041f45b  mov        eax, dword ptr [0x4ce8cc]
0041f460  push       eax
0041f461  mov        eax, edi
0041f463  call       0x45c900

// block 0041f468
0041f468  add        edi, 0x4b4
0041f46e  sub        ebx, 1
0041f471  jne        0x41f45b

// block 0041f473
0041f473  mov        ecx, dword ptr [0x4b43b8]
0041f479  fld        dword ptr [0x4a3f50]
0041f47f  mov        dword ptr [ecx + 0x18f98], 3
0041f489  fstp       dword ptr [esp + 0x20]
0041f48d  mov        dl, byte ptr [esi + 0x3cf]
0041f493  fldz       
0041f495  mov        byte ptr [ecx + 0x18f83], dl
0041f49b  fstp       dword ptr [esp + 0x24]
0041f49f  test       byte ptr [0x4b0ce0], 0x10
0041f4a6  jne        0x41f4b5

// block 0041f4a8
0041f4a8  mov        eax, dword ptr [0x4b0c40]
0041f4ad  mov        edx, dword ptr [0x4b0cc8]
0041f4b3  jmp        0x41f4f5

// block 0041f4b5
0041f4b5  mov        eax, dword ptr [0x4b0c94]
0041f4ba  mov        edx, dword ptr [0x4b0c90]
0041f4c0  mov        edi, dword ptr [0x4b0cb0]
0041f4c6  lea        edx, [eax + edx*2]
0041f4c9  mov        eax, dword ptr [0x4b0ca8]
0041f4ce  imul       edx, edx, 0x45f4
0041f4d4  add        edx, dword ptr [0x4b451c]
0041f4da  lea        eax, [eax + eax*2]
0041f4dd  lea        eax, [edi + eax*2]
0041f4e0  mov        eax, dword ptr [edx + eax*8 + 0x5a4]
0041f4e7  mov        edx, dword ptr [esi + 0x6cdc]
0041f4ed  cmp        eax, edx
0041f4ef  jge        0x41f4f3

// block 0041f4f1
0041f4f1  mov        eax, edx

// block 0041f4f3
0041f4f3  xor        edx, edx

// block 0041f4f5
0041f4f5  fld        dword ptr [0x4a41b4]
0041f4fb  mov        edi, 2
0041f500  mov        dword ptr [ecx + 0x18fa4], edi
0041f506  fstp       dword ptr [esp + 0x1c]
0041f50a  mov        dword ptr [ecx + 0x18fa8], 1
0041f514  lea        ecx, [eax + eax*4]
0041f517  lea        ecx, [edx + ecx*2]
0041f51a  lea        ebx, [esp + 0x1c]
0041f51e  call       0x401610

// block 0041f523
0041f523  fld        dword ptr [0x4a40d8]
0041f529  mov        esi, dword ptr [esi + 0x6cdc]
0041f52f  fstp       dword ptr [esp + 0x28]
0041f533  fld        dword ptr [0x4a41b0]
0041f539  mov        edx, dword ptr [esp + 0x28]
0041f53d  fstp       dword ptr [esp + 0x2c]
0041f541  mov        eax, dword ptr [esp + 0x2c]
0041f545  fldz       
0041f547  mov        dword ptr [esp + 0x1c], edx
0041f54b  fstp       dword ptr [esp + 0x30]
0041f54f  mov        dword ptr [esp + 0x20], eax
0041f553  mov        ecx, dword ptr [esp + 0x30]
0041f557  fld        dword ptr [0x4a41b4]
0041f55d  mov        eax, dword ptr [0x4b0cc4]
0041f562  fstp       dword ptr [esp + 0x1c]
0041f566  lea        edx, [esi + esi*4]
0041f569  mov        dword ptr [esp + 0x24], ecx
0041f56d  lea        ecx, [eax + edx*2]
0041f570  call       0x401610

// block 0041f575
0041f575  fld        dword ptr [0x4a41ac]
0041f57b  mov        esi, dword ptr [0x4b43b8]
0041f581  fstp       dword ptr [esp + 0x28]
0041f585  fld        dword ptr [0x4a41a8]
0041f58b  mov        eax, 1
0041f590  fstp       dword ptr [esp + 0x2c]
0041f594  mov        edx, dword ptr [esp + 0x2c]
0041f598  fldz       
0041f59a  mov        dword ptr [esi + 0x18fa4], eax
0041f5a0  mov        dword ptr [esi + 0x18fa8], eax
0041f5a6  fstp       dword ptr [esp + 0x30]
0041f5aa  mov        eax, dword ptr [esp + 0x30]
0041f5ae  mov        dword ptr [esp + 0x24], eax
0041f5b2  mov        eax, dword ptr [0x4b0c48]
0041f5b7  mov        dword ptr [esp + 0x20], edx
0041f5bb  cdq        
0041f5bc  idiv       dword ptr [0x4b0cd4]
0041f5c2  mov        ecx, dword ptr [esp + 0x28]
0041f5c6  mov        dword ptr [esp + 0x1c], ecx
0041f5ca  push       eax
0041f5cb  push       0x49fd48
0041f5d0  call       0x4015c0

// block 0041f5d5
0041f5d5  fld        dword ptr [0x4a41f4]
0041f5db  mov        esi, dword ptr [0x4b43b8]
0041f5e1  fst        dword ptr [esi + 0x18f84]
0041f5e7  fstp       dword ptr [esi + 0x18f88]
0041f5ed  mov        eax, dword ptr [0x4b0c48]
0041f5f2  mov        ecx, dword ptr [0x4b0cd4]
0041f5f8  fld        dword ptr [0x4a41a4]
0041f5fe  cdq        
0041f5ff  fstp       dword ptr [esp + 0x24]
0041f603  idiv       ecx
0041f605  fld        dword ptr [esp + 0x28]
0041f609  fadd       qword ptr [0x4a4120]
0041f60f  fstp       dword ptr [esp + 0x28]
0041f613  mov        eax, edx
0041f615  imul       eax, eax, 0x64
0041f618  cdq        
0041f619  idiv       ecx
0041f61b  push       eax
0041f61c  push       0x49fd4c
0041f621  call       0x4015c0

// block 0041f626
0041f626  fld1       
0041f628  mov        esi, dword ptr [0x4b43b8]
0041f62e  fst        dword ptr [esi + 0x18f84]
0041f634  fstp       dword ptr [esi + 0x18f88]
0041f63a  fld        dword ptr [esp + 0x30]
0041f63e  mov        eax, dword ptr [0x4b0cd0]
0041f643  fsub       qword ptr [0x4a4120]
0041f649  cdq        
0041f64a  fstp       dword ptr [esp + 0x30]
0041f64e  fld        dword ptr [0x4a41a0]
0041f654  fstp       dword ptr [esp + 0x2c]
0041f658  idiv       dword ptr [0x4b0cd4]
0041f65e  push       eax
0041f65f  push       0x49fd54
0041f664  call       0x4015c0

// block 0041f669
0041f669  fld        dword ptr [0x4a419c]
0041f66f  mov        esi, dword ptr [0x4b43b8]
0041f675  fstp       dword ptr [esp + 0x34]
0041f679  fld        dword ptr [esp + 0x38]
0041f67d  push       0x49fd5c
0041f682  fadd       qword ptr [0x4a4120]
0041f688  fstp       dword ptr [esp + 0x3c]
0041f68c  fld        dword ptr [0x4a41f4]
0041f692  fst        dword ptr [esi + 0x18f84]
0041f698  fstp       dword ptr [esi + 0x18f88]
0041f69e  call       0x4015c0

// block 0041f6a3
0041f6a3  mov        eax, dword ptr [0x4b43b8]
0041f6a8  fld1       
0041f6aa  fst        dword ptr [eax + 0x18f84]
0041f6b0  mov        dword ptr [eax + 0x18fa4], edi
0041f6b6  fstp       dword ptr [eax + 0x18f88]
0041f6bc  mov        dword ptr [eax + 0x18fa8], 1
0041f6c6  fld        dword ptr [0x4a41b4]
0041f6cc  mov        esi, 0xa
0041f6d1  fstp       dword ptr [esp + 0x44]
0041f6d5  add        esp, 0x1c
0041f6d8  fld        dword ptr [0x4a4198]
0041f6de  mov        ecx, dword ptr [esp + 0x28]
0041f6e2  fstp       dword ptr [esp + 0x2c]
0041f6e6  mov        dword ptr [esp + 0x1c], ecx
0041f6ea  fldz       
0041f6ec  mov        edx, dword ptr [esp + 0x2c]
0041f6f0  mov        ecx, dword ptr [0x4b0c78]
0041f6f6  fstp       dword ptr [esp + 0x30]
0041f6fa  mov        dword ptr [esp + 0x20], edx
0041f6fe  mov        eax, dword ptr [esp + 0x30]
0041f702  mov        dword ptr [esp + 0x24], eax
0041f706  mov        eax, 0x51eb851f
0041f70b  imul       ecx
0041f70d  sar        edx, 5
0041f710  mov        ecx, edx
0041f712  shr        ecx, 0x1f
0041f715  add        ecx, edx
0041f717  mov        eax, ecx
0041f719  cdq        
0041f71a  idiv       esi
0041f71c  sub        ecx, edx
0041f71e  call       0x401610

// block 0041f723
0041f723  fld        dword ptr [0x4a41b4]
0041f729  fstp       dword ptr [esp + 0x28]
0041f72d  mov        ecx, dword ptr [esp + 0x28]
0041f731  fld        dword ptr [0x4a41fc]
0041f737  mov        dword ptr [esp + 0x1c], ecx
0041f73b  mov        ecx, dword ptr [0x4b0cdc]
0041f741  fstp       dword ptr [esp + 0x2c]
0041f745  fldz       
0041f747  mov        edx, dword ptr [esp + 0x2c]
0041f74b  fstp       dword ptr [esp + 0x30]
0041f74f  mov        dword ptr [esp + 0x20], edx
0041f753  mov        eax, dword ptr [esp + 0x30]
0041f757  mov        dword ptr [esp + 0x24], eax
0041f75b  call       0x401610

// block 0041f760
0041f760  mov        eax, dword ptr [0x4b43b8]
0041f765  xor        edi, edi
0041f767  mov        ebx, 1
0041f76c  mov        dword ptr [eax + 0x18f80], 0xffffffff
0041f776  mov        dword ptr [eax + 0x18fa4], ebx
0041f77c  mov        dword ptr [eax + 0x18fa8], ebx
0041f782  mov        byte ptr [eax + 0x18f83], 0xff
0041f789  mov        dword ptr [eax + 0x18f98], edi
0041f78f  mov        dword ptr [eax + 0x18f9c], edi
0041f795  mov        eax, dword ptr [0x4b43dc]
0041f79a  cmp        eax, edi
0041f79c  je         0x41f8c4

// block 0041f7a2
0041f7a2  mov        esi, dword ptr [ebp + 8]
0041f7a5  cmp        dword ptr [esi + 0x6d38], edi
0041f7ab  jl         0x41f8c4

// block 0041f7b1
0041f7b1  cmp        dword ptr [eax + 0x1c], edi
0041f7b4  je         0x41f8c4

// block 0041f7ba
0041f7ba  test       byte ptr [eax + 0x3c], bl
0041f7bd  jne        0x41f8c4

// block 0041f7c3
0041f7c3  cmp        dword ptr [esi + 0x6d30], edi
0041f7c9  jne        0x41f8c4

// block 0041f7cf
0041f7cf  add        esi, 0x4b50
0041f7d5  mov        dword ptr [esp + 0x18], 2
0041f7dd  lea        ecx, [ecx]

// block 0041f7e0
0041f7e0  mov        ecx, dword ptr [0x4ce8cc]
0041f7e6  push       ecx
0041f7e7  mov        eax, esi
0041f7e9  call       0x45c900

// block 0041f7ee
0041f7ee  add        esi, 0x4b4
0041f7f4  sub        dword ptr [esp + 0x18], ebx
0041f7f8  jne        0x41f7e0

// block 0041f7fa
0041f7fa  fld        dword ptr [0x4a4194]
0041f800  mov        esi, dword ptr [0x4b43b8]
0041f806  mov        edx, dword ptr [ebp + 8]
0041f809  fstp       dword ptr [esp + 0x1c]
0041f80d  fld        dword ptr [0x4a3e68]
0041f813  mov        eax, dword ptr [edx + 0x4f0c]
0041f819  fstp       dword ptr [esp + 0x20]
0041f81d  mov        dword ptr [esi + 0x18f9c], ebx
0041f823  fldz       
0041f825  push       0x49fd60
0041f82a  lea        ebx, [esp + 0x20]
0041f82e  fstp       dword ptr [esp + 0x28]
0041f832  mov        dword ptr [esi + 0x18f80], eax
0041f838  mov        dword ptr [esi + 0x18f98], 3
0041f842  call       0x4015c0

// block 0041f847
0041f847  fld        dword ptr [0x4a4190]
0041f84d  mov        esi, dword ptr [0x4b43b8]
0041f853  fstp       dword ptr [esp + 0x20]
0041f857  fld        dword ptr [0x4a41b8]
0041f85d  mov        ecx, dword ptr [ebp + 8]
0041f860  fstp       dword ptr [esp + 0x24]
0041f864  add        esp, 4
0041f867  fld        dword ptr [0x4a41f4]
0041f86d  fst        dword ptr [esi + 0x18f84]
0041f873  fstp       dword ptr [esi + 0x18f88]
0041f879  mov        edx, dword ptr [ecx + 0x6d3c]
0041f87f  push       edx
0041f880  push       0x49fd4c
0041f885  call       0x4015c0

// block 0041f88a
0041f88a  fld1       
0041f88c  mov        eax, dword ptr [0x4b43b8]
0041f891  fst        dword ptr [eax + 0x18f84]
0041f897  fstp       dword ptr [eax + 0x18f88]
0041f89d  mov        dword ptr [eax + 0x18f80], 0xffffffff
0041f8a7  mov        dword ptr [eax + 0x18f9c], edi
0041f8ad  mov        dword ptr [eax + 0x18f98], edi
0041f8b3  add        esp, 8
0041f8b6  mov        eax, 1
0041f8bb  pop        edi
0041f8bc  pop        esi
0041f8bd  pop        ebx
0041f8be  mov        esp, ebp
0041f8c0  pop        ebp
0041f8c1  ret        4

// block 0041f8c4
0041f8c4  pop        edi
0041f8c5  pop        esi
0041f8c6  mov        eax, ebx
0041f8c8  pop        ebx
0041f8c9  mov        esp, ebp
0041f8cb  pop        ebp
0041f8cc  ret        4
