
// block 0041e020
0041e020  push       ebp
0041e021  mov        ebp, esp
0041e023  and        esp, 0xfffffff8
0041e026  sub        esp, 8
0041e029  test       dword ptr [edi + 0x6d18], 0x200
0041e033  push       ebx
0041e034  push       esi
0041e035  je         0x41e042

// block 0041e037
0041e037  lea        esi, [edi + 0x6d1c]
0041e03d  call       0x464a80

// block 0041e042
0041e042  test       byte ptr [edi + 0x6d18], 0x10
0041e049  je         0x41e0d4

// block 0041e04f
0041e04f  inc        dword ptr [edi + 0x6d4c]
0041e055  cmp        dword ptr [edi + 0x6d4c], 0xb4
0041e05f  jne        0x41e09a

// block 0041e061
0041e061  push       0x44
0041e063  call       0x46c9ea

// block 0041e068
0041e068  mov        esi, eax
0041e06a  add        esp, 4
0041e06d  test       esi, esi
0041e06f  je         0x41e09a

// block 0041e071
0041e071  and        dword ptr [esi + 0x40], 0xfffffffe
0041e075  push       0x44
0041e077  push       0
0041e079  push       esi
0041e07a  call       0x477420

// block 0041e07f
0041e07f  or         dword ptr [esi], 2
0041e082  add        esp, 0xc
0041e085  push       0x46
0041e087  push       0
0041e089  push       0
0041e08b  push       0
0041e08d  push       0xc8
0041e092  push       5
0041e094  push       esi
0041e095  call       0x452a60

// block 0041e09a
0041e09a  cmp        dword ptr [edi + 0x6d4c], 0x17c
0041e0a4  jl         0x41e0d4

// block 0041e0a6
0041e0a6  mov        eax, dword ptr [0x4b44e8]
0041e0ab  cmp        dword ptr [eax + 0x74], 0
0041e0af  je         0x41e0b8

// block 0041e0b1
0041e0b1  call       0x432850

// block 0041e0b6
0041e0b6  jmp        0x41e0d4

// block 0041e0b8
0041e0b8  mov        ecx, dword ptr [0x4cee78]
0041e0be  and        ecx, 0x2000
0041e0c4  neg        ecx
0041e0c6  sbb        ecx, ecx
0041e0c8  and        ecx, 0xfffffff3
0041e0cb  add        ecx, 0xf
0041e0ce  mov        dword ptr [0x4cee40], ecx

// block 0041e0d4
0041e0d4  xor        esi, esi
0041e0d6  cmp        dword ptr [edi + 0x6cb8], esi
0041e0dc  je         0x41e100

// block 0041e0de
0041e0de  mov        edx, dword ptr [edi + 0x6cb4]
0041e0e4  push       edx
0041e0e5  mov        edx, dword ptr [0x4ce8cc]
0041e0eb  call       0x461920

// block 0041e0f0
0041e0f0  cmp        eax, esi
0041e0f2  jne        0x41e100

// block 0041e0f4
0041e0f4  mov        dword ptr [edi + 0x6cb4], esi
0041e0fa  mov        dword ptr [edi + 0x6cb8], esi

// block 0041e100
0041e100  lea        esi, [edi + 0x10]
0041e103  mov        ebx, 8

// block 0041e108
0041e108  push       esi
0041e109  call       0x455630

// block 0041e10e
0041e10e  add        esi, 0x4b4
0041e114  sub        ebx, 1
0041e117  jne        0x41e108

// block 0041e119
0041e119  lea        esi, [edi + 0x25b0]
0041e11f  mov        ebx, 8

// block 0041e124
0041e124  push       esi
0041e125  call       0x455630

// block 0041e12a
0041e12a  add        esi, 0x4b4
0041e130  sub        ebx, 1
0041e133  jne        0x41e124

// block 0041e135
0041e135  lea        esi, [edi + 0x4b50]
0041e13b  mov        ebx, 2

// block 0041e140
0041e140  push       esi
0041e141  call       0x455630

// block 0041e146
0041e146  add        esi, 0x4b4
0041e14c  sub        ebx, 1
0041e14f  jne        0x41e140

// block 0041e151
0041e151  lea        esi, [edi + 0x54b8]
0041e157  mov        ebx, 4
0041e15c  lea        esp, [esp]

// block 0041e160
0041e160  push       esi
0041e161  call       0x455630

// block 0041e166
0041e166  add        esi, 0x4b4
0041e16c  sub        ebx, 1
0041e16f  jne        0x41e160

// block 0041e171
0041e171  test       byte ptr [edi + 0x6d18], 1
0041e178  fld        dword ptr [0x4a4050]
0041e17e  mov        edx, dword ptr [0x4b4514]
0041e184  jne        0x41e1fa

// block 0041e186
0041e186  test       edx, edx
0041e188  je         0x41e269

// block 0041e18e
0041e18e  fld        dword ptr [edx + 0x980]
0041e194  fcomp      qword ptr [0x4a3e80]
0041e19a  fnstsw     ax
0041e19c  test       ah, 0x41
0041e19f  jne        0x41e269

// block 0041e1a5
0041e1a5  fcomp      dword ptr [edx + 0x97c]
0041e1ab  fnstsw     ax
0041e1ad  test       ah, 0x41
0041e1b0  jne        0x41e26b

// block 0041e1b6
0041e1b6  lea        esi, [edi + 0x594c]
0041e1bc  mov        ebx, 4

// block 0041e1c1
0041e1c1  mov        eax, dword ptr [esi]
0041e1c3  test       eax, eax
0041e1c5  je         0x41e1d4

// block 0041e1c7
0041e1c7  lea        ecx, [esi - 0x494]
0041e1cd  mov        edx, 3
0041e1d2  call       eax

// block 0041e1d4
0041e1d4  mov        eax, 3
0041e1d9  mov        word ptr [esi - 0xd0], ax
0041e1e0  add        esi, 0x4b4
0041e1e6  sub        ebx, 1
0041e1e9  jne        0x41e1c1

// block 0041e1eb
0041e1eb  or         dword ptr [edi + 0x6d18], 1
0041e1f2  mov        edx, dword ptr [0x4b4514]
0041e1f8  jmp        0x41e26b

// block 0041e1fa
0041e1fa  test       edx, edx
0041e1fc  je         0x41e269

// block 0041e1fe
0041e1fe  fld        dword ptr [edx + 0x980]
0041e204  fcomp      qword ptr [0x4a4460]
0041e20a  fnstsw     ax
0041e20c  test       ah, 5
0041e20f  jnp        0x41e220

// block 0041e211
0041e211  fcomp      dword ptr [edx + 0x97c]
0041e217  fnstsw     ax
0041e219  test       ah, 5
0041e21c  jp         0x41e26b

// block 0041e21e
0041e21e  jmp        0x41e222

// block 0041e220
0041e220  fstp       st(0)

// block 0041e222
0041e222  lea        esi, [edi + 0x594c]
0041e228  mov        ebx, 4
0041e22d  lea        ecx, [ecx]

// block 0041e230
0041e230  mov        eax, dword ptr [esi]
0041e232  test       eax, eax
0041e234  je         0x41e243

// block 0041e236
0041e236  lea        ecx, [esi - 0x494]
0041e23c  mov        edx, 2
0041e241  call       eax

