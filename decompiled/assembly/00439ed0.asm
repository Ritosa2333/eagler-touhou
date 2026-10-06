
// block 00439ed0
00439ed0  push       ebp
00439ed1  mov        ebp, esp
00439ed3  and        esp, 0xfffffff8
00439ed6  sub        esp, 0x54
00439ed9  push       ebx
00439eda  push       esi
00439edb  mov        esi, dword ptr [0x4b4514]
00439ee1  mov        eax, dword ptr [esi + 0xa34]
00439ee7  xor        ebx, ebx
00439ee9  push       edi
00439eea  mov        dword ptr [esp + 0x14], esi
00439eee  mov        dword ptr [esp + 0x10], ebx
00439ef2  cmp        eax, dword ptr [esi + 0xa30]
00439ef8  jne        0x439f05

// block 00439efa
00439efa  xor        eax, eax
00439efc  pop        edi
00439efd  pop        esi
00439efe  pop        ebx
00439eff  mov        esp, ebp
00439f01  pop        ebp
00439f02  ret        8

// block 00439f05
00439f05  mov        ecx, dword ptr [ebp + 0xc]
00439f08  fld        dword ptr [ecx]
00439f0a  mov        eax, dword ptr [ebp + 8]
00439f0d  fld        qword ptr [0x4a3cf8]
00439f13  add        esi, 0xa58
00439f19  fmul       st(1), st(0)
00439f1b  mov        dword ptr [esp + 0x18], esi
00439f1f  fld        dword ptr [eax]
00439f21  lea        edi, [esi + 0x18]
00439f24  mov        dword ptr [esp + 0x1c], 0x100
00439f2c  fsub       st(2)
00439f2e  fstp       dword ptr [esp + 0x54]
00439f32  fld        dword ptr [ecx + 4]
00439f35  fmul       st(1)
00439f37  fld        dword ptr [eax + 4]
00439f3a  fsub       st(1)
00439f3c  fstp       dword ptr [esp + 0x58]
00439f40  fld        dword ptr [eax]
00439f42  faddp      st(3)
00439f44  fxch       st(2)
00439f46  fstp       dword ptr [esp + 0x48]
00439f4a  fld        dword ptr [eax + 4]
00439f4d  faddp      st(2)
00439f4f  fxch       st(1)
00439f51  fstp       dword ptr [esp + 0x4c]
00439f55  fld        dword ptr [esp + 0x4c]
00439f59  fldz       
00439f5b  jmp        0x439f70

// block 00439f60
00439f60  fld        dword ptr [esp + 0x4c]
00439f64  fldz       
00439f66  fld        qword ptr [0x4a3cf8]
00439f6c  fxch       st(2)
00439f6e  fxch       st(1)

// block 00439f70
00439f70  mov        eax, dword ptr [edi + 0x30]
00439f73  test       eax, eax
00439f75  je         0x43a1d7

// block 00439f7b
00439f7b  cmp        eax, 2
00439f7e  je         0x43a1d7

// block 00439f84
00439f84  fld        dword ptr [edi + 0x50]
00439f87  mov        edx, dword ptr [edi + 0x5c]
00439f8a  fmul       st(3)
00439f8c  mov        cl, byte ptr [edx + 0x1d]
00439f8f  fld        dword ptr [edi - 4]
00439f92  fsub       st(1)
00439f94  fstp       dword ptr [esp + 0x30]
00439f98  fld        dword ptr [edi + 0x54]
00439f9b  fmul       st(4)
00439f9d  fld        dword ptr [edi]
00439f9f  fsub       st(1)
00439fa1  fstp       dword ptr [esp + 0x34]
00439fa5  fld        dword ptr [edi - 4]
00439fa8  faddp      st(2)
00439faa  fxch       st(1)
00439fac  fstp       dword ptr [esp + 0x3c]
00439fb0  fadd       dword ptr [edi]
00439fb2  fstp       dword ptr [esp + 0x40]
00439fb6  cmp        cl, 2
00439fb9  jne        0x439fd8

