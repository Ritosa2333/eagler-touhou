
// block 0045dfa0
0045dfa0  push       -1
0045dfa2  push       0x497477
0045dfa7  mov        eax, dword ptr fs:[0]
0045dfad  push       eax
0045dfae  push       ebx
0045dfaf  push       ebp
0045dfb0  push       esi
0045dfb1  push       edi
0045dfb2  mov        eax, dword ptr [0x4ad138]
0045dfb7  xor        eax, esp
0045dfb9  push       eax
0045dfba  lea        eax, [esp + 0x14]
0045dfbe  mov        dword ptr fs:[0], eax
0045dfc4  mov        edi, dword ptr [esp + 0x24]
0045dfc8  push       0x4026e0
0045dfcd  push       0x4027e0
0045dfd2  push       0x1000
0045dfd7  push       0x4b4
0045dfdc  lea        esi, [edi + 0xbc]
0045dfe2  push       esi
0045dfe3  call       0x46ce49

// block 0045dfe8
0045dfe8  xor        ebx, ebx
0045dfea  lea        ecx, [edi + 0x4b5180]
0045dff0  mov        dword ptr [esp + 0x1c], ebx
0045dff4  call       0x4027e0

// block 0045dff9
0045dff9  push       0x4026e0
0045dffe  push       0x4027e0
0045e003  push       0x20
0045e005  push       0x4b4
0045e00a  lea        eax, [edi + 0x8856c8]
0045e010  push       eax
0045e011  mov        byte ptr [esp + 0x30], 1
0045e016  call       0x46ce49

// block 0045e01b
0045e01b  push       0x88ed54
0045e020  push       ebx
0045e021  push       edi
0045e022  mov        byte ptr [esp + 0x28], 2
0045e027  call       0x477420

// block 0045e02c
0045e02c  fld1       
0045e02e  fst        dword ptr [0x4d47dc]
0045e034  add        esp, 0xc
0045e037  fst        dword ptr [0x4d47c4]
0045e03d  or         eax, 0xffffffff
0045e040  fst        dword ptr [0x4d47ac]
0045e046  mov        ebp, 0x1000
0045e04b  fst        dword ptr [0x4d4794]
0045e051  fldz       
0045e053  fst        dword ptr [0x4d4798]
0045e059  fst        dword ptr [0x4d479c]
0045e05f  fst        dword ptr [0x4d47b4]
0045e065  fst        dword ptr [0x4d47c8]
0045e06b  fst        dword ptr [0x4d47fc]
0045e071  fst        dword ptr [0x4d4800]
0045e077  fst        dword ptr [0x4d481c]
0045e07d  fstp       dword ptr [0x4d4834]
0045e083  fst        dword ptr [0x4d47b0]
0045e089  fst        dword ptr [0x4d47cc]
0045e08f  fst        dword ptr [0x4d47e0]
0045e095  fst        dword ptr [0x4d47e4]
0045e09b  fst        dword ptr [0x4d4848]
0045e0a1  fst        dword ptr [0x4d482c]
0045e0a7  fst        dword ptr [0x4d4810]
0045e0ad  fst        dword ptr [0x4d47f4]
0045e0b3  fst        dword ptr [0x4d4818]
0045e0b9  fst        dword ptr [0x4d4838]
0045e0bf  fst        dword ptr [0x4d4850]
0045e0c5  fstp       dword ptr [0x4d4854]
0045e0cb  mov        dword ptr [edi + 0x4b564c], ebx
0045e0d1  mov        dword ptr [edi + 0x4b563c], ebx
0045e0d7  mov        byte ptr [edi + 0x4b5640], bl
0045e0dd  mov        byte ptr [edi + 0x4b5641], bl
0045e0e3  mov        dword ptr [edi + 0x4b5638], 1
0045e0ed  mov        byte ptr [edi + 0x4b5642], bl
0045e0f3  mov        byte ptr [edi + 0x4b5644], 0xff
0045e0fa  mov        byte ptr [edi + 0x4b5643], bl
0045e100  mov        dword ptr [edi], eax
0045e102  mov        dword ptr [edi + 0x28], eax
0045e105  mov        dword ptr [edi + 0x50], eax
0045e108  mov        dword ptr [edi + 0x78], eax
0045e10b  jmp        0x45e110

// block 0045e110
0045e110  call       0x402520

// block 0045e115
0045e115  add        esi, 0x4b4
0045e11b  sub        ebp, 1
0045e11e  jne        0x45e110

// block 0045e120
0045e120  push       0x24
0045e122  call       0x46c9ea

// block 0045e127
0045e127  add        esp, 4
0045e12a  cmp        eax, ebx
0045e12c  je         0x45e148

// block 0045e12e
0045e12e  and        dword ptr [eax + 4], 0xfffffffe
0045e132  mov        dword ptr [eax + 8], ebx
0045e135  mov        dword ptr [eax + 0xc], ebx
0045e138  mov        dword ptr [eax + 0x10], ebx
0045e13b  mov        dword ptr [eax], ebx
0045e13d  mov        dword ptr [eax + 0x14], eax
0045e140  mov        dword ptr [eax + 0x18], ebx
0045e143  mov        dword ptr [eax + 0x1c], ebx
0045e146  jmp        0x45e14a

// block 0045e148
0045e148  xor        eax, eax

// block 0045e14a
0045e14a  mov        ebp, 3
0045e14f  or         dword ptr [eax + 4], ebp
0045e152  mov        dword ptr [eax + 0xc], ebx
0045e155  mov        dword ptr [eax + 0x10], ebx
0045e158  lea        ebx, [ebp + 0x18]
0045e15b  mov        esi, eax
0045e15d  mov        dword ptr [eax + 8], 0x460c30
0045e164  mov        dword ptr [eax + 0x20], edi
0045e167  call       0x462380

// block 0045e16c
0045e16c  push       0x24
0045e16e  call       0x46c9ea