// block 0041e243
0041e243  mov        ecx, 2
0041e248  mov        word ptr [esi - 0xd0], cx
0041e24f  add        esi, 0x4b4
0041e255  sub        ebx, 1
0041e258  jne        0x41e230

// block 0041e25a
0041e25a  and        dword ptr [edi + 0x6d18], 0xfffffffe
0041e261  mov        edx, dword ptr [0x4b4514]
0041e267  jmp        0x41e26b

// block 0041e269
0041e269  fstp       st(0)

// block 0041e26b
0041e26b  mov        ecx, dword ptr [0x4b43dc]
0041e271  test       ecx, ecx
0041e273  je         0x41e6e3

// block 0041e279
0041e279  mov        eax, dword ptr [edi + 0x6d38]
0041e27f  test       eax, eax
0041e281  jl         0x41e439

// block 0041e287
0041e287  cmp        dword ptr [ecx + 0x1c], 0
0041e28b  je         0x41e439

// block 0041e291
0041e291  test       byte ptr [ecx + 0x3c], 1
0041e295  jne        0x41e439

// block 0041e29b
0041e29b  cmp        dword ptr [edi + 0x6d30], 0
0041e2a2  jne        0x41e439

// block 0041e2a8
0041e2a8  mov        ecx, dword ptr [edi + 0x6d40]
0041e2ae  cmp        eax, ecx
0041e2b0  jge        0x41e369

// block 0041e2b6
0041e2b6  cmp        eax, 5
0041e2b9  jge        0x41e30d

// block 0041e2bb
0041e2bb  mov        eax, dword ptr [edi + 0x4fe4]
0041e2c1  lea        esi, [edi + 0x4b50]
0041e2c7  test       eax, eax
0041e2c9  je         0x41e2d4

// block 0041e2cb
0041e2cb  mov        edx, 9
0041e2d0  mov        ecx, esi
0041e2d2  call       eax

// block 0041e2d4
0041e2d4  mov        edx, 9
0041e2d9  mov        word ptr [esi + 0x3c4], dx
0041e2e0  mov        eax, dword ptr [edi + 0x5498]
0041e2e6  lea        esi, [edi + 0x5004]
0041e2ec  test       eax, eax
0041e2ee  je         0x41e2f4

// block 0041e2f0
0041e2f0  mov        ecx, esi
0041e2f2  call       eax

// block 0041e2f4
0041e2f4  mov        eax, 9
0041e2f9  lea        edx, [eax + 3]
0041e2fc  mov        word ptr [esi + 0x3c4], ax
0041e303  call       0x453d90

// block 0041e308
0041e308  jmp        0x41e3b5

// block 0041e30d
0041e30d  cmp        eax, 0xa
0041e310  jge        0x41e3b5

// block 0041e316
0041e316  mov        eax, dword ptr [edi + 0x4fe4]
0041e31c  lea        esi, [edi + 0x4b50]
0041e322  test       eax, eax
0041e324  je         0x41e32f

// block 0041e326
0041e326  mov        edx, 8
0041e32b  mov        ecx, esi
0041e32d  call       eax

// block 0041e32f
0041e32f  mov        ecx, 8
0041e334  mov        word ptr [esi + 0x3c4], cx
0041e33b  mov        eax, dword ptr [edi + 0x5498]
0041e341  lea        esi, [edi + 0x5004]
0041e347  test       eax, eax
0041e349  je         0x41e351

// block 0041e34b
0041e34b  mov        edx, ecx
0041e34d  mov        ecx, esi
0041e34f  call       eax

// block 0041e351
0041e351  mov        edx, 8
0041e356  mov        word ptr [esi + 0x3c4], dx
0041e35d  mov        edx, 0xb
0041e362  call       0x453d90

// block 0041e367
0041e367  jmp        0x41e3b5

// block 0041e369
0041e369  jle        0x41e3b5

// block 0041e36b
0041e36b  mov        eax, dword ptr [edi + 0x4fe4]
0041e371  lea        esi, [edi + 0x4b50]
0041e377  test       eax, eax
0041e379  je         0x41e384

// block 0041e37b
0041e37b  mov        edx, 7
0041e380  mov        ecx, esi
0041e382  call       eax

// block 0041e384
0041e384  mov        eax, 7
0041e389  mov        word ptr [esi + 0x3c4], ax
0041e390  mov        eax, dword ptr [edi + 0x5498]
0041e396  lea        esi, [edi + 0x5004]
0041e39c  test       eax, eax
0041e39e  je         0x41e3a9

// block 0041e3a0
0041e3a0  mov        edx, 7
0041e3a5  mov        ecx, esi
0041e3a7  call       eax

// block 0041e3a9
0041e3a9  mov        ecx, 7
0041e3ae  mov        word ptr [esi + 0x3c4], cx

// block 0041e3b5
0041e3b5  mov        ecx, dword ptr [edi + 0x6d38]
0041e3bb  cmp        ecx, dword ptr [edi + 0x6d40]
0041e3c1  je         0x41e421

// block 0041e3c3
0041e3c3  mov        ebx, dword ptr [edi + 0x4f48]
0041e3c9  lea        esi, [edi + 0x4b50]
0041e3cf  test       ebx, ebx
0041e3d1  je         0x41e3f2

// block 0041e3d3
0041e3d3  mov        eax, 0x66666667
0041e3d8  imul       ecx
0041e3da  sar        edx, 2
0041e3dd  mov        eax, edx
0041e3df  shr        eax, 0x1f
0041e3e2  lea        ecx, [edx + eax + 0xf3]
0041e3e9  mov        eax, esi
0041e3eb  mov        edx, ebx
0041e3ed  call       0x454b80

// block 0041e3f2
0041e3f2  mov        ebx, dword ptr [edi + 0x53fc]
0041e3f8  lea        esi, [edi + 0x5004]
0041e3fe  test       ebx, ebx
0041e400  je         0x41e421

// block 0041e402
0041e402  mov        eax, dword ptr [edi + 0x6d38]
0041e408  cdq        
0041e409  mov        ecx, 0xa
0041e40e  idiv       ecx
0041e410  mov        eax, esi
0041e412  mov        ecx, edx
0041e414  add        ecx, 0xf3
0041e41a  mov        edx, ebx
0041e41c  call       0x454b80

// block 0041e421
0041e421  mov        edx, dword ptr [edi + 0x6d38]
0041e427  mov        ecx, dword ptr [0x4b43dc]
0041e42d  mov        dword ptr [edi + 0x6d40], edx
0041e433  mov        edx, dword ptr [0x4b4514]

// block 0041e439
0041e439  test       ecx, ecx
0041e43b  je         0x41e6e3

// block 0041e441
0041e441  mov        eax, dword ptr [ecx + 0x1c]
0041e444  test       eax, eax
0041e446  je         0x41e6e3

// block 0041e44c
0041e44c  test       byte ptr [ecx + 0x3c], 1
0041e450  jne        0x41e6e3

// block 0041e456
0041e456  cmp        dword ptr [edi + 0x6d30], 0
0041e45d  jne        0x41e6e3