// block 00439fbb
00439fbb  fld        dword ptr [edi + 0x54]
00439fbe  fmulp      st(3)
00439fc0  fld        dword ptr [esp + 0x34]
00439fc4  fsub       st(3)
00439fc6  fstp       dword ptr [esp + 0x34]
00439fca  fld        dword ptr [esp + 0x40]
00439fce  fsubrp     st(3)
00439fd0  fxch       st(2)
00439fd2  fstp       dword ptr [esp + 0x40]
00439fd6  jmp        0x439fda

// block 00439fd8
00439fd8  fstp       st(2)

// block 00439fda
00439fda  fld        dword ptr [esp + 0x34]
00439fde  fcomp      st(1)
00439fe0  fnstsw     ax
00439fe2  test       ah, 0x41
00439fe5  je         0x43a1d9

// block 00439feb
00439feb  fld        dword ptr [esp + 0x30]
00439fef  fld        dword ptr [esp + 0x48]
00439ff3  fcompp     
00439ff5  fnstsw     ax
00439ff7  test       ah, 5
00439ffa  jnp        0x43a1d9

// block 0043a000
0043a000  fld        dword ptr [esp + 0x40]
0043a004  fld        dword ptr [esp + 0x58]
0043a008  fcompp     
0043a00a  fnstsw     ax
0043a00c  test       ah, 0x41
0043a00f  je         0x43a1d9

// block 0043a015
0043a015  fld        dword ptr [esp + 0x3c]
0043a019  fld        dword ptr [esp + 0x54]
0043a01d  fcompp     
0043a01f  fnstsw     ax
0043a021  test       ah, 0x41
0043a024  je         0x43a1d9

// block 0043a02a
0043a02a  cmp        cl, 2
0043a02d  je         0x43a041

// block 0043a02f
0043a02f  fcom       st(1)
0043a031  fnstsw     ax
0043a033  test       ah, 5
0043a036  jnp        0x43a1d9

// block 0043a03c
0043a03c  cmp        cl, 2
0043a03f  jne        0x43a050

// block 0043a041
0043a041  fcompp     
0043a043  fnstsw     ax
0043a045  test       ah, 5
0043a048  jnp        0x43a1dd

// block 0043a04e
0043a04e  jmp        0x43a054

// block 0043a050
0043a050  fstp       st(0)
0043a052  fstp       st(0)

// block 0043a054
0043a054  mov        eax, dword ptr [edx + 0x30]
0043a057  test       eax, eax
0043a059  je         0x43a06f

// block 0043a05b
0043a05b  mov        ecx, dword ptr [ebp + 8]
0043a05e  push       ecx
0043a05f  mov        ecx, dword ptr [esp + 0x18]
0043a063  mov        edx, esi
0043a065  call       eax

// block 0043a067
0043a067  test       eax, eax
0043a069  jne        0x43a1dd

// block 0043a06f
0043a06f  mov        edx, dword ptr [edi + 0x5c]
0043a072  mov        dword ptr [edi + 0x40], 1
0043a079  mov        al, byte ptr [edx + 0x1d]
0043a07c  cmp        al, 2
0043a07e  jne        0x43a0a1

// block 0043a080
0043a080  cmp        dword ptr [edi + 0x44], 0
0043a084  jne        0x43a0a5

// block 0043a086
0043a086  mov        eax, dword ptr [edi + 0x34]
0043a089  push       eax
0043a08a  mov        ebx, 2
0043a08f  call       0x461970

// block 0043a094
0043a094  mov        ebx, dword ptr [esp + 0x10]
0043a098  mov        dword ptr [edi + 0x44], 1
0043a09f  jmp        0x43a0a5

// block 0043a0a1
0043a0a1  cmp        al, 4
0043a0a3  jne        0x43a0b2

// block 0043a0a5
0043a0a5  push       4
0043a0a7  mov        ecx, esi
0043a0a9  call       0x407700

// block 0043a0ae
0043a0ae  test       eax, eax
0043a0b0  je         0x43a0b9