// block 0045e173
0045e173  xor        ecx, ecx
0045e175  add        esp, 4
0045e178  cmp        eax, ecx
0045e17a  je         0x45e196

// block 0045e17c
0045e17c  and        dword ptr [eax + 4], 0xfffffffe
0045e180  mov        dword ptr [eax + 8], ecx
0045e183  mov        dword ptr [eax + 0xc], ecx
0045e186  mov        dword ptr [eax + 0x10], ecx
0045e189  mov        dword ptr [eax], ecx
0045e18b  mov        dword ptr [eax + 0x14], eax
0045e18e  mov        dword ptr [eax + 0x18], ecx
0045e191  mov        dword ptr [eax + 0x1c], ecx
0045e194  jmp        0x45e198

// block 0045e196
0045e196  xor        eax, eax

// block 0045e198
0045e198  or         dword ptr [eax + 4], ebp
0045e19b  mov        ebx, 8
0045e1a0  mov        esi, eax
0045e1a2  mov        dword ptr [eax + 8], 0x460c40
0045e1a9  mov        dword ptr [eax + 0xc], ecx
0045e1ac  mov        dword ptr [eax + 0x10], ecx
0045e1af  mov        dword ptr [eax + 0x20], edi
0045e1b2  call       0x462380

// block 0045e1b7
0045e1b7  push       0x24
0045e1b9  call       0x46c9ea

// block 0045e1be
0045e1be  xor        ecx, ecx
0045e1c0  add        esp, 4
0045e1c3  cmp        eax, ecx
0045e1c5  je         0x45e1e1

// block 0045e1c7
0045e1c7  and        dword ptr [eax + 4], 0xfffffffe
0045e1cb  mov        dword ptr [eax + 8], ecx
0045e1ce  mov        dword ptr [eax + 0xc], ecx
0045e1d1  mov        dword ptr [eax + 0x10], ecx
0045e1d4  mov        dword ptr [eax], ecx
0045e1d6  mov        dword ptr [eax + 0x14], eax
0045e1d9  mov        dword ptr [eax + 0x18], ecx
0045e1dc  mov        dword ptr [eax + 0x1c], ecx
0045e1df  jmp        0x45e1e3

// block 0045e1e1
0045e1e1  xor        eax, eax

// block 0045e1e3
0045e1e3  or         dword ptr [eax + 4], ebp
0045e1e6  mov        ebx, 4
0045e1eb  mov        esi, eax
0045e1ed  mov        dword ptr [eax + 8], 0x460c50
0045e1f4  mov        dword ptr [eax + 0xc], ecx
0045e1f7  mov        dword ptr [eax + 0x10], ecx
0045e1fa  mov        dword ptr [eax + 0x20], edi
0045e1fd  call       0x462420

// block 0045e202
0045e202  push       0x24
0045e204  call       0x46c9ea

// block 0045e209
0045e209  xor        ecx, ecx
0045e20b  add        esp, 4
0045e20e  cmp        eax, ecx
0045e210  je         0x45e22c

// block 0045e212
0045e212  and        dword ptr [eax + 4], 0xfffffffe
0045e216  mov        dword ptr [eax + 8], ecx
0045e219  mov        dword ptr [eax + 0xc], ecx
0045e21c  mov        dword ptr [eax + 0x10], ecx
0045e21f  mov        dword ptr [eax], ecx
0045e221  mov        dword ptr [eax + 0x14], eax
0045e224  mov        dword ptr [eax + 0x18], ecx
0045e227  mov        dword ptr [eax + 0x1c], ecx
0045e22a  jmp        0x45e22e

// block 0045e22c
0045e22c  xor        eax, eax

// block 0045e22e
0045e22e  or         dword ptr [eax + 4], ebp
0045e231  mov        ebx, 6
0045e236  mov        esi, eax
0045e238  mov        dword ptr [eax + 8], 0x460c60
0045e23f  mov        dword ptr [eax + 0xc], ecx
0045e242  mov        dword ptr [eax + 0x10], ecx
0045e245  mov        dword ptr [eax + 0x20], edi
0045e248  call       0x462420

// block 0045e24d
0045e24d  push       0x24
0045e24f  call       0x46c9ea

// block 0045e254
0045e254  xor        ecx, ecx
0045e256  add        esp, 4
0045e259  cmp        eax, ecx
0045e25b  je         0x45e277

// block 0045e25d
0045e25d  and        dword ptr [eax + 4], 0xfffffffe
0045e261  mov        dword ptr [eax + 8], ecx
0045e264  mov        dword ptr [eax + 0xc], ecx
0045e267  mov        dword ptr [eax + 0x10], ecx
0045e26a  mov        dword ptr [eax], ecx
0045e26c  mov        dword ptr [eax + 0x14], eax
0045e26f  mov        dword ptr [eax + 0x18], ecx
0045e272  mov        dword ptr [eax + 0x1c], ecx
0045e275  jmp        0x45e279

// block 0045e277
0045e277  xor        eax, eax

// block 0045e279
0045e279  or         dword ptr [eax + 4], ebp
0045e27c  mov        ebx, 8
0045e281  mov        esi, eax
0045e283  mov        dword ptr [eax + 8], 0x460c70
0045e28a  mov        dword ptr [eax + 0xc], ecx
0045e28d  mov        dword ptr [eax + 0x10], ecx
0045e290  mov        dword ptr [eax + 0x20], edi
0045e293  call       0x462420

// block 0045e298
0045e298  push       0x24
0045e29a  call       0x46c9ea

// block 0045e29f
0045e29f  xor        ecx, ecx
0045e2a1  add        esp, 4
0045e2a4  cmp        eax, ecx
0045e2a6  je         0x45e2c2