// block 0041e463
0041e463  mov        ecx, dword ptr [eax + 0x2648]
0041e469  mov        dword ptr [esp + 8], ecx
0041e46d  fild       dword ptr [esp + 8]
0041e471  mov        dword ptr [edi + 0x6cf0], ecx
0041e477  fidiv      dword ptr [eax + 0x264c]
0041e47d  fstp       dword ptr [esp + 8]
0041e481  fld        dword ptr [esp + 8]
0041e485  fst        dword ptr [edi + 0x6cec]
0041e48b  fld        dword ptr [edi + 0x6ce8]
0041e491  fcompp     
0041e493  fnstsw     ax
0041e495  test       ah, 5
0041e498  jp         0x41e4ac

// block 0041e49a
0041e49a  fld        dword ptr [edi + 0x6ce8]
0041e4a0  fadd       qword ptr [0x4a4458]
0041e4a6  fstp       dword ptr [edi + 0x6ce8]

// block 0041e4ac
0041e4ac  fld        dword ptr [edi + 0x6ce8]
0041e4b2  fld        dword ptr [edi + 0x6cec]
0041e4b8  fcompp     
0041e4ba  fnstsw     ax
0041e4bc  test       ah, 5
0041e4bf  jp         0x41e4cd

// block 0041e4c1
0041e4c1  fld        dword ptr [edi + 0x6cec]
0041e4c7  fstp       dword ptr [edi + 0x6ce8]

// block 0041e4cd
0041e4cd  test       byte ptr [edi + 0x6d18], 8
0041e4d4  je         0x41e549

// block 0041e4d6
0041e4d6  fld        dword ptr [0x4a3f00]
0041e4dc  fcomp      dword ptr [edx + 0x980]
0041e4e2  fnstsw     ax
0041e4e4  test       ah, 0x41
0041e4e7  jnp        0x41e4fc

// block 0041e4e9
0041e4e9  fldz       
0041e4eb  fcomp      dword ptr [edx + 0x97c]
0041e4f1  fnstsw     ax
0041e4f3  test       ah, 5
0041e4f6  jp         0x41e5ba

// block 0041e4fc
0041e4fc  xor        esi, esi
0041e4fe  cmp        dword ptr [edi + 0x6cf4], esi
0041e504  jle        0x41e52f

// block 0041e506
0041e506  lea        eax, [edi + 0x6c7c]
0041e50c  mov        dword ptr [esp + 8], eax

// block 0041e510
0041e510  mov        ecx, dword ptr [esp + 8]
0041e514  mov        edx, dword ptr [ecx]
0041e516  push       edx
0041e517  mov        ebx, 2
0041e51c  call       0x461970

// block 0041e521
0041e521  add        dword ptr [esp + 8], 4
0041e526  inc        esi
0041e527  cmp        esi, dword ptr [edi + 0x6cf4]
0041e52d  jl         0x41e510

// block 0041e52f
0041e52f  mov        eax, dword ptr [edi + 0x6c78]
0041e535  push       eax
0041e536  mov        ebx, 2
0041e53b  call       0x461970

// block 0041e540
0041e540  and        dword ptr [edi + 0x6d18], 0xfffffff7
0041e547  jmp        0x41e5ba

// block 0041e549
0041e549  fld        dword ptr [0x4a3e04]
0041e54f  fcomp      dword ptr [edx + 0x980]
0041e555  fnstsw     ax
0041e557  test       ah, 1
0041e55a  jne        0x41e5ba

// block 0041e55c
0041e55c  fld        dword ptr [0x4a4050]
0041e562  fcomp      dword ptr [edx + 0x97c]
0041e568  fnstsw     ax
0041e56a  test       ah, 0x41
0041e56d  jne        0x41e5ba

// block 0041e56f
0041e56f  xor        esi, esi
0041e571  cmp        dword ptr [edi + 0x6cf4], esi
0041e577  jle        0x41e5a2

// block 0041e579
0041e579  lea        ecx, [edi + 0x6c7c]
0041e57f  mov        dword ptr [esp + 8], ecx

// block 0041e583
0041e583  mov        edx, dword ptr [esp + 8]
0041e587  mov        eax, dword ptr [edx]
0041e589  push       eax
0041e58a  mov        ebx, 3
0041e58f  call       0x461970

// block 0041e594
0041e594  add        dword ptr [esp + 8], 4
0041e599  inc        esi
0041e59a  cmp        esi, dword ptr [edi + 0x6cf4]
0041e5a0  jl         0x41e583

// block 0041e5a2
0041e5a2  mov        ecx, dword ptr [edi + 0x6c78]
0041e5a8  push       ecx
0041e5a9  mov        ebx, 3
0041e5ae  call       0x461970

// block 0041e5b3
0041e5b3  or         dword ptr [edi + 0x6d18], 8

// block 0041e5ba
0041e5ba  cmp        dword ptr [edi + 0x6c78], 0
0041e5c1  jne        0x41e679

// block 0041e5c7
0041e5c7  mov        ecx, dword ptr [0x4b0cb0]
0041e5cd  dec        ecx
0041e5ce  mov        eax, 0x76
0041e5d3  cmp        ecx, 6
0041e5d6  ja         0x41e654

// block 0041e5d8
0041e5d8  jmp        dword ptr [ecx*4 + 0x41ef8c]

// block 0041e654
0041e654  push       0
0041e656  push       eax
0041e657  mov        eax, dword ptr [edi + 0x6d44]
0041e65d  lea        edx, [esp + 0x10]
0041e661  push       edx
0041e662  push       eax
0041e663  mov        eax, 0x17
0041e668  xor        ecx, ecx
0041e66a  call       0x4615a0

// block 0041e66f
0041e66f  mov        ecx, dword ptr [esp + 8]
0041e673  mov        dword ptr [edi + 0x6c78], ecx

// block 0041e679
0041e679  xor        ebx, ebx
0041e67b  mov        dword ptr [esp + 8], ebx
0041e67f  lea        esi, [edi + 0x6c7c]

// block 0041e685
0041e685  cmp        ebx, dword ptr [edi + 0x6cf4]
0041e68b  jge        0x41e6b8

// block 0041e68d
0041e68d  cmp        dword ptr [esi], 0
0041e690  jne        0x41e6d4

// block 0041e692
0041e692  mov        ecx, dword ptr [edi + 0x6d44]
0041e698  push       0
0041e69a  lea        edx, [ebx + 0x35]
0041e69d  push       edx
0041e69e  lea        eax, [esp + 0x14]
0041e6a2  push       eax
0041e6a3  push       ecx
0041e6a4  mov        eax, 0x17
0041e6a9  xor        ecx, ecx
0041e6ab  call       0x4615a0

// block 0041e6b0
0041e6b0  mov        edx, dword ptr [esp + 0xc]
0041e6b4  mov        dword ptr [esi], edx
0041e6b6  jmp        0x41e6d4

// block 0041e6b8
0041e6b8  cmp        dword ptr [esi], 0
0041e6bb  je         0x41e6d4

// block 0041e6bd
0041e6bd  mov        eax, dword ptr [esi]
0041e6bf  push       eax
0041e6c0  mov        ebx, 1
0041e6c5  call       0x461970

// block 0041e6ca
0041e6ca  mov        ebx, dword ptr [esp + 8]
0041e6ce  mov        dword ptr [esi], 0

// block 0041e6d4
0041e6d4  inc        ebx
0041e6d5  add        esi, 4
0041e6d8  mov        dword ptr [esp + 8], ebx
0041e6dc  cmp        ebx, 0xa
0041e6df  jb         0x41e685