// block 0043a0b2
0043a0b2  add        ebx, dword ptr [edi + 0x48]
0043a0b5  mov        dword ptr [esp + 0x10], ebx

// block 0043a0b9
0043a0b9  mov        ecx, dword ptr [edi + 0x5c]
0043a0bc  mov        al, byte ptr [ecx + 0x1d]
0043a0bf  cmp        al, 2
0043a0c1  je         0x43a1dd

// block 0043a0c7
0043a0c7  cmp        al, 4
0043a0c9  je         0x43a1dd

// block 0043a0cf
0043a0cf  lea        esi, [edi + 0x34]
0043a0d2  call       0x461c50

// block 0043a0d7
0043a0d7  fld        dword ptr [eax + 0x2c]
0043a0da  mov        edx, dword ptr [esi]
0043a0dc  fstp       dword ptr [esp + 0x2c]
0043a0e0  push       edx
0043a0e1  call       0x461a70

// block 0043a0e6
0043a0e6  mov        dword ptr [esi], 0
0043a0ec  mov        eax, dword ptr [edi + 0x5c]
0043a0ef  cmp        byte ptr [eax + 0x1d], 3
0043a0f3  push       0
0043a0f5  je         0x43a120

// block 0043a0f7
0043a0f7  movsx      eax, word ptr [eax + 0x20]
0043a0fb  mov        edx, dword ptr [esp + 0x18]
0043a0ff  add        eax, 5
0043a102  push       eax
0043a103  mov        eax, dword ptr [edx + 0x10]
0043a106  lea        ecx, [esp + 0x28]
0043a10a  push       ecx
0043a10b  push       eax
0043a10c  mov        eax, 0x17
0043a111  xor        ecx, ecx
0043a113  call       0x4615a0

// block 0043a118
0043a118  mov        ecx, dword ptr [esp + 0x20]
0043a11c  mov        dword ptr [esi], ecx
0043a11e  jmp        0x43a191

// block 0043a120
0043a120  mov        edx, dword ptr [0x4b43c4]
0043a126  cmp        dword ptr [edx + 0x3c], 0
0043a12a  je         0x43a16a

// block 0043a12c
0043a12c  movsx      eax, word ptr [eax + 0x20]
0043a130  mov        edx, dword ptr [esp + 0x18]
0043a134  add        eax, 6
0043a137  push       eax
0043a138  mov        eax, dword ptr [edx + 0x10]
0043a13b  lea        ecx, [esp + 0x2c]
0043a13f  push       ecx
0043a140  push       eax
0043a141  mov        eax, 0x17
0043a146  xor        ecx, ecx
0043a148  call       0x4615a0

// block 0043a14d
0043a14d  mov        ecx, dword ptr [esp + 0x24]
0043a151  mov        dword ptr [esi], ecx
0043a153  mov        ecx, dword ptr [edi + 0x48]
0043a156  mov        eax, 0x55555556
0043a15b  imul       ecx
0043a15d  mov        eax, edx
0043a15f  shr        eax, 0x1f
0043a162  add        eax, edx
0043a164  add        dword ptr [esp + 0x10], eax
0043a168  jmp        0x43a191

// block 0043a16a
0043a16a  movsx      ecx, word ptr [eax + 0x20]
0043a16e  mov        eax, dword ptr [esp + 0x18]
0043a172  add        ecx, 5
0043a175  push       ecx
0043a176  mov        ecx, dword ptr [eax + 0x10]
0043a179  lea        edx, [esp + 0x30]
0043a17d  push       edx
0043a17e  push       ecx
0043a17f  mov        eax, 0x17
0043a184  xor        ecx, ecx
0043a186  call       0x4615a0

// block 0043a18b
0043a18b  mov        edx, dword ptr [esp + 0x28]
0043a18f  mov        dword ptr [esi], edx

// block 0043a191
0043a191  call       0x461c50

