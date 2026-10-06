
// block 00459e50
00459e50  fld        dword ptr [0x4d47e8]
00459e56  sub        esp, 0xc
00459e59  push       ebx
00459e5a  push       ebp
00459e5b  push       esi
00459e5c  mov        esi, ecx
00459e5e  fadd       dword ptr [esi + 0xb0]
00459e64  mov        bl, byte ptr [esp + 0x1c]
00459e68  push       edi
00459e69  mov        edi, eax
00459e6b  fstp       dword ptr [0x4d47e8]
00459e71  fld        dword ptr [esi + 0xb4]
00459e77  fadd       dword ptr [0x4d47ec]
00459e7d  fstp       dword ptr [0x4d47ec]
00459e83  fld        dword ptr [0x4d4804]
00459e89  fadd       dword ptr [esi + 0xb0]
00459e8f  fstp       dword ptr [0x4d4804]
00459e95  fld        dword ptr [esi + 0xb4]
00459e9b  fadd       dword ptr [0x4d4808]
00459ea1  fstp       dword ptr [0x4d4808]
00459ea7  fld        dword ptr [0x4d4820]
00459ead  fadd       dword ptr [esi + 0xb0]
00459eb3  fstp       dword ptr [0x4d4820]
00459eb9  fld        dword ptr [esi + 0xb4]
00459ebf  fadd       dword ptr [0x4d4824]
00459ec5  fstp       dword ptr [0x4d4824]
00459ecb  fld        dword ptr [0x4d483c]
00459ed1  fadd       dword ptr [esi + 0xb0]
00459ed7  fstp       dword ptr [0x4d483c]
00459edd  fld        dword ptr [esi + 0xb4]
00459ee3  fadd       dword ptr [0x4d4840]
00459ee9  fstp       dword ptr [0x4d4840]
00459eef  test       bl, 1
00459ef2  je         0x459f5c

// block 00459ef4
00459ef4  fld        dword ptr [0x4d47e8]
00459efa  frndint    
00459efc  fsub       dword ptr [0x4a3238]
00459f02  fld        dword ptr [0x4d4804]
00459f08  frndint    
00459f0a  fsub       dword ptr [0x4a3238]
00459f10  fld        dword ptr [0x4d47ec]
00459f16  frndint    
00459f18  fsub       dword ptr [0x4a3238]
00459f1e  fld        dword ptr [0x4d4824]
00459f24  frndint    
00459f26  fsub       dword ptr [0x4a3238]
00459f2c  fst        dword ptr [0x4d4824]
00459f32  fstp       dword ptr [0x4d4840]
00459f38  fst        dword ptr [0x4d47ec]
00459f3e  fstp       dword ptr [0x4d4808]
00459f44  fst        dword ptr [0x4d4804]
00459f4a  fstp       dword ptr [0x4d483c]
00459f50  fst        dword ptr [0x4d47e8]
00459f56  fstp       dword ptr [0x4d4820]