// block 0041e6e1
0041e6e1  jmp        0x41e727

// block 0041e6e3
0041e6e3  cmp        dword ptr [edi + 0x6c78], 0
0041e6ea  je         0x41e707

// block 0041e6ec
0041e6ec  mov        ecx, dword ptr [edi + 0x6c78]
0041e6f2  push       ecx
0041e6f3  mov        ebx, 1
0041e6f8  call       0x461970

// block 0041e6fd
0041e6fd  mov        dword ptr [edi + 0x6c78], 0

// block 0041e707
0041e707  fldz       
0041e709  fst        dword ptr [edi + 0x6ce8]
0041e70f  fst        dword ptr [edi + 0x6cf8]
0041e715  fst        dword ptr [edi + 0x6d00]
0041e71b  fst        dword ptr [edi + 0x6d08]
0041e721  fstp       dword ptr [edi + 0x6d10]

// block 0041e727
0041e727  mov        esi, dword ptr [edi + 0x6d30]
0041e72d  test       esi, esi
0041e72f  je         0x41ebb5

// block 0041e735
0041e735  push       esi
0041e736  call       0x41fcc0

// block 0041e73b
0041e73b  test       eax, eax
0041e73d  jne        0x41e798

// block 0041e73f
0041e73f  mov        edx, dword ptr [esi + 8]
0041e742  mov        ecx, dword ptr [esi + 0x10]
0041e745  mov        dword ptr [esi + 4], edx
0041e748  fld        dword ptr [ecx]
0041e74a  fcomp      qword ptr [0x4a3db0]
0041e750  fnstsw     ax
0041e752  test       ah, 0x41
0041e755  jne        0x41e77b

// block 0041e757
0041e757  fld        dword ptr [0x4a3dac]
0041e75d  fcomp      dword ptr [ecx]
0041e75f  fnstsw     ax
0041e761  test       ah, 0x41
0041e764  jne        0x41e77b

// block 0041e766
0041e766  fld        dword ptr [esi + 0xc]
0041e769  inc        edx
0041e76a  fadd       qword ptr [0x4a3ce0]
0041e770  mov        dword ptr [esi + 8], edx
0041e773  fstp       dword ptr [esi + 0xc]
0041e776  jmp        0x41ebb5

// block 0041e77b
0041e77b  fld        dword ptr [ecx]
0041e77d  fadd       dword ptr [esi + 0xc]
0041e780  fstp       dword ptr [esp + 0xc]
0041e784  fld        dword ptr [esp + 0xc]
0041e788  fst        dword ptr [esi + 0xc]
0041e78b  call       0x4931e0

// block 0041e790
0041e790  mov        dword ptr [esi + 8], eax
0041e793  jmp        0x41ebb5

// block 0041e798
0041e798  mov        eax, dword ptr [edi + 0x6d30]
0041e79e  xor        esi, esi
0041e7a0  mov        dword ptr [esp + 8], eax
0041e7a4  cmp        eax, esi
0041e7a6  je         0x41ebaf

// block 0041e7ac
0041e7ac  mov        ecx, dword ptr [eax + 0x40]
0041e7af  mov        edx, 0x10000000
0041e7b4  cmp        ecx, esi
0041e7b6  je         0x41e81f

// block 0041e7b8
0041e7b8  mov        eax, dword ptr [0x4ce8cc]
0041e7bd  mov        eax, dword ptr [eax + 0x8856b8]
0041e7c3  cmp        eax, esi
0041e7c5  je         0x41e7d4

// block 0041e7c7
0041e7c7  mov        ebx, dword ptr [eax]
0041e7c9  cmp        dword ptr [ebx], ecx
0041e7cb  je         0x41e7f2

// block 0041e7cd
0041e7cd  mov        eax, dword ptr [eax + 4]
0041e7d0  cmp        eax, esi
0041e7d2  jne        0x41e7c7

// block 0041e7d4
0041e7d4  mov        eax, dword ptr [0x4ce8cc]
0041e7d9  mov        eax, dword ptr [eax + 0x8856c0]
0041e7df  cmp        eax, esi
0041e7e1  je         0x41e81f

// block 0041e7e3
0041e7e3  mov        ebx, dword ptr [eax]
0041e7e5  cmp        dword ptr [ebx], ecx
0041e7e7  je         0x41e7f2

// block 0041e7e9
0041e7e9  mov        eax, dword ptr [eax + 4]
0041e7ec  cmp        eax, esi
0041e7ee  jne        0x41e7e3

// block 0041e7f0
0041e7f0  jmp        0x41e81f

// block 0041e7f2
0041e7f2  mov        eax, ebx
0041e7f4  cmp        eax, esi
0041e7f6  je         0x41e81f

// block 0041e7f8
0041e7f8  or         dword ptr [eax + 0x47c], edx
0041e7fe  cmp        dword ptr [eax + 0x18], esi
0041e801  jne        0x41e81f

// block 0041e803
0041e803  mov        eax, dword ptr [eax + 0x14]
0041e806  cmp        eax, esi
0041e808  je         0x41e81f

// block 0041e80a
0041e80a  lea        ebx, [ebx]

// block 0041e81f
0041e81f  mov        eax, dword ptr [esp + 8]
0041e823  mov        dword ptr [eax + 0x40], esi
0041e826  mov        ecx, dword ptr [eax + 0x44]
0041e829  cmp        ecx, esi
0041e82b  je         0x41e89f

// block 0041e82d
0041e82d  mov        eax, dword ptr [0x4ce8cc]
0041e832  mov        eax, dword ptr [eax + 0x8856b8]
0041e838  cmp        eax, esi
0041e83a  je         0x41e84d

// block 0041e83c
0041e83c  lea        esp, [esp]

// block 0041e840
0041e840  mov        ebx, dword ptr [eax]
0041e842  cmp        dword ptr [ebx], ecx
0041e844  je         0x41e86f

// block 0041e846
0041e846  mov        eax, dword ptr [eax + 4]
0041e849  cmp        eax, esi
0041e84b  jne        0x41e840

// block 0041e84d
0041e84d  mov        eax, dword ptr [0x4ce8cc]
0041e852  mov        eax, dword ptr [eax + 0x8856c0]
0041e858  cmp        eax, esi
0041e85a  je         0x41e89f

// block 0041e85c
0041e85c  lea        esp, [esp]

// block 0041e860
0041e860  mov        ebx, dword ptr [eax]
0041e862  cmp        dword ptr [ebx], ecx
0041e864  je         0x41e86f

// block 0041e866
0041e866  mov        eax, dword ptr [eax + 4]
0041e869  cmp        eax, esi
0041e86b  jne        0x41e860

// block 0041e86d
0041e86d  jmp        0x41e89f

// block 0041e86f
0041e86f  mov        eax, ebx
0041e871  cmp        eax, esi
0041e873  je         0x41e89f

// block 0041e875
0041e875  or         dword ptr [eax + 0x47c], edx
0041e87b  cmp        dword ptr [eax + 0x18], esi
0041e87e  jne        0x41e89f

// block 0041e880
0041e880  mov        eax, dword ptr [eax + 0x14]
0041e883  cmp        eax, esi
0041e885  je         0x41e89f