// block 0043a196
0043a196  fld        dword ptr [esp + 0x2c]
0043a19a  or         dword ptr [eax + 0x47c], 4
0043a1a1  fstp       dword ptr [eax + 0x2c]
0043a1a4  fld        dword ptr [0x4a4210]
0043a1aa  add        eax, 0x430
0043a1af  fstp       dword ptr [edi + 4]
0043a1b2  lea        ecx, [edi - 4]
0043a1b5  fld        dword ptr [edi + 0x14]
0043a1b8  mov        dword ptr [edi + 0x30], 2
0043a1bf  fmul       qword ptr [0x4a3ec8]
0043a1c5  fstp       dword ptr [edi + 0x14]
0043a1c8  call       0x422c10

// block 0043a1cd
0043a1cd  mov        esi, dword ptr [esp + 0x18]
0043a1d1  mov        ebx, dword ptr [esp + 0x10]
0043a1d5  jmp        0x43a1dd

// block 0043a1d7
0043a1d7  fstp       st(2)

// block 0043a1d9
0043a1d9  fstp       st(0)
0043a1db  fstp       st(0)

// block 0043a1dd
0043a1dd  add        esi, 0x78
0043a1e0  add        edi, 0x78
0043a1e3  sub        dword ptr [esp + 0x1c], 1
0043a1e8  mov        dword ptr [esp + 0x18], esi
0043a1ec  jne        0x439f60

// block 0043a1f2
0043a1f2  mov        eax, dword ptr [ebp + 0xc]
0043a1f5  mov        edi, dword ptr [ebp + 8]
0043a1f8  push       eax
0043a1f9  call       0x406de0

// block 0043a1fe
0043a1fe  fld        qword ptr [0x4a3cf8]
0043a204  add        ebx, eax
0043a206  mov        dword ptr [esp + 0x10], ebx
0043a20a  mov        ebx, dword ptr [esp + 0x14]
0043a20e  add        ebx, 0x8988
0043a214  lea        edi, [ebx + 0x50]
0043a217  mov        esi, 0x80

// block 0043a21c
0043a21c  mov        ecx, dword ptr [edi + 0x20]
0043a21f  test       cl, 1
0043a222  je         0x43a3f6

// block 0043a228
0043a228  mov        eax, dword ptr [edi]
0043a22a  cmp        eax, dword ptr [edi - 4]
0043a22d  je         0x43a23b

// block 0043a22f
0043a22f  cdq        
0043a230  idiv       dword ptr [edi + 0x1c]
0043a233  test       edx, edx
0043a235  je         0x43a3f6

// block 0043a23b
0043a23b  test       cl, 2
0043a23e  jne        0x43a3a4

// block 0043a244
0043a244  fldz       
0043a246  fcomp      dword ptr [edi - 0x48]
0043a249  fnstsw     ax
0043a24b  test       ah, 0x44
0043a24e  jp         0x43a2b3

// block 0043a250
0043a250  fld        dword ptr [edi - 0x40]
0043a253  fmul       st(1)
0043a255  fld        dword ptr [edi - 0x38]
0043a258  fsub       st(1)
0043a25a  fld        dword ptr [esp + 0x48]
0043a25e  fcompp     
0043a260  fnstsw     ax
0043a262  test       ah, 5
0043a265  jnp        0x43a3f4

// block 0043a26b
0043a26b  fadd       dword ptr [edi - 0x38]
0043a26e  fld        dword ptr [esp + 0x54]
0043a272  fcompp     
0043a274  fnstsw     ax
0043a276  test       ah, 0x41
0043a279  je         0x43a3f6

// block 0043a27f
0043a27f  fld        dword ptr [edi - 0x3c]
0043a282  fmul       st(1)
0043a284  fld        dword ptr [edi - 0x34]
0043a287  fsub       st(1)
0043a289  fld        dword ptr [esp + 0x4c]
0043a28d  fcompp     
0043a28f  fnstsw     ax
0043a291  test       ah, 5
0043a294  jnp        0x43a3f4