// block 00459f5c
00459f5c  mov        eax, dword ptr [0x4d47e8]
00459f61  fld        dword ptr [edi + 0x7c]
00459f64  fadd       dword ptr [edi + 0x60]
00459f67  mov        dword ptr [edi + 0x448], eax
00459f6d  mov        ecx, dword ptr [0x4d47ec]
00459f73  mov        dword ptr [edi + 0x44c], ecx
00459f79  mov        edx, dword ptr [0x4d47f0]
00459f7f  mov        dword ptr [edi + 0x450], edx
00459f85  mov        eax, dword ptr [0x4d4804]
00459f8a  mov        dword ptr [edi + 0x454], eax
00459f90  mov        ecx, dword ptr [0x4d4808]
00459f96  mov        dword ptr [edi + 0x458], ecx
00459f9c  mov        edx, dword ptr [0x4d480c]
00459fa2  mov        dword ptr [edi + 0x45c], edx
00459fa8  mov        eax, dword ptr [0x4d4820]
00459fad  mov        dword ptr [edi + 0x460], eax
00459fb3  mov        ecx, dword ptr [0x4d4824]
00459fb9  mov        dword ptr [edi + 0x464], ecx
00459fbf  mov        edx, dword ptr [0x4d4828]
00459fc5  mov        dword ptr [edi + 0x468], edx
00459fcb  mov        eax, dword ptr [0x4d483c]
00459fd0  mov        dword ptr [edi + 0x46c], eax
00459fd6  mov        ecx, dword ptr [0x4d4840]
00459fdc  mov        dword ptr [edi + 0x470], ecx
00459fe2  mov        edx, dword ptr [0x4d4844]
00459fe8  mov        dword ptr [edi + 0x474], edx
00459fee  fstp       dword ptr [0x4d47fc]
00459ff4  fld        dword ptr [edi + 0x80]
00459ffa  fadd       dword ptr [edi + 0x64]
00459ffd  fstp       dword ptr [0x4d4800]
0045a003  fld        dword ptr [edi + 0x84]
0045a009  fsub       dword ptr [edi + 0x7c]
0045a00c  fmul       dword ptr [edi + 0x50]
0045a00f  fld        dword ptr [edi + 0x7c]
0045a012  fadd       dword ptr [edi + 0x60]
0045a015  faddp      st(1)
0045a017  fstp       dword ptr [0x4d4818]
0045a01d  fld        dword ptr [edi + 0x88]
0045a023  fadd       dword ptr [edi + 0x64]
0045a026  fstp       dword ptr [0x4d481c]
0045a02c  fld        dword ptr [edi + 0x60]
0045a02f  fadd       dword ptr [edi + 0x8c]
0045a035  fstp       dword ptr [0x4d4834]
0045a03b  fld        dword ptr [edi + 0x90]
0045a041  fsub       dword ptr [edi + 0x80]
0045a047  fmul       dword ptr [edi + 0x54]
0045a04a  fld        dword ptr [edi + 0x80]
0045a050  fadd       dword ptr [edi + 0x64]
0045a053  faddp      st(1)
0045a055  fstp       dword ptr [0x4d4838]
0045a05b  fld        dword ptr [edi + 0x94]
0045a061  fsub       dword ptr [edi + 0x8c]
0045a067  fmul       dword ptr [edi + 0x50]
0045a06a  fld        dword ptr [edi + 0x60]
0045a06d  fadd       dword ptr [edi + 0x8c]
0045a073  faddp      st(1)
0045a075  fstp       dword ptr [0x4d4850]
0045a07b  fld        dword ptr [edi + 0x98]

// block 0045a081
0045a081  fsub       dword ptr [edi + 0x88]
0045a087  fmul       dword ptr [edi + 0x54]
0045a08a  fld        dword ptr [edi + 0x88]
0045a090  fadd       dword ptr [edi + 0x64]
0045a093  faddp      st(1)
0045a095  fstp       dword ptr [0x4d4854]
0045a09b  fld        dword ptr [0x4d47e8]
0045a0a1  fld        dword ptr [0x4d4804]
0045a0a7  fcom       st(1)
0045a0a9  fnstsw     ax
0045a0ab  test       ah, 5
0045a0ae  jp         0x45a0b4

// block 0045a0b0
0045a0b0  fstp       st(0)
0045a0b2  jmp        0x45a0b6

// block 0045a0b4
0045a0b4  fstp       st(1)

// block 0045a0b6
0045a0b6  fstp       dword ptr [esp + 0x20]
0045a0ba  fld        dword ptr [0x4d4820]
0045a0c0  fld        dword ptr [esp + 0x20]
0045a0c4  fcomp      st(1)
0045a0c6  fnstsw     ax
0045a0c8  test       ah, 5
0045a0cb  jp         0x45a0d1

// block 0045a0cd
0045a0cd  fst        dword ptr [esp + 0x20]

// block 0045a0d1
0045a0d1  fld        dword ptr [0x4d483c]
0045a0d7  fld        dword ptr [esp + 0x20]
0045a0db  fcomp      st(1)
0045a0dd  fnstsw     ax
0045a0df  test       ah, 5
0045a0e2  jp         0x45a0e8

// block 0045a0e4
0045a0e4  fst        dword ptr [esp + 0x20]

// block 0045a0e8
0045a0e8  fld        dword ptr [0x4d47ec]
0045a0ee  fld        dword ptr [0x4d4808]
0045a0f4  fcom       st(1)
0045a0f6  fnstsw     ax
0045a0f8  test       ah, 5
0045a0fb  jp         0x45a105

// block 0045a0fd
0045a0fd  fxch       st(1)
0045a0ff  fst        dword ptr [esp + 0x10]
0045a103  jmp        0x45a10b

// block 0045a105
0045a105  fst        dword ptr [esp + 0x10]
0045a109  fxch       st(1)

// block 0045a10b
0045a10b  fld        dword ptr [0x4d4824]
0045a111  fld        dword ptr [esp + 0x10]
0045a115  fcomp      st(1)
0045a117  fnstsw     ax
0045a119  test       ah, 5
0045a11c  jp         0x45a122

// block 0045a11e
0045a11e  fst        dword ptr [esp + 0x10]

// block 0045a122
0045a122  fld        dword ptr [0x4d4840]
0045a128  fld        dword ptr [esp + 0x10]
0045a12c  fcomp      st(1)
0045a12e  fnstsw     ax
0045a130  test       ah, 5
0045a133  jp         0x45a139