// block 0041e887
0041e887  jmp        0x41e890

// block 0041e890
0041e890  mov        ecx, dword ptr [eax]
0041e892  or         dword ptr [ecx + 0x47c], edx
0041e898  mov        eax, dword ptr [eax + 4]
0041e89b  cmp        eax, esi
0041e89d  jne        0x41e890

// block 0041e89f
0041e89f  mov        eax, dword ptr [esp + 8]
0041e8a3  mov        dword ptr [eax + 0x44], esi
0041e8a6  mov        ecx, dword ptr [eax + 0x48]
0041e8a9  cmp        ecx, esi
0041e8ab  je         0x41e91f

// block 0041e8ad
0041e8ad  mov        eax, dword ptr [0x4ce8cc]
0041e8b2  mov        eax, dword ptr [eax + 0x8856b8]
0041e8b8  cmp        eax, esi
0041e8ba  je         0x41e8cd

// block 0041e8bc
0041e8bc  lea        esp, [esp]

// block 0041e8c0
0041e8c0  mov        ebx, dword ptr [eax]
0041e8c2  cmp        dword ptr [ebx], ecx
0041e8c4  je         0x41e8ef

// block 0041e8c6
0041e8c6  mov        eax, dword ptr [eax + 4]
0041e8c9  cmp        eax, esi
0041e8cb  jne        0x41e8c0

// block 0041e8cd
0041e8cd  mov        eax, dword ptr [0x4ce8cc]
0041e8d2  mov        eax, dword ptr [eax + 0x8856c0]
0041e8d8  cmp        eax, esi
0041e8da  je         0x41e91f

// block 0041e8dc
0041e8dc  lea        esp, [esp]

// block 0041e8e0
0041e8e0  mov        ebx, dword ptr [eax]
0041e8e2  cmp        dword ptr [ebx], ecx
0041e8e4  je         0x41e8ef

// block 0041e8e6
0041e8e6  mov        eax, dword ptr [eax + 4]
0041e8e9  cmp        eax, esi
0041e8eb  jne        0x41e8e0

// block 0041e8ed
0041e8ed  jmp        0x41e91f

// block 0041e8ef
0041e8ef  mov        eax, ebx
0041e8f1  cmp        eax, esi
0041e8f3  je         0x41e91f

// block 0041e8f5
0041e8f5  or         dword ptr [eax + 0x47c], edx
0041e8fb  cmp        dword ptr [eax + 0x18], esi
0041e8fe  jne        0x41e91f

// block 0041e900
0041e900  mov        eax, dword ptr [eax + 0x14]
0041e903  cmp        eax, esi
0041e905  je         0x41e91f

// block 0041e907
0041e907  jmp        0x41e910

// block 0041e910
0041e910  mov        ecx, dword ptr [eax]
0041e912  or         dword ptr [ecx + 0x47c], edx
0041e918  mov        eax, dword ptr [eax + 4]
0041e91b  cmp        eax, esi
0041e91d  jne        0x41e910

// block 0041e91f
0041e91f  mov        eax, dword ptr [esp + 8]
0041e923  mov        dword ptr [eax + 0x48], esi
0041e926  mov        ecx, dword ptr [eax + 0x4c]
0041e929  cmp        ecx, esi
0041e92b  je         0x41e99f

// block 0041e92d
0041e92d  mov        eax, dword ptr [0x4ce8cc]
0041e932  mov        eax, dword ptr [eax + 0x8856b8]
0041e938  cmp        eax, esi
0041e93a  je         0x41e94d

// block 0041e93c
0041e93c  lea        esp, [esp]

// block 0041e940
0041e940  mov        ebx, dword ptr [eax]
0041e942  cmp        dword ptr [ebx], ecx
0041e944  je         0x41e96f

// block 0041e946
0041e946  mov        eax, dword ptr [eax + 4]
0041e949  cmp        eax, esi
0041e94b  jne        0x41e940

// block 0041e94d
0041e94d  mov        eax, dword ptr [0x4ce8cc]
0041e952  mov        eax, dword ptr [eax + 0x8856c0]
0041e958  cmp        eax, esi
0041e95a  je         0x41e99f

// block 0041e95c
0041e95c  lea        esp, [esp]

// block 0041e960
0041e960  mov        ebx, dword ptr [eax]
0041e962  cmp        dword ptr [ebx], ecx
0041e964  je         0x41e96f

// block 0041e966
0041e966  mov        eax, dword ptr [eax + 4]
0041e969  cmp        eax, esi
0041e96b  jne        0x41e960

// block 0041e96d
0041e96d  jmp        0x41e99f

// block 0041e96f
0041e96f  mov        eax, ebx
0041e971  cmp        eax, esi
0041e973  je         0x41e99f

// block 0041e975
0041e975  or         dword ptr [eax + 0x47c], edx
0041e97b  cmp        dword ptr [eax + 0x18], esi
0041e97e  jne        0x41e99f

// block 0041e980
0041e980  mov        eax, dword ptr [eax + 0x14]
0041e983  cmp        eax, esi
0041e985  je         0x41e99f

// block 0041e987
0041e987  jmp        0x41e990

// block 0041e990
0041e990  mov        ecx, dword ptr [eax]
0041e992  or         dword ptr [ecx + 0x47c], edx
0041e998  mov        eax, dword ptr [eax + 4]
0041e99b  cmp        eax, esi
0041e99d  jne        0x41e990

// block 0041e99f
0041e99f  mov        eax, dword ptr [esp + 8]
0041e9a3  mov        dword ptr [eax + 0x4c], esi
0041e9a6  mov        ecx, dword ptr [eax + 0x50]
0041e9a9  cmp        ecx, esi
0041e9ab  je         0x41ea1f

// block 0041e9ad
0041e9ad  mov        eax, dword ptr [0x4ce8cc]
0041e9b2  mov        eax, dword ptr [eax + 0x8856b8]
0041e9b8  cmp        eax, esi
0041e9ba  je         0x41e9cd

// block 0041e9bc
0041e9bc  lea        esp, [esp]

// block 0041e9c0
0041e9c0  mov        ebx, dword ptr [eax]
0041e9c2  cmp        dword ptr [ebx], ecx
0041e9c4  je         0x41e9ef

// block 0041e9c6
0041e9c6  mov        eax, dword ptr [eax + 4]
0041e9c9  cmp        eax, esi
0041e9cb  jne        0x41e9c0

// block 0041e9cd
0041e9cd  mov        eax, dword ptr [0x4ce8cc]
0041e9d2  mov        eax, dword ptr [eax + 0x8856c0]
0041e9d8  cmp        eax, esi
0041e9da  je         0x41ea1f

// block 0041e9dc
0041e9dc  lea        esp, [esp]

// block 0041e9e0
0041e9e0  mov        ebx, dword ptr [eax]
0041e9e2  cmp        dword ptr [ebx], ecx
0041e9e4  je         0x41e9ef

// block 0041e9e6
0041e9e6  mov        eax, dword ptr [eax + 4]
0041e9e9  cmp        eax, esi
0041e9eb  jne        0x41e9e0

// block 0041e9ed
0041e9ed  jmp        0x41ea1f

// block 0041e9ef
0041e9ef  mov        eax, ebx
0041e9f1  cmp        eax, esi
0041e9f3  je         0x41ea1f