// block 0045e2a8
0045e2a8  and        dword ptr [eax + 4], 0xfffffffe
0045e2ac  mov        dword ptr [eax + 8], ecx
0045e2af  mov        dword ptr [eax + 0xc], ecx
0045e2b2  mov        dword ptr [eax + 0x10], ecx
0045e2b5  mov        dword ptr [eax], ecx
0045e2b7  mov        dword ptr [eax + 0x14], eax
0045e2ba  mov        dword ptr [eax + 0x18], ecx
0045e2bd  mov        dword ptr [eax + 0x1c], ecx
0045e2c0  jmp        0x45e2c4

// block 0045e2c2
0045e2c2  xor        eax, eax

// block 0045e2c4
0045e2c4  or         dword ptr [eax + 4], ebp
0045e2c7  mov        ebx, 9
0045e2cc  mov        esi, eax
0045e2ce  mov        dword ptr [eax + 8], 0x460c80
0045e2d5  mov        dword ptr [eax + 0xc], ecx
0045e2d8  mov        dword ptr [eax + 0x10], ecx
0045e2db  mov        dword ptr [eax + 0x20], edi
0045e2de  call       0x462420

// block 0045e2e3
0045e2e3  push       0x24
0045e2e5  call       0x46c9ea

// block 0045e2ea
0045e2ea  xor        ecx, ecx
0045e2ec  add        esp, 4
0045e2ef  cmp        eax, ecx
0045e2f1  je         0x45e30d

// block 0045e2f3
0045e2f3  and        dword ptr [eax + 4], 0xfffffffe
0045e2f7  mov        dword ptr [eax + 8], ecx
0045e2fa  mov        dword ptr [eax + 0xc], ecx
0045e2fd  mov        dword ptr [eax + 0x10], ecx
0045e300  mov        dword ptr [eax], ecx
0045e302  mov        dword ptr [eax + 0x14], eax
0045e305  mov        dword ptr [eax + 0x18], ecx
0045e308  mov        dword ptr [eax + 0x1c], ecx
0045e30b  jmp        0x45e30f

// block 0045e30d
0045e30d  xor        eax, eax

// block 0045e30f
0045e30f  or         dword ptr [eax + 4], ebp
0045e312  mov        ebx, 0xb
0045e317  mov        esi, eax
0045e319  mov        dword ptr [eax + 8], 0x460c90
0045e320  mov        dword ptr [eax + 0xc], ecx
0045e323  mov        dword ptr [eax + 0x10], ecx
0045e326  mov        dword ptr [eax + 0x20], edi
0045e329  call       0x462420

// block 0045e32e
0045e32e  push       0x24
0045e330  call       0x46c9ea

// block 0045e335
0045e335  xor        ecx, ecx
0045e337  add        esp, 4
0045e33a  cmp        eax, ecx
0045e33c  je         0x45e358

// block 0045e33e
0045e33e  and        dword ptr [eax + 4], 0xfffffffe
0045e342  mov        dword ptr [eax + 8], ecx
0045e345  mov        dword ptr [eax + 0xc], ecx
0045e348  mov        dword ptr [eax + 0x10], ecx
0045e34b  mov        dword ptr [eax], ecx
0045e34d  mov        dword ptr [eax + 0x14], eax
0045e350  mov        dword ptr [eax + 0x18], ecx
0045e353  mov        dword ptr [eax + 0x1c], ecx
0045e356  jmp        0x45e35a

// block 0045e358
0045e358  xor        eax, eax

// block 0045e35a
0045e35a  or         dword ptr [eax + 4], ebp
0045e35d  mov        ebx, 0xd
0045e362  mov        esi, eax
0045e364  mov        dword ptr [eax + 8], 0x460d10
0045e36b  mov        dword ptr [eax + 0xc], ecx
0045e36e  mov        dword ptr [eax + 0x10], ecx
0045e371  mov        dword ptr [eax + 0x20], edi
0045e374  call       0x462420

// block 0045e379
0045e379  push       0x24
0045e37b  call       0x46c9ea

// block 0045e380
0045e380  xor        ecx, ecx
0045e382  add        esp, 4
0045e385  cmp        eax, ecx
0045e387  je         0x45e3a3

// block 0045e389
0045e389  and        dword ptr [eax + 4], 0xfffffffe
0045e38d  mov        dword ptr [eax + 8], ecx
0045e390  mov        dword ptr [eax + 0xc], ecx
0045e393  mov        dword ptr [eax + 0x10], ecx
0045e396  mov        dword ptr [eax], ecx
0045e398  mov        dword ptr [eax + 0x14], eax
0045e39b  mov        dword ptr [eax + 0x18], ecx
0045e39e  mov        dword ptr [eax + 0x1c], ecx
0045e3a1  jmp        0x45e3a5

// block 0045e3a3
0045e3a3  xor        eax, eax

// block 0045e3a5
0045e3a5  or         dword ptr [eax + 4], ebp
0045e3a8  mov        ebx, 0x10
0045e3ad  mov        esi, eax
0045e3af  mov        dword ptr [eax + 8], 0x460d20
0045e3b6  mov        dword ptr [eax + 0xc], ecx
0045e3b9  mov        dword ptr [eax + 0x10], ecx
0045e3bc  mov        dword ptr [eax + 0x20], edi
0045e3bf  call       0x462420

// block 0045e3c4
0045e3c4  push       0x24
0045e3c6  call       0x46c9ea

// block 0045e3cb
0045e3cb  xor        ecx, ecx
0045e3cd  add        esp, 4
0045e3d0  cmp        eax, ecx
0045e3d2  je         0x45e3ee

// block 0045e3d4
0045e3d4  and        dword ptr [eax + 4], 0xfffffffe
0045e3d8  mov        dword ptr [eax + 8], ecx
0045e3db  mov        dword ptr [eax + 0xc], ecx
0045e3de  mov        dword ptr [eax + 0x10], ecx
0045e3e1  mov        dword ptr [eax], ecx
0045e3e3  mov        dword ptr [eax + 0x14], eax
0045e3e6  mov        dword ptr [eax + 0x18], ecx
0045e3e9  mov        dword ptr [eax + 0x1c], ecx
0045e3ec  jmp        0x45e3f0