// block 0045a135
0045a135  fst        dword ptr [esp + 0x10]

// block 0045a139
0045a139  fld        dword ptr [0x4d47e8]
0045a13f  fld        dword ptr [0x4d4804]
0045a145  fcompp     
0045a147  fnstsw     ax
0045a149  test       ah, 0x41
0045a14c  jne        0x45a156

// block 0045a14e
0045a14e  fld        dword ptr [0x4d47e8]
0045a154  jmp        0x45a15c

// block 0045a156
0045a156  fld        dword ptr [0x4d4804]

// block 0045a15c
0045a15c  fstp       dword ptr [esp + 0x14]
0045a160  fld        dword ptr [esp + 0x14]
0045a164  fcomp      st(6)
0045a166  fnstsw     ax
0045a168  test       ah, 0x41
0045a16b  jne        0x45a175

// block 0045a16d
0045a16d  fxch       st(5)
0045a16f  fstp       dword ptr [esp + 0x14]
0045a173  jmp        0x45a177

// block 0045a175
0045a175  fstp       st(5)

// block 0045a177
0045a177  fld        dword ptr [esp + 0x14]
0045a17b  fcomp      st(4)
0045a17d  fnstsw     ax
0045a17f  test       ah, 0x41
0045a182  jne        0x45a18c

// block 0045a184
0045a184  fxch       st(3)
0045a186  fstp       dword ptr [esp + 0x14]
0045a18a  jmp        0x45a18e

// block 0045a18c
0045a18c  fstp       st(3)

// block 0045a18e
0045a18e  fcom       st(1)
0045a190  fnstsw     ax
0045a192  test       ah, 5
0045a195  jp         0x45a19b

// block 0045a197
0045a197  fstp       st(1)
0045a199  jmp        0x45a19d

// block 0045a19b
0045a19b  fstp       st(0)

// block 0045a19d
0045a19d  fstp       dword ptr [esp + 0x18]
0045a1a1  fld        dword ptr [esp + 0x18]
0045a1a5  fcomp      st(1)
0045a1a7  fnstsw     ax
0045a1a9  test       ah, 0x41
0045a1ac  jne        0x45a1b4

// block 0045a1ae
0045a1ae  fstp       dword ptr [esp + 0x18]
0045a1b2  jmp        0x45a1b6

// block 0045a1b4
0045a1b4  fstp       st(0)

// block 0045a1b6
0045a1b6  fld        dword ptr [esp + 0x18]
0045a1ba  fcomp      st(1)
0045a1bc  fnstsw     ax
0045a1be  test       ah, 0x41
0045a1c1  jne        0x45a1c9

// block 0045a1c3
0045a1c3  fstp       dword ptr [esp + 0x18]
0045a1c7  jmp        0x45a1cb

// block 0045a1c9
0045a1c9  fstp       st(0)

// block 0045a1cb
0045a1cb  mov        ecx, dword ptr [0x4cee34]
0045a1d1  fld        dword ptr [esp + 0x20]
0045a1d5  mov        ebp, dword ptr [ecx + 0xcc]
0045a1db  mov        eax, ebp
0045a1dd  mov        dword ptr [esp + 0x20], eax
0045a1e1  fild       dword ptr [esp + 0x20]
0045a1e5  test       eax, eax
0045a1e7  jge        0x45a1ef

// block 0045a1e9
0045a1e9  fadd       dword ptr [0x4a3e10]

// block 0045a1ef
0045a1ef  fcompp     
0045a1f1  fnstsw     ax
0045a1f3  test       ah, 0x41
0045a1f6  je         0x45a36c

// block 0045a1fc
0045a1fc  mov        edx, dword ptr [ecx + 0xd0]
0045a202  fld        dword ptr [esp + 0x10]
0045a206  mov        eax, edx
0045a208  mov        dword ptr [esp + 0x20], eax
0045a20c  fild       dword ptr [esp + 0x20]
0045a210  test       eax, eax
0045a212  jge        0x45a21a

// block 0045a214
0045a214  fadd       dword ptr [0x4a3e10]

// block 0045a21a
0045a21a  fcompp     
0045a21c  fnstsw     ax
0045a21e  test       ah, 0x41
0045a221  je         0x45a36c

// block 0045a227
0045a227  mov        eax, dword ptr [ecx + 0xd4]
0045a22d  fld        dword ptr [esp + 0x14]
0045a231  add        eax, ebp
0045a233  mov        dword ptr [esp + 0x20], eax
0045a237  fild       dword ptr [esp + 0x20]
0045a23b  test       eax, eax
0045a23d  jge        0x45a245