// block 0043a29a
0043a29a  fadd       dword ptr [edi - 0x34]
0043a29d  fld        dword ptr [esp + 0x58]
0043a2a1  fcompp     
0043a2a3  fnstsw     ax
0043a2a5  test       ah, 0x41
0043a2a8  jne        0x43a3d1

// block 0043a2ae
0043a2ae  jmp        0x43a3f6

// block 0043a2b3
0043a2b3  mov        eax, dword ptr [ebp + 8]
0043a2b6  fstp       st(0)
0043a2b8  fld        dword ptr [eax]
0043a2ba  fsub       dword ptr [edi - 0x38]
0043a2bd  fstp       dword ptr [esp + 0x30]
0043a2c1  fld        dword ptr [eax + 4]
0043a2c4  fsub       dword ptr [edi - 0x34]
0043a2c7  fstp       dword ptr [esp + 0x34]
0043a2cb  fld        dword ptr [edi - 0x48]
0043a2ce  fchs       
0043a2d0  fstp       dword ptr [esp + 0x1c]
0043a2d4  fld        dword ptr [esp + 0x1c]
0043a2d8  call       0x4938c0

// block 0043a2dd
0043a2dd  fstp       dword ptr [esp + 0x2c]
0043a2e1  fld        dword ptr [esp + 0x2c]
0043a2e5  fstp       dword ptr [esp + 0x28]
0043a2e9  fld        dword ptr [esp + 0x1c]
0043a2ed  call       0x4939f0

// block 0043a2f2
0043a2f2  fstp       dword ptr [esp + 0x2c]
0043a2f6  fld        dword ptr [esp + 0x2c]
0043a2fa  mov        ecx, dword ptr [ebp + 0xc]
0043a2fd  fstp       dword ptr [esp + 0x2c]
0043a301  fld        dword ptr [esp + 0x2c]
0043a305  fld        st(0)
0043a307  fld        dword ptr [esp + 0x30]
0043a30b  fld        st(0)
0043a30d  fmulp      st(2)
0043a30f  fld        dword ptr [esp + 0x28]
0043a313  fld        st(0)
0043a315  fld        dword ptr [esp + 0x34]
0043a319  fld        st(0)
0043a31b  fmulp      st(2)
0043a31d  fxch       st(4)
0043a31f  fsubrp     st(1)
0043a321  fstp       dword ptr [esp + 0x3c]
0043a325  fmulp      st(1)
0043a327  fxch       st(2)
0043a329  fmulp      st(1)
0043a32b  faddp      st(1)
0043a32d  fstp       dword ptr [esp + 0x40]
0043a331  fld        dword ptr [ecx]
0043a333  fld        qword ptr [0x4a3cf8]
0043a339  fmul       st(1), st(0)
0043a33b  fld        dword ptr [edi - 0x40]
0043a33e  fchs       
0043a340  fmul       st(1)
0043a342  fld        dword ptr [esp + 0x3c]
0043a346  fld        st(0)
0043a348  fadd       st(4)
0043a34a  fcomp      st(2)
0043a34c  fnstsw     ax
0043a34e  fstp       st(1)
0043a350  test       ah, 5
0043a353  jnp        0x43a3ec

// block 0043a359
0043a359  fld        dword ptr [edi - 0x40]
0043a35c  fmul       st(2)
0043a35e  fxch       st(1)
0043a360  fsubrp     st(3)
0043a362  fcomp      st(2)
0043a364  fnstsw     ax
0043a366  fstp       st(1)
0043a368  test       ah, 5
0043a36b  jnp        0x43a3f6

// block 0043a371
0043a371  fld        dword ptr [ecx + 4]
0043a374  fmul       st(1)
0043a376  fld        dword ptr [edi - 0x3c]
0043a379  fchs       
0043a37b  fmul       st(2)
0043a37d  fld        dword ptr [esp + 0x40]
0043a381  fld        st(0)
0043a383  fadd       st(3)
0043a385  fcomp      st(2)
0043a387  fnstsw     ax
0043a389  fstp       st(1)
0043a38b  test       ah, 5
0043a38e  jnp        0x43a3f2