// block 0045e3ee
0045e3ee  xor        eax, eax

// block 0045e3f0
0045e3f0  or         dword ptr [eax + 4], ebp
0045e3f3  mov        ebx, 0x11
0045e3f8  mov        esi, eax
0045e3fa  mov        dword ptr [eax + 8], 0x460d30
0045e401  mov        dword ptr [eax + 0xc], ecx
0045e404  mov        dword ptr [eax + 0x10], ecx
0045e407  mov        dword ptr [eax + 0x20], edi
0045e40a  call       0x462420

// block 0045e40f
0045e40f  push       0x24
0045e411  call       0x46c9ea

// block 0045e416
0045e416  xor        ecx, ecx
0045e418  add        esp, 4
0045e41b  cmp        eax, ecx
0045e41d  je         0x45e439

// block 0045e41f
0045e41f  and        dword ptr [eax + 4], 0xfffffffe
0045e423  mov        dword ptr [eax + 8], ecx
0045e426  mov        dword ptr [eax + 0xc], ecx
0045e429  mov        dword ptr [eax + 0x10], ecx
0045e42c  mov        dword ptr [eax], ecx
0045e42e  mov        dword ptr [eax + 0x14], eax
0045e431  mov        dword ptr [eax + 0x18], ecx
0045e434  mov        dword ptr [eax + 0x1c], ecx
0045e437  jmp        0x45e43b

// block 0045e439
0045e439  xor        eax, eax

// block 0045e43b
0045e43b  or         dword ptr [eax + 4], ebp
0045e43e  mov        ebx, 0x12
0045e443  mov        esi, eax
0045e445  mov        dword ptr [eax + 8], 0x460d40
0045e44c  mov        dword ptr [eax + 0xc], ecx
0045e44f  mov        dword ptr [eax + 0x10], ecx
0045e452  mov        dword ptr [eax + 0x20], edi
0045e455  call       0x462420

// block 0045e45a
0045e45a  push       0x24
0045e45c  call       0x46c9ea

// block 0045e461
0045e461  xor        ecx, ecx
0045e463  add        esp, 4
0045e466  cmp        eax, ecx
0045e468  je         0x45e484

// block 0045e46a
0045e46a  and        dword ptr [eax + 4], 0xfffffffe
0045e46e  mov        dword ptr [eax + 8], ecx
0045e471  mov        dword ptr [eax + 0xc], ecx
0045e474  mov        dword ptr [eax + 0x10], ecx
0045e477  mov        dword ptr [eax], ecx
0045e479  mov        dword ptr [eax + 0x14], eax
0045e47c  mov        dword ptr [eax + 0x18], ecx
0045e47f  mov        dword ptr [eax + 0x1c], ecx
0045e482  jmp        0x45e486

// block 0045e484
0045e484  xor        eax, eax

// block 0045e486
0045e486  or         dword ptr [eax + 4], ebp
0045e489  mov        ebx, 0x13
0045e48e  mov        esi, eax
0045e490  mov        dword ptr [eax + 8], 0x460d50
0045e497  mov        dword ptr [eax + 0xc], ecx
0045e49a  mov        dword ptr [eax + 0x10], ecx
0045e49d  mov        dword ptr [eax + 0x20], edi
0045e4a0  call       0x462420

// block 0045e4a5
0045e4a5  push       0x24
0045e4a7  call       0x46c9ea

// block 0045e4ac
0045e4ac  xor        ecx, ecx
0045e4ae  add        esp, 4
0045e4b1  cmp        eax, ecx
0045e4b3  je         0x45e4cf

// block 0045e4b5
0045e4b5  and        dword ptr [eax + 4], 0xfffffffe
0045e4b9  mov        dword ptr [eax + 8], ecx
0045e4bc  mov        dword ptr [eax + 0xc], ecx
0045e4bf  mov        dword ptr [eax + 0x10], ecx
0045e4c2  mov        dword ptr [eax], ecx
0045e4c4  mov        dword ptr [eax + 0x14], eax
0045e4c7  mov        dword ptr [eax + 0x18], ecx
0045e4ca  mov        dword ptr [eax + 0x1c], ecx
0045e4cd  jmp        0x45e4d1

// block 0045e4cf
0045e4cf  xor        eax, eax

// block 0045e4d1
0045e4d1  or         dword ptr [eax + 4], ebp
0045e4d4  mov        ebx, 0x14
0045e4d9  mov        esi, eax
0045e4db  mov        dword ptr [eax + 8], 0x460d60
0045e4e2  mov        dword ptr [eax + 0xc], ecx
0045e4e5  mov        dword ptr [eax + 0x10], ecx
0045e4e8  mov        dword ptr [eax + 0x20], edi
0045e4eb  call       0x462420

// block 0045e4f0
0045e4f0  push       0x24
0045e4f2  call       0x46c9ea

// block 0045e4f7
0045e4f7  xor        ecx, ecx
0045e4f9  add        esp, 4
0045e4fc  cmp        eax, ecx
0045e4fe  je         0x45e51a

// block 0045e500
0045e500  and        dword ptr [eax + 4], 0xfffffffe
0045e504  mov        dword ptr [eax + 8], ecx
0045e507  mov        dword ptr [eax + 0xc], ecx
0045e50a  mov        dword ptr [eax + 0x10], ecx
0045e50d  mov        dword ptr [eax], ecx
0045e50f  mov        dword ptr [eax + 0x14], eax
0045e512  mov        dword ptr [eax + 0x18], ecx
0045e515  mov        dword ptr [eax + 0x1c], ecx
0045e518  jmp        0x45e51c

// block 0045e51a
0045e51a  xor        eax, eax