// block 0041e9f5
0041e9f5  or         dword ptr [eax + 0x47c], edx
0041e9fb  cmp        dword ptr [eax + 0x18], esi
0041e9fe  jne        0x41ea1f

// block 0041ea00
0041ea00  mov        eax, dword ptr [eax + 0x14]
0041ea03  cmp        eax, esi
0041ea05  je         0x41ea1f

// block 0041ea07
0041ea07  jmp        0x41ea10

// block 0041ea10
0041ea10  mov        ecx, dword ptr [eax]
0041ea12  or         dword ptr [ecx + 0x47c], edx
0041ea18  mov        eax, dword ptr [eax + 4]
0041ea1b  cmp        eax, esi
0041ea1d  jne        0x41ea10

// block 0041ea1f
0041ea1f  mov        eax, dword ptr [esp + 8]
0041ea23  mov        dword ptr [eax + 0x50], esi
0041ea26  mov        ecx, dword ptr [eax + 0x54]
0041ea29  cmp        ecx, esi
0041ea2b  je         0x41ea9f

// block 0041ea2d
0041ea2d  mov        eax, dword ptr [0x4ce8cc]
0041ea32  mov        eax, dword ptr [eax + 0x8856b8]
0041ea38  cmp        eax, esi
0041ea3a  je         0x41ea4d

// block 0041ea3c
0041ea3c  lea        esp, [esp]

// block 0041ea40
0041ea40  mov        ebx, dword ptr [eax]
0041ea42  cmp        dword ptr [ebx], ecx
0041ea44  je         0x41ea6f

// block 0041ea46
0041ea46  mov        eax, dword ptr [eax + 4]
0041ea49  cmp        eax, esi
0041ea4b  jne        0x41ea40

// block 0041ea4d
0041ea4d  mov        eax, dword ptr [0x4ce8cc]
0041ea52  mov        eax, dword ptr [eax + 0x8856c0]
0041ea58  cmp        eax, esi
0041ea5a  je         0x41ea9f

// block 0041ea5c
0041ea5c  lea        esp, [esp]

// block 0041ea60
0041ea60  mov        ebx, dword ptr [eax]
0041ea62  cmp        dword ptr [ebx], ecx
0041ea64  je         0x41ea6f

// block 0041ea66
0041ea66  mov        eax, dword ptr [eax + 4]
0041ea69  cmp        eax, esi
0041ea6b  jne        0x41ea60

// block 0041ea6d
0041ea6d  jmp        0x41ea9f

// block 0041ea6f
0041ea6f  mov        eax, ebx
0041ea71  cmp        eax, esi
0041ea73  je         0x41ea9f

// block 0041ea75
0041ea75  or         dword ptr [eax + 0x47c], edx
0041ea7b  cmp        dword ptr [eax + 0x18], esi
0041ea7e  jne        0x41ea9f

// block 0041ea80
0041ea80  mov        eax, dword ptr [eax + 0x14]
0041ea83  cmp        eax, esi
0041ea85  je         0x41ea9f

// block 0041ea87
0041ea87  jmp        0x41ea90

// block 0041ea90
0041ea90  mov        ecx, dword ptr [eax]
0041ea92  or         dword ptr [ecx + 0x47c], edx
0041ea98  mov        eax, dword ptr [eax + 4]
0041ea9b  cmp        eax, esi
0041ea9d  jne        0x41ea90

// block 0041ea9f
0041ea9f  mov        eax, dword ptr [esp + 8]
0041eaa3  mov        dword ptr [eax + 0x54], esi
0041eaa6  mov        ecx, dword ptr [eax + 0x58]
0041eaa9  cmp        ecx, esi
0041eaab  je         0x41eb1f

// block 0041eaad
0041eaad  mov        eax, dword ptr [0x4ce8cc]
0041eab2  mov        eax, dword ptr [eax + 0x8856b8]
0041eab8  cmp        eax, esi
0041eaba  je         0x41eacd

// block 0041eabc
0041eabc  lea        esp, [esp]

// block 0041eac0
0041eac0  mov        ebx, dword ptr [eax]
0041eac2  cmp        dword ptr [ebx], ecx
0041eac4  je         0x41eaef

// block 0041eac6
0041eac6  mov        eax, dword ptr [eax + 4]
0041eac9  cmp        eax, esi
0041eacb  jne        0x41eac0

// block 0041eacd
0041eacd  mov        eax, dword ptr [0x4ce8cc]
0041ead2  mov        eax, dword ptr [eax + 0x8856c0]
0041ead8  cmp        eax, esi
0041eada  je         0x41eb1f

// block 0041eadc
0041eadc  lea        esp, [esp]

// block 0041eae0
0041eae0  mov        ebx, dword ptr [eax]
0041eae2  cmp        dword ptr [ebx], ecx
0041eae4  je         0x41eaef

// block 0041eae6
0041eae6  mov        eax, dword ptr [eax + 4]
0041eae9  cmp        eax, esi
0041eaeb  jne        0x41eae0

// block 0041eaed
0041eaed  jmp        0x41eb1f

// block 0041eaef
0041eaef  mov        eax, ebx
0041eaf1  cmp        eax, esi
0041eaf3  je         0x41eb1f

// block 0041eaf5
0041eaf5  or         dword ptr [eax + 0x47c], edx
0041eafb  cmp        dword ptr [eax + 0x18], esi
0041eafe  jne        0x41eb1f

// block 0041eb00
0041eb00  mov        eax, dword ptr [eax + 0x14]
0041eb03  cmp        eax, esi
0041eb05  je         0x41eb1f

// block 0041eb07
0041eb07  jmp        0x41eb10

// block 0041eb10
0041eb10  mov        ecx, dword ptr [eax]
0041eb12  or         dword ptr [ecx + 0x47c], edx
0041eb18  mov        eax, dword ptr [eax + 4]
0041eb1b  cmp        eax, esi
0041eb1d  jne        0x41eb10

// block 0041eb1f
0041eb1f  mov        eax, dword ptr [esp + 8]
0041eb23  mov        dword ptr [eax + 0x58], esi
0041eb26  mov        ecx, dword ptr [eax + 0x5c]
0041eb29  cmp        ecx, esi
0041eb2b  je         0x41eb9f

// block 0041eb2d
0041eb2d  mov        eax, dword ptr [0x4ce8cc]
0041eb32  mov        eax, dword ptr [eax + 0x8856b8]
0041eb38  cmp        eax, esi
0041eb3a  je         0x41eb4d

// block 0041eb3c
0041eb3c  lea        esp, [esp]

// block 0041eb40
0041eb40  mov        ebx, dword ptr [eax]
0041eb42  cmp        dword ptr [ebx], ecx
0041eb44  je         0x41eb6f

// block 0041eb46
0041eb46  mov        eax, dword ptr [eax + 4]
0041eb49  cmp        eax, esi
0041eb4b  jne        0x41eb40

// block 0041eb4d
0041eb4d  mov        eax, dword ptr [0x4ce8cc]
0041eb52  mov        eax, dword ptr [eax + 0x8856c0]
0041eb58  cmp        eax, esi
0041eb5a  je         0x41eb9f

// block 0041eb5c
0041eb5c  lea        esp, [esp]

// block 0041eb60
0041eb60  mov        ebx, dword ptr [eax]
0041eb62  cmp        dword ptr [ebx], ecx
0041eb64  je         0x41eb6f