// block 0043a390
0043a390  fld        dword ptr [edi - 0x3c]
0043a393  fmul       st(3)
0043a395  fxch       st(1)
0043a397  fsubrp     st(2)
0043a399  fcompp     
0043a39b  fnstsw     ax
0043a39d  test       ah, 5
0043a3a0  jnp        0x43a3f6

// block 0043a3a2
0043a3a2  jmp        0x43a3d1

// block 0043a3a4
0043a3a4  fld        dword ptr [ebx]
0043a3a6  mov        eax, dword ptr [ebp + 8]
0043a3a9  fld        dword ptr [edi - 0x34]
0043a3ac  fsub       dword ptr [eax + 4]
0043a3af  fld        dword ptr [edi - 0x38]
0043a3b2  fsub       dword ptr [eax]
0043a3b4  fld        st(2)
0043a3b6  fmulp      st(3)
0043a3b8  fmul       st(0), st(0)
0043a3ba  fld        st(1)
0043a3bc  fmulp      st(2)
0043a3be  faddp      st(1)
0043a3c0  fstp       dword ptr [esp + 0x2c]
0043a3c4  fld        dword ptr [esp + 0x2c]
0043a3c8  fcompp     
0043a3ca  fnstsw     ax
0043a3cc  test       ah, 0x41
0043a3cf  je         0x43a3f6

// block 0043a3d1
0043a3d1  mov        eax, dword ptr [edi + 0x10]
0043a3d4  add        dword ptr [esp + 0x10], eax
0043a3d8  add        dword ptr [edi + 0x14], eax
0043a3db  mov        eax, dword ptr [edi + 0x14]
0043a3de  cmp        dword ptr [edi + 0x18], eax
0043a3e1  jg         0x43a3f6

// block 0043a3e3
0043a3e3  mov        dword ptr [edi + 0x10], 0
0043a3ea  jmp        0x43a3f6

// block 0043a3ec
0043a3ec  fstp       st(2)
0043a3ee  fstp       st(1)
0043a3f0  jmp        0x43a3f6

// block 0043a3f2
0043a3f2  fstp       st(1)

// block 0043a3f4
0043a3f4  fstp       st(0)

// block 0043a3f6
0043a3f6  add        ebx, 0x74
0043a3f9  add        edi, 0x74
0043a3fc  sub        esi, 1
0043a3ff  jne        0x43a21c

// block 0043a405
0043a405  mov        esi, dword ptr [esp + 0x10]
0043a409  fstp       st(0)
0043a40b  cmp        esi, 0x50
0043a40e  jl         0x43a41e

// block 0043a410
0043a410  mov        dword ptr [esp + 0x10], 0x50
0043a418  mov        esi, dword ptr [esp + 0x10]
0043a41c  jmp        0x43a422

// block 0043a41e
0043a41e  test       esi, esi
0043a420  je         0x43a463

// block 0043a422
0043a422  mov        eax, 0x66666667
0043a427  imul       esi
0043a429  sar        edx, 2
0043a42c  mov        ecx, edx
0043a42e  shr        ecx, 0x1f
0043a431  lea        ecx, [edx + ecx + 0xa]
0043a435  mov        eax, 0x66666667
0043a43a  imul       ecx
0043a43c  mov        eax, dword ptr [0x4b0c44]
0043a441  sar        edx, 2
0043a444  mov        ecx, edx
0043a446  shr        ecx, 0x1f
0043a449  add        ecx, edx
0043a44b  add        eax, ecx
0043a44d  cmp        eax, 0x3b9aca00
0043a452  mov        dword ptr [0x4b0c44], eax
0043a457  jl         0x43a463

// block 0043a459
0043a459  mov        dword ptr [0x4b0c44], 0x3b9ac9ff

// block 0043a463
0043a463  pop        edi
0043a464  mov        eax, esi
0043a466  pop        esi
0043a467  pop        ebx
0043a468  mov        esp, ebp
0043a46a  pop        ebp
0043a46b  ret        8