// block 0045e51c
0045e51c  or         dword ptr [eax + 4], ebp
0045e51f  mov        ebx, 0x17
0045e524  mov        esi, eax
0045e526  mov        dword ptr [eax + 8], 0x460d70
0045e52d  mov        dword ptr [eax + 0xc], ecx
0045e530  mov        dword ptr [eax + 0x10], ecx
0045e533  mov        dword ptr [eax + 0x20], edi
0045e536  call       0x462420

// block 0045e53b
0045e53b  push       0x24
0045e53d  call       0x46c9ea

// block 0045e542
0045e542  xor        ecx, ecx
0045e544  add        esp, 4
0045e547  cmp        eax, ecx
0045e549  je         0x45e565

// block 0045e54b
0045e54b  and        dword ptr [eax + 4], 0xfffffffe
0045e54f  mov        dword ptr [eax + 8], ecx
0045e552  mov        dword ptr [eax + 0xc], ecx
0045e555  mov        dword ptr [eax + 0x10], ecx
0045e558  mov        dword ptr [eax], ecx
0045e55a  mov        dword ptr [eax + 0x14], eax
0045e55d  mov        dword ptr [eax + 0x18], ecx
0045e560  mov        dword ptr [eax + 0x1c], ecx
0045e563  jmp        0x45e567

// block 0045e565
0045e565  xor        eax, eax

// block 0045e567
0045e567  or         dword ptr [eax + 4], ebp
0045e56a  mov        ebx, 0x19
0045e56f  mov        esi, eax
0045e571  mov        dword ptr [eax + 8], 0x460d80
0045e578  mov        dword ptr [eax + 0xc], ecx
0045e57b  mov        dword ptr [eax + 0x10], ecx
0045e57e  mov        dword ptr [eax + 0x20], edi
0045e581  call       0x462420

// block 0045e586
0045e586  push       0x24
0045e588  call       0x46c9ea

// block 0045e58d
0045e58d  xor        ecx, ecx
0045e58f  add        esp, 4
0045e592  cmp        eax, ecx
0045e594  je         0x45e5b0

// block 0045e596
0045e596  and        dword ptr [eax + 4], 0xfffffffe
0045e59a  mov        dword ptr [eax + 8], ecx
0045e59d  mov        dword ptr [eax + 0xc], ecx
0045e5a0  mov        dword ptr [eax + 0x10], ecx
0045e5a3  mov        dword ptr [eax], ecx
0045e5a5  mov        dword ptr [eax + 0x14], eax
0045e5a8  mov        dword ptr [eax + 0x18], ecx
0045e5ab  mov        dword ptr [eax + 0x1c], ecx
0045e5ae  jmp        0x45e5b2

// block 0045e5b0
0045e5b0  xor        eax, eax

// block 0045e5b2
0045e5b2  or         dword ptr [eax + 4], ebp
0045e5b5  mov        ebx, 0x1a
0045e5ba  mov        esi, eax
0045e5bc  mov        dword ptr [eax + 8], 0x460d90
0045e5c3  mov        dword ptr [eax + 0xc], ecx
0045e5c6  mov        dword ptr [eax + 0x10], ecx
0045e5c9  mov        dword ptr [eax + 0x20], edi
0045e5cc  call       0x462420

// block 0045e5d1
0045e5d1  push       0x24
0045e5d3  call       0x46c9ea

// block 0045e5d8
0045e5d8  xor        ecx, ecx
0045e5da  add        esp, 4
0045e5dd  cmp        eax, ecx
0045e5df  je         0x45e5fb

// block 0045e5e1
0045e5e1  and        dword ptr [eax + 4], 0xfffffffe
0045e5e5  mov        dword ptr [eax + 8], ecx
0045e5e8  mov        dword ptr [eax + 0xc], ecx
0045e5eb  mov        dword ptr [eax + 0x10], ecx
0045e5ee  mov        dword ptr [eax], ecx
0045e5f0  mov        dword ptr [eax + 0x14], eax
0045e5f3  mov        dword ptr [eax + 0x18], ecx
0045e5f6  mov        dword ptr [eax + 0x1c], ecx
0045e5f9  jmp        0x45e5fd

// block 0045e5fb
0045e5fb  xor        eax, eax

// block 0045e5fd
0045e5fd  or         dword ptr [eax + 4], ebp
0045e600  mov        ebx, 0x1c
0045e605  mov        esi, eax
0045e607  mov        dword ptr [eax + 8], 0x460da0
0045e60e  mov        dword ptr [eax + 0xc], ecx
0045e611  mov        dword ptr [eax + 0x10], ecx
0045e614  mov        dword ptr [eax + 0x20], edi
0045e617  call       0x462420

// block 0045e61c
0045e61c  push       0x24
0045e61e  call       0x46c9ea

// block 0045e623
0045e623  xor        ecx, ecx
0045e625  add        esp, 4
0045e628  cmp        eax, ecx
0045e62a  je         0x45e646

// block 0045e62c
0045e62c  and        dword ptr [eax + 4], 0xfffffffe
0045e630  mov        dword ptr [eax + 8], ecx
0045e633  mov        dword ptr [eax + 0xc], ecx
0045e636  mov        dword ptr [eax + 0x10], ecx
0045e639  mov        dword ptr [eax], ecx
0045e63b  mov        dword ptr [eax + 0x14], eax
0045e63e  mov        dword ptr [eax + 0x18], ecx
0045e641  mov        dword ptr [eax + 0x1c], ecx
0045e644  jmp        0x45e648

// block 0045e646
0045e646  xor        eax, eax

// block 0045e648
0045e648  or         dword ptr [eax + 4], ebp
0045e64b  mov        ebx, 0x1e
0045e650  mov        esi, eax
0045e652  mov        dword ptr [eax + 8], 0x460db0
0045e659  mov        dword ptr [eax + 0xc], ecx
0045e65c  mov        dword ptr [eax + 0x10], ecx
0045e65f  mov        dword ptr [eax + 0x20], edi
0045e662  call       0x462420