// block 0041eb66
0041eb66  mov        eax, dword ptr [eax + 4]
0041eb69  cmp        eax, esi
0041eb6b  jne        0x41eb60

// block 0041eb6d
0041eb6d  jmp        0x41eb9f

// block 0041eb6f
0041eb6f  mov        eax, ebx
0041eb71  cmp        eax, esi
0041eb73  je         0x41eb9f

// block 0041eb75
0041eb75  or         dword ptr [eax + 0x47c], edx
0041eb7b  cmp        dword ptr [eax + 0x18], esi
0041eb7e  jne        0x41eb9f

// block 0041eb80
0041eb80  mov        eax, dword ptr [eax + 0x14]
0041eb83  cmp        eax, esi
0041eb85  je         0x41eb9f

// block 0041eb87
0041eb87  jmp        0x41eb90

// block 0041eb90
0041eb90  mov        ecx, dword ptr [eax]
0041eb92  or         dword ptr [ecx + 0x47c], edx
0041eb98  mov        eax, dword ptr [eax + 4]
0041eb9b  cmp        eax, esi
0041eb9d  jne        0x41eb90

// block 0041eb9f
0041eb9f  mov        eax, dword ptr [esp + 8]
0041eba3  push       eax
0041eba4  mov        dword ptr [eax + 0x5c], esi
0041eba7  call       0x46ca4f

// block 0041ebac
0041ebac  add        esp, 4

// block 0041ebaf
0041ebaf  mov        dword ptr [edi + 0x6d30], esi

// block 0041ebb5
0041ebb5  mov        eax, dword ptr [0x4b43dc]
0041ebba  test       eax, eax
0041ebbc  je         0x41ef0c

// block 0041ebc2
0041ebc2  mov        ebx, dword ptr [eax + 0x1c]
0041ebc5  test       ebx, ebx
0041ebc7  je         0x41ef0c

// block 0041ebcd
0041ebcd  mov        eax, dword ptr [ebx + 0x26f8]
0041ebd3  mov        ecx, eax
0041ebd5  shr        ecx, 5
0041ebd8  not        ecx
0041ebda  test       cl, 1
0041ebdd  je         0x41ef0c

// block 0041ebe3
0041ebe3  not        eax
0041ebe5  test       al, 1
0041ebe7  je         0x41ef0c

// block 0041ebed
0041ebed  mov        eax, dword ptr [edi + 0x6d18]
0041ebf3  mov        edx, dword ptr [0x4b43cc]
0041ebf9  shr        eax, 1
0041ebfb  test       byte ptr [edx + 0x7c], 1
0041ebff  je         0x41ed11

// block 0041ec05
0041ec05  and        eax, 3
0041ec08  jne        0x41ec5a

// block 0041ec0a
0041ec0a  cmp        dword ptr [ebx + 0x2650], 0x7d0
0041ec14  jge        0x41ee2f

// block 0041ec1a
0041ec1a  mov        eax, dword ptr [edi + 0x6c20]
0041ec20  lea        esi, [edi + 0x678c]
0041ec26  test       eax, eax
0041ec28  je         0x41ed3f

// block 0041ec2e
0041ec2e  mov        edx, 7
0041ec33  mov        ecx, esi
0041ec35  call       eax

// block 0041ec37
0041ec37  mov        eax, 7
0041ec3c  mov        word ptr [esi + 0x3c4], ax
0041ec43  mov        ecx, dword ptr [edi + 0x6d18]
0041ec49  and        ecx, 0xfffffffb
0041ec4c  or         ecx, 2
0041ec4f  mov        dword ptr [edi + 0x6d18], ecx
0041ec55  jmp        0x41ee2f

// block 0041ec5a
0041ec5a  cmp        eax, 1
0041ec5d  jne        0x41ecaf

// block 0041ec5f
0041ec5f  cmp        dword ptr [ebx + 0x2650], 0x3e8
0041ec69  jge        0x41ee2f

// block 0041ec6f
0041ec6f  mov        eax, dword ptr [edi + 0x6c20]
0041ec75  lea        esi, [edi + 0x678c]
0041ec7b  test       eax, eax
0041ec7d  je         0x41ed90

// block 0041ec83
0041ec83  mov        edx, 8
0041ec88  mov        ecx, esi
0041ec8a  call       eax

// block 0041ec8c
0041ec8c  mov        edx, 8
0041ec91  mov        word ptr [esi + 0x3c4], dx
0041ec98  mov        eax, dword ptr [edi + 0x6d18]
0041ec9e  and        eax, 0xfffffffd
0041eca1  or         eax, 4
0041eca4  mov        dword ptr [edi + 0x6d18], eax
0041ecaa  jmp        0x41ee2f

// block 0041ecaf
0041ecaf  cmp        eax, 2
0041ecb2  jne        0x41ecf9

// block 0041ecb4
0041ecb4  cmp        dword ptr [ebx + 0x2650], 0x190
0041ecbe  jge        0x41ee2f

// block 0041ecc4
0041ecc4  mov        eax, dword ptr [edi + 0x6c20]
0041ecca  lea        esi, [edi + 0x678c]
0041ecd0  test       eax, eax
0041ecd2  je         0x41eddd

// block 0041ecd8
0041ecd8  mov        edx, 9
0041ecdd  mov        ecx, esi
0041ecdf  call       eax

// block 0041ece1
0041ece1  mov        ecx, 9
0041ece6  mov        word ptr [esi + 0x3c4], cx
0041eced  or         dword ptr [edi + 0x6d18], 6
0041ecf4  jmp        0x41ee2f

// block 0041ecf9
0041ecf9  cmp        eax, 3
0041ecfc  jne        0x41ee2f

// block 0041ed02
0041ed02  cmp        dword ptr [ebx + 0x2650], 0x190
0041ed0c  jmp        0x41ee01

// block 0041ed11
0041ed11  and        eax, 3
0041ed14  jne        0x41ed62

// block 0041ed16
0041ed16  cmp        dword ptr [ebx + 0x2650], 0x2bc
0041ed20  jge        0x41ee2f

// block 0041ed26
0041ed26  mov        eax, dword ptr [edi + 0x6c20]
0041ed2c  lea        esi, [edi + 0x678c]
0041ed32  test       eax, eax
0041ed34  je         0x41ed3f

// block 0041ed36
0041ed36  mov        edx, 7
0041ed3b  mov        ecx, esi
0041ed3d  call       eax

// block 0041ed3f
0041ed3f  mov        eax, 7
0041ed44  mov        word ptr [esi + 0x3c4], ax
0041ed4b  mov        ecx, dword ptr [edi + 0x6d18]
0041ed51  and        ecx, 0xfffffffb
0041ed54  or         ecx, 2
0041ed57  mov        dword ptr [edi + 0x6d18], ecx
0041ed5d  jmp        0x41ee2f

// block 0041ed62
0041ed62  cmp        eax, 1
0041ed65  jne        0x41edb3

// block 0041ed67
0041ed67  cmp        dword ptr [ebx + 0x2650], 0x190
0041ed71  jge        0x41ee2f

// block 0041ed77
0041ed77  mov        eax, dword ptr [edi + 0x6c20]
0041ed7d  lea        esi, [edi + 0x678c]
0041ed83  test       eax, eax
0041ed85  je         0x41ed90