// block 0045a23f
0045a23f  fadd       dword ptr [0x4a3e10]

// block 0045a245
0045a245  fcompp     
0045a247  fnstsw     ax
0045a249  test       ah, 5
0045a24c  jnp        0x45a36c

// block 0045a252
0045a252  mov        ecx, dword ptr [ecx + 0xd8]
0045a258  fld        dword ptr [esp + 0x18]
0045a25c  add        ecx, edx
0045a25e  mov        dword ptr [esp + 0x20], ecx
0045a262  fild       dword ptr [esp + 0x20]
0045a266  test       ecx, ecx
0045a268  jge        0x45a270

// block 0045a26a
0045a26a  fadd       dword ptr [0x4a3e10]

// block 0045a270
0045a270  fcompp     
0045a272  fnstsw     ax
0045a274  test       ah, 5
0045a277  jnp        0x45a36c

// block 0045a27d
0045a27d  mov        edx, dword ptr [edi + 0x3f4]
0045a283  mov        eax, dword ptr [edx + 8]
0045a286  cmp        dword ptr [esi + 0x4b563c], eax
0045a28c  je         0x45a2b4

// block 0045a28e
0045a28e  mov        dword ptr [esi + 0x4b563c], eax
0045a294  call       0x45a3c0

// block 0045a299
0045a299  mov        edx, dword ptr [esi + 0x4b563c]
0045a29f  mov        edx, dword ptr [edx]
0045a2a1  mov        eax, dword ptr [0x4ce8f0]
0045a2a6  mov        ecx, dword ptr [eax]
0045a2a8  push       edx
0045a2a9  push       0
0045a2ab  push       eax
0045a2ac  mov        eax, dword ptr [ecx + 0x104]
0045a2b2  call       eax

// block 0045a2b4
0045a2b4  cmp        byte ptr [esi + 0x4b5642], 1
0045a2bb  je         0x45a2c9

// block 0045a2bd
0045a2bd  call       0x45a3c0

// block 0045a2c2
0045a2c2  mov        byte ptr [esi + 0x4b5642], 1

// block 0045a2c9
0045a2c9  test       bl, 2
0045a2cc  jne        0x45a359

// block 0045a2d2
0045a2d2  test       byte ptr [edi + 0x47e], 1
0045a2d9  je         0x45a2e3

// block 0045a2db
0045a2db  mov        eax, dword ptr [edi + 0x3c0]
0045a2e1  jmp        0x45a2e9

// block 0045a2e3
0045a2e3  mov        eax, dword ptr [edi + 0x3bc]

// block 0045a2e9
0045a2e9  cmp        dword ptr [esi + 0x88ed50], 0
0045a2f0  mov        dword ptr [esp + 0x20], eax
0045a2f4  je         0x45a345

// block 0045a2f6
0045a2f6  mov        cl, byte ptr [esi + 0x88ed4e]
0045a2fc  shr        eax, 0x10
0045a2ff  call       0x459a40

// block 0045a304
0045a304  mov        cl, byte ptr [esi + 0x88ed4d]
0045a30a  mov        byte ptr [esp + 0x22], al
0045a30e  mov        al, byte ptr [esp + 0x21]
0045a312  call       0x459a40

// block 0045a317
0045a317  mov        cl, byte ptr [esi + 0x88ed4c]
0045a31d  mov        byte ptr [esp + 0x21], al
0045a321  mov        al, byte ptr [esp + 0x20]
0045a325  call       0x459a40

// block 0045a32a
0045a32a  mov        cl, byte ptr [esi + 0x88ed4f]
0045a330  mov        byte ptr [esp + 0x20], al
0045a334  mov        al, byte ptr [esp + 0x23]
0045a338  call       0x459a40

// block 0045a33d
0045a33d  mov        byte ptr [esp + 0x23], al
0045a341  mov        eax, dword ptr [esp + 0x20]

// block 0045a345
0045a345  mov        dword ptr [0x4d47f8], eax
0045a34a  mov        dword ptr [0x4d4814], eax
0045a34f  mov        dword ptr [0x4d4830], eax
0045a354  mov        dword ptr [0x4d484c], eax

// block 0045a359
0045a359  mov        eax, esi
0045a35b  call       0x459cf0

// block 0045a360
0045a360  mov        edx, 0x4d47e8
0045a365  mov        eax, esi
0045a367  call       0x45a4a0

// block 0045a36c
0045a36c  pop        edi
0045a36d  pop        esi
0045a36e  pop        ebp
0045a36f  xor        eax, eax
0045a371  pop        ebx
0045a372  add        esp, 0xc
0045a375  ret        4