// block 0045e667
0045e667  push       0x24
0045e669  call       0x46c9ea

// block 0045e66e
0045e66e  xor        ecx, ecx
0045e670  add        esp, 4
0045e673  cmp        eax, ecx
0045e675  je         0x45e691

// block 0045e677
0045e677  and        dword ptr [eax + 4], 0xfffffffe
0045e67b  mov        dword ptr [eax + 8], ecx
0045e67e  mov        dword ptr [eax + 0xc], ecx
0045e681  mov        dword ptr [eax + 0x10], ecx
0045e684  mov        dword ptr [eax], ecx
0045e686  mov        dword ptr [eax + 0x14], eax
0045e689  mov        dword ptr [eax + 0x18], ecx
0045e68c  mov        dword ptr [eax + 0x1c], ecx
0045e68f  jmp        0x45e693

// block 0045e691
0045e691  xor        eax, eax

// block 0045e693
0045e693  or         dword ptr [eax + 4], ebp
0045e696  mov        ebx, 0x22
0045e69b  mov        esi, eax
0045e69d  mov        dword ptr [eax + 8], 0x460dc0
0045e6a4  mov        dword ptr [eax + 0xc], ecx
0045e6a7  mov        dword ptr [eax + 0x10], ecx
0045e6aa  mov        dword ptr [eax + 0x20], edi
0045e6ad  call       0x462420

// block 0045e6b2
0045e6b2  push       0x24
0045e6b4  call       0x46c9ea

// block 0045e6b9
0045e6b9  xor        ecx, ecx
0045e6bb  add        esp, 4
0045e6be  cmp        eax, ecx
0045e6c0  je         0x45e6dc

// block 0045e6c2
0045e6c2  and        dword ptr [eax + 4], 0xfffffffe
0045e6c6  mov        dword ptr [eax + 8], ecx
0045e6c9  mov        dword ptr [eax + 0xc], ecx
0045e6cc  mov        dword ptr [eax + 0x10], ecx
0045e6cf  mov        dword ptr [eax], ecx
0045e6d1  mov        dword ptr [eax + 0x14], eax
0045e6d4  mov        dword ptr [eax + 0x18], ecx
0045e6d7  mov        dword ptr [eax + 0x1c], ecx
0045e6da  jmp        0x45e6de

// block 0045e6dc
0045e6dc  xor        eax, eax

// block 0045e6de
0045e6de  or         dword ptr [eax + 4], ebp
0045e6e1  mov        ebx, 0x24
0045e6e6  mov        esi, eax
0045e6e8  mov        dword ptr [eax + 8], 0x460dd0
0045e6ef  mov        dword ptr [eax + 0xc], ecx
0045e6f2  mov        dword ptr [eax + 0x10], ecx
0045e6f5  mov        dword ptr [eax + 0x20], edi
0045e6f8  call       0x462420

// block 0045e6fd
0045e6fd  push       ebx
0045e6fe  call       0x46c9ea

// block 0045e703
0045e703  xor        ecx, ecx
0045e705  add        esp, 4
0045e708  cmp        eax, ecx
0045e70a  je         0x45e726

// block 0045e70c
0045e70c  and        dword ptr [eax + 4], 0xfffffffe
0045e710  mov        dword ptr [eax + 8], ecx
0045e713  mov        dword ptr [eax + 0xc], ecx
0045e716  mov        dword ptr [eax + 0x10], ecx
0045e719  mov        dword ptr [eax], ecx
0045e71b  mov        dword ptr [eax + 0x14], eax
0045e71e  mov        dword ptr [eax + 0x18], ecx
0045e721  mov        dword ptr [eax + 0x1c], ecx
0045e724  jmp        0x45e728

// block 0045e726
0045e726  xor        eax, eax

// block 0045e728
0045e728  or         dword ptr [eax + 4], ebp
0045e72b  mov        ebx, 0x26
0045e730  mov        esi, eax
0045e732  mov        dword ptr [eax + 8], 0x460de0
0045e739  mov        dword ptr [eax + 0xc], ecx
0045e73c  mov        dword ptr [eax + 0x10], ecx
0045e73f  mov        dword ptr [eax + 0x20], edi
0045e742  call       0x462420

// block 0045e747
0045e747  push       0x24
0045e749  call       0x46c9ea

// block 0045e74e
0045e74e  xor        ecx, ecx
0045e750  add        esp, 4
0045e753  cmp        eax, ecx
0045e755  je         0x45e771

// block 0045e757
0045e757  and        dword ptr [eax + 4], 0xfffffffe
0045e75b  mov        dword ptr [eax + 8], ecx
0045e75e  mov        dword ptr [eax + 0xc], ecx
0045e761  mov        dword ptr [eax + 0x10], ecx
0045e764  mov        dword ptr [eax], ecx
0045e766  mov        dword ptr [eax + 0x14], eax
0045e769  mov        dword ptr [eax + 0x18], ecx
0045e76c  mov        dword ptr [eax + 0x1c], ecx
0045e76f  jmp        0x45e773

// block 0045e771
0045e771  xor        eax, eax

// block 0045e773
0045e773  or         dword ptr [eax + 4], ebp
0045e776  mov        ebx, 0x29
0045e77b  mov        esi, eax
0045e77d  mov        dword ptr [eax + 8], 0x460df0
0045e784  mov        dword ptr [eax + 0xc], ecx
0045e787  mov        dword ptr [eax + 0x10], ecx
0045e78a  mov        dword ptr [eax + 0x20], edi
0045e78d  call       0x462420

// block 0045e792
0045e792  push       0x24
0045e794  call       0x46c9ea

// block 0045e799
0045e799  xor        ecx, ecx
0045e79b  add        esp, 4
0045e79e  cmp        eax, ecx
0045e7a0  je         0x45e7bc