// block 0041ed87
0041ed87  mov        edx, 8
0041ed8c  mov        ecx, esi
0041ed8e  call       eax

// block 0041ed90
0041ed90  mov        edx, 8
0041ed95  mov        word ptr [esi + 0x3c4], dx
0041ed9c  mov        eax, dword ptr [edi + 0x6d18]
0041eda2  and        eax, 0xfffffffd
0041eda5  or         eax, 4
0041eda8  mov        dword ptr [edi + 0x6d18], eax
0041edae  jmp        0x41ee2f

// block 0041edb3
0041edb3  cmp        eax, 2
0041edb6  jne        0x41edf2

// block 0041edb8
0041edb8  cmp        dword ptr [ebx + 0x2650], 0xc8
0041edc2  jge        0x41ee2f

// block 0041edc4
0041edc4  mov        eax, dword ptr [edi + 0x6c20]
0041edca  lea        esi, [edi + 0x678c]
0041edd0  test       eax, eax
0041edd2  je         0x41eddd

// block 0041edd4
0041edd4  mov        edx, 9
0041edd9  mov        ecx, esi
0041eddb  call       eax

// block 0041eddd
0041eddd  mov        ecx, 9
0041ede2  mov        word ptr [esi + 0x3c4], cx
0041ede9  or         dword ptr [edi + 0x6d18], 6
0041edf0  jmp        0x41ee2f

// block 0041edf2
0041edf2  cmp        eax, 3
0041edf5  jne        0x41ee2f

// block 0041edf7
0041edf7  cmp        dword ptr [ebx + 0x2650], 0xc8

// block 0041ee01
0041ee01  jle        0x41ee2f

// block 0041ee03
0041ee03  mov        eax, dword ptr [edi + 0x6c20]
0041ee09  lea        esi, [edi + 0x678c]
0041ee0f  test       eax, eax
0041ee11  je         0x41ee1c

// block 0041ee13
0041ee13  mov        edx, 0xa
0041ee18  mov        ecx, esi
0041ee1a  call       eax

// block 0041ee1c
0041ee1c  mov        edx, 0xa
0041ee21  mov        word ptr [esi + 0x3c4], dx
0041ee28  and        dword ptr [edi + 0x6d18], 0xfffffff9

// block 0041ee2f
0041ee2f  lea        eax, [edi + 0x678c]
0041ee35  push       eax
0041ee36  call       0x455630

// block 0041ee3b
0041ee3b  fld        dword ptr [ebx + 0x1074]
0041ee41  fadd       qword ptr [0x4a3d70]
0041ee47  mov        ecx, dword ptr [0x4b4514]
0041ee4d  fadd       qword ptr [0x4a3d28]
0041ee53  fstp       dword ptr [edi + 0x6bbc]
0041ee59  fld        dword ptr [0x4a3ec0]
0041ee5f  fstp       dword ptr [edi + 0x6bc0]
0041ee65  fld        dword ptr [ebx + 0x1074]
0041ee6b  fsub       dword ptr [ecx + 0x97c]
0041ee71  fstp       dword ptr [esp + 0xc]
0041ee75  fld        dword ptr [esp + 0xc]
0041ee79  fabs       
0041ee7b  fstp       dword ptr [esp + 0xc]
0041ee7f  fld        dword ptr [esp + 0xc]
0041ee83  fstp       dword ptr [esp + 0xc]
0041ee87  fld        dword ptr [0x4a3e04]
0041ee8d  fld        dword ptr [esp + 0xc]
0041ee91  fcom       st(1)
0041ee93  fnstsw     ax
0041ee95  fstp       st(1)
0041ee97  test       ah, 1
0041ee9a  jne        0x41eea7

// block 0041ee9c
0041ee9c  fstp       st(0)
0041ee9e  mov        byte ptr [edi + 0x6b4b], 0xff
0041eea5  jmp        0x41eedf

// block 0041eea7
0041eea7  fmul       qword ptr [0x4a4450]
0041eead  fnstcw     word ptr [esp + 8]
0041eeb1  movzx      eax, word ptr [esp + 8]
0041eeb6  fmul       qword ptr [0x4a42a8]
0041eebc  or         eax, 0xc00
0041eec1  mov        dword ptr [esp + 0xc], eax
0041eec5  mov        al, 0x40
0041eec7  fldcw      word ptr [esp + 0xc]
0041eecb  fistp      dword ptr [esp + 0xc]
0041eecf  mov        dl, byte ptr [esp + 0xc]
0041eed3  sub        al, dl
0041eed5  mov        byte ptr [edi + 0x6b4b], al
0041eedb  fldcw      word ptr [esp + 8]

// block 0041eedf
0041eedf  fld        dword ptr [0x4a3d98]
0041eee5  fcomp      dword ptr [ebx + 0x1074]
0041eeeb  fnstsw     ax
0041eeed  test       ah, 0x41
0041eef0  je         0x41ef05

// block 0041eef2
0041eef2  fld        dword ptr [0x4a3d88]
0041eef8  fcomp      dword ptr [ebx + 0x1074]
0041eefe  fnstsw     ax
0041ef00  test       ah, 5
0041ef03  jp         0x41ef0c

// block 0041ef05
0041ef05  mov        byte ptr [edi + 0x6b4b], 0

// block 0041ef0c
0041ef0c  mov        edx, dword ptr [edi + 0x6cc4]
0041ef12  mov        ecx, dword ptr [edi + 0x6ccc]
0041ef18  mov        dword ptr [edi + 0x6cc0], edx
0041ef1e  fld        dword ptr [ecx]
0041ef20  fcomp      qword ptr [0x4a3db0]
0041ef26  fnstsw     ax
0041ef28  test       ah, 0x41
0041ef2b  jne        0x41ef60

// block 0041ef2d
0041ef2d  fld        dword ptr [0x4a3dac]
0041ef33  fcomp      dword ptr [ecx]
0041ef35  fnstsw     ax
0041ef37  test       ah, 0x41
0041ef3a  jne        0x41ef60

// block 0041ef3c
0041ef3c  fld        dword ptr [edi + 0x6cc8]
0041ef42  inc        edx
0041ef43  fadd       qword ptr [0x4a3ce0]
0041ef49  mov        dword ptr [edi + 0x6cc4], edx
0041ef4f  mov        eax, 1
0041ef54  fstp       dword ptr [edi + 0x6cc8]
0041ef5a  pop        esi
0041ef5b  pop        ebx
0041ef5c  mov        esp, ebp
0041ef5e  pop        ebp
0041ef5f  ret        

// block 0041ef60
0041ef60  fld        dword ptr [ecx]
0041ef62  fadd       dword ptr [edi + 0x6cc8]
0041ef68  fstp       dword ptr [esp + 0xc]
0041ef6c  fld        dword ptr [esp + 0xc]
0041ef70  fst        dword ptr [edi + 0x6cc8]
0041ef76  call       0x4931e0

// block 0041ef7b
0041ef7b  pop        esi
0041ef7c  mov        dword ptr [edi + 0x6cc4], eax
0041ef82  mov        eax, 1
0041ef87  pop        ebx
0041ef88  mov        esp, ebp
0041ef8a  pop        ebp
0041ef8b  ret        