// block 0045e7a2
0045e7a2  and        dword ptr [eax + 4], 0xfffffffe
0045e7a6  mov        dword ptr [eax + 8], ecx
0045e7a9  mov        dword ptr [eax + 0xc], ecx
0045e7ac  mov        dword ptr [eax + 0x10], ecx
0045e7af  mov        dword ptr [eax], ecx
0045e7b1  mov        dword ptr [eax + 0x14], eax
0045e7b4  mov        dword ptr [eax + 0x18], ecx
0045e7b7  mov        dword ptr [eax + 0x1c], ecx
0045e7ba  jmp        0x45e7be

// block 0045e7bc
0045e7bc  xor        eax, eax

// block 0045e7be
0045e7be  or         dword ptr [eax + 4], ebp
0045e7c1  mov        ebx, 0x2a
0045e7c6  mov        esi, eax
0045e7c8  mov        dword ptr [eax + 8], 0x460e40
0045e7cf  mov        dword ptr [eax + 0xc], ecx
0045e7d2  mov        dword ptr [eax + 0x10], ecx
0045e7d5  mov        dword ptr [eax + 0x20], edi
0045e7d8  call       0x462420

// block 0045e7dd
0045e7dd  push       0x24
0045e7df  call       0x46c9ea

// block 0045e7e4
0045e7e4  xor        ecx, ecx
0045e7e6  add        esp, 4
0045e7e9  cmp        eax, ecx
0045e7eb  je         0x45e807

// block 0045e7ed
0045e7ed  and        dword ptr [eax + 4], 0xfffffffe
0045e7f1  mov        dword ptr [eax + 8], ecx
0045e7f4  mov        dword ptr [eax + 0xc], ecx
0045e7f7  mov        dword ptr [eax + 0x10], ecx
0045e7fa  mov        dword ptr [eax], ecx
0045e7fc  mov        dword ptr [eax + 0x14], eax
0045e7ff  mov        dword ptr [eax + 0x18], ecx
0045e802  mov        dword ptr [eax + 0x1c], ecx
0045e805  jmp        0x45e809

// block 0045e807
0045e807  xor        eax, eax

// block 0045e809
0045e809  or         dword ptr [eax + 4], ebp
0045e80c  mov        ebx, 0x34
0045e811  mov        esi, eax
0045e813  mov        dword ptr [eax + 8], 0x460e20
0045e81a  mov        dword ptr [eax + 0xc], ecx
0045e81d  mov        dword ptr [eax + 0x10], ecx
0045e820  mov        dword ptr [eax + 0x20], edi
0045e823  call       0x462420

// block 0045e828
0045e828  push       0x24
0045e82a  call       0x46c9ea

// block 0045e82f
0045e82f  xor        ecx, ecx
0045e831  add        esp, 4
0045e834  cmp        eax, ecx
0045e836  je         0x45e852

// block 0045e838
0045e838  and        dword ptr [eax + 4], 0xfffffffe
0045e83c  mov        dword ptr [eax + 8], ecx
0045e83f  mov        dword ptr [eax + 0xc], ecx
0045e842  mov        dword ptr [eax + 0x10], ecx
0045e845  mov        dword ptr [eax], ecx
0045e847  mov        dword ptr [eax + 0x14], eax
0045e84a  mov        dword ptr [eax + 0x18], ecx
0045e84d  mov        dword ptr [eax + 0x1c], ecx
0045e850  jmp        0x45e854

// block 0045e852
0045e852  xor        eax, eax

// block 0045e854
0045e854  or         dword ptr [eax + 4], ebp
0045e857  mov        ebx, 0x3f
0045e85c  mov        esi, eax
0045e85e  mov        dword ptr [eax + 8], 0x460e10
0045e865  mov        dword ptr [eax + 0xc], ecx
0045e868  mov        dword ptr [eax + 0x10], ecx
0045e86b  mov        dword ptr [eax + 0x20], edi
0045e86e  call       0x462420

// block 0045e873
0045e873  push       0x24
0045e875  call       0x46c9ea

// block 0045e87a
0045e87a  xor        ecx, ecx
0045e87c  add        esp, 4
0045e87f  cmp        eax, ecx
0045e881  je         0x45e89d

// block 0045e883
0045e883  and        dword ptr [eax + 4], 0xfffffffe
0045e887  mov        dword ptr [eax + 8], ecx
0045e88a  mov        dword ptr [eax + 0xc], ecx
0045e88d  mov        dword ptr [eax + 0x10], ecx
0045e890  mov        dword ptr [eax], ecx
0045e892  mov        dword ptr [eax + 0x14], eax
0045e895  mov        dword ptr [eax + 0x18], ecx
0045e898  mov        dword ptr [eax + 0x1c], ecx
0045e89b  jmp        0x45e89f

// block 0045e89d
0045e89d  xor        eax, eax

// block 0045e89f
0045e89f  or         dword ptr [eax + 4], ebp
0045e8a2  mov        ebx, 0x40
0045e8a7  mov        esi, eax
0045e8a9  mov        dword ptr [eax + 8], 0x460e00
0045e8b0  mov        dword ptr [eax + 0xc], ecx
0045e8b3  mov        dword ptr [eax + 0x10], ecx
0045e8b6  mov        dword ptr [eax + 0x20], edi
0045e8b9  call       0x462420

// block 0045e8be
0045e8be  push       0x24
0045e8c0  call       0x46c9ea

// block 0045e8c5
0045e8c5  xor        ecx, ecx
0045e8c7  add        esp, 4
0045e8ca  cmp        eax, ecx
0045e8cc  je         0x45e8e8

// block 0045e8ce
0045e8ce  and        dword ptr [eax + 4], 0xfffffffe
0045e8d2  mov        dword ptr [eax + 8], ecx
0045e8d5  mov        dword ptr [eax + 0xc], ecx
0045e8d8  mov        dword ptr [eax + 0x10], ecx
0045e8db  mov        dword ptr [eax], ecx
0045e8dd  mov        dword ptr [eax + 0x14], eax
0045e8e0  mov        dword ptr [eax + 0x18], ecx
0045e8e3  mov        dword ptr [eax + 0x1c], ecx
0045e8e6  jmp        0x45e8ea

// block 0045e8e8
0045e8e8  xor        eax, eax

// block 0045e8ea
0045e8ea  or         dword ptr [eax + 4], ebp
0045e8ed  mov        ebx, 0x33
0045e8f2  mov        esi, eax
0045e8f4  mov        dword ptr [eax + 8], 0x460e30
0045e8fb  mov        dword ptr [eax + 0xc], ecx
0045e8fe  mov        dword ptr [eax + 0x10], ecx
0045e901  mov        dword ptr [eax + 0x20], edi
0045e904  call       0x462420

// block 0045e909
0045e909  push       0x24
0045e90b  call       0x46c9ea

// block 0045e910
0045e910  xor        ecx, ecx
0045e912  add        esp, 4
0045e915  cmp        eax, ecx
0045e917  je         0x45e933

// block 0045e919
0045e919  and        dword ptr [eax + 4], 0xfffffffe
0045e91d  mov        dword ptr [eax + 8], ecx
0045e920  mov        dword ptr [eax + 0xc], ecx
0045e923  mov        dword ptr [eax + 0x10], ecx
0045e926  mov        dword ptr [eax], ecx
0045e928  mov        dword ptr [eax + 0x14], eax
0045e92b  mov        dword ptr [eax + 0x18], ecx
0045e92e  mov        dword ptr [eax + 0x1c], ecx
0045e931  jmp        0x45e935

// block 0045e933
0045e933  xor        eax, eax

// block 0045e935
0045e935  or         dword ptr [eax + 4], ebp
0045e938  mov        ebx, 0x32
0045e93d  mov        esi, eax
0045e93f  mov        dword ptr [eax + 8], 0x460e70
0045e946  mov        dword ptr [eax + 0xc], ecx
0045e949  mov        dword ptr [eax + 0x10], ecx
0045e94c  mov        dword ptr [eax + 0x20], edi
0045e94f  call       0x462420

// block 0045e954
0045e954  push       0x24
0045e956  call       0x46c9ea

// block 0045e95b
0045e95b  xor        ecx, ecx
0045e95d  add        esp, 4
0045e960  cmp        eax, ecx
0045e962  je         0x45e97e

// block 0045e964
0045e964  and        dword ptr [eax + 4], 0xfffffffe
0045e968  mov        dword ptr [eax + 8], ecx
0045e96b  mov        dword ptr [eax + 0xc], ecx
0045e96e  mov        dword ptr [eax + 0x10], ecx
0045e971  mov        dword ptr [eax], ecx
0045e973  mov        dword ptr [eax + 0x14], eax
0045e976  mov        dword ptr [eax + 0x18], ecx
0045e979  mov        dword ptr [eax + 0x1c], ecx
0045e97c  jmp        0x45e980

// block 0045e97e
0045e97e  xor        eax, eax

// block 0045e980
0045e980  or         dword ptr [eax + 4], ebp
0045e983  mov        ebx, 0x42
0045e988  mov        esi, eax
0045e98a  mov        dword ptr [eax + 8], 0x460f00
0045e991  mov        dword ptr [eax + 0xc], ecx
0045e994  mov        dword ptr [eax + 0x10], ecx
0045e997  mov        dword ptr [eax + 0x20], edi
0045e99a  call       0x462420

// block 0045e99f
0045e99f  push       0x24
0045e9a1  call       0x46c9ea

// block 0045e9a6
0045e9a6  xor        ecx, ecx
0045e9a8  add        esp, 4
0045e9ab  cmp        eax, ecx
0045e9ad  je         0x45e9c9

// block 0045e9af
0045e9af  and        dword ptr [eax + 4], 0xfffffffe
0045e9b3  mov        dword ptr [eax + 8], ecx
0045e9b6  mov        dword ptr [eax + 0xc], ecx
0045e9b9  mov        dword ptr [eax + 0x10], ecx
0045e9bc  mov        dword ptr [eax], ecx
0045e9be  mov        dword ptr [eax + 0x14], eax
0045e9c1  mov        dword ptr [eax + 0x18], ecx
0045e9c4  mov        dword ptr [eax + 0x1c], ecx
0045e9c7  jmp        0x45e9cb

// block 0045e9c9
0045e9c9  xor        eax, eax

// block 0045e9cb
0045e9cb  or         dword ptr [eax + 4], ebp
0045e9ce  mov        ebx, 0x41
0045e9d3  mov        esi, eax
0045e9d5  mov        dword ptr [eax + 8], 0x460f50
0045e9dc  mov        dword ptr [eax + 0xc], ecx
0045e9df  mov        dword ptr [eax + 0x10], ecx
0045e9e2  mov        dword ptr [eax + 0x20], edi
0045e9e5  call       0x462420

// block 0045e9ea
0045e9ea  mov        eax, dword ptr [0x4ce8f0]
0045e9ef  mov        ecx, dword ptr [eax]
0045e9f1  mov        edx, dword ptr [ecx + 0x170]
0045e9f7  push       0
0045e9f9  push       eax
0045e9fa  call       edx

// block 0045e9fc
0045e9fc  mov        eax, edi
0045e9fe  mov        ecx, dword ptr [esp + 0x14]
0045ea02  mov        dword ptr fs:[0], ecx
0045ea09  pop        ecx
0045ea0a  pop        edi
0045ea0b  pop        esi
0045ea0c  pop        ebp
0045ea0d  pop        ebx
0045ea0e  add        esp, 0xc
0045ea11  ret        4
