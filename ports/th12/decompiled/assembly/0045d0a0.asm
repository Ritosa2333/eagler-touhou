
// block 0045d0a0
0045d0a0  fldz       
0045d0a2  mov        ecx, dword ptr [edi + 0x8856b0]
0045d0a8  fld        st(0)
0045d0aa  sub        esp, 8
0045d0ad  fld        dword ptr [esp + 0x1c]
0045d0b1  push       esi
0045d0b2  fucom      st(1)
0045d0b4  mov        esi, eax
0045d0b6  fnstsw     ax
0045d0b8  fstp       st(1)
0045d0ba  fld1       
0045d0bc  test       ah, 0x44
0045d0bf  jnp        0x45d0dd

// block 0045d0c1
0045d0c1  fstp       st(2)
0045d0c3  fstp       st(1)
0045d0c5  fstp       dword ptr [esp + 8]
0045d0c9  fld        dword ptr [esp + 8]
0045d0cd  fsincos    
0045d0cf  fstp       dword ptr [esp + 4]
0045d0d3  fstp       dword ptr [esp + 0x20]
0045d0d7  fld1       
0045d0d9  fldz       
0045d0db  jmp        0x45d0ed

// block 0045d0dd
0045d0dd  fstp       st(1)
0045d0df  fxch       st(1)
0045d0e1  fst        dword ptr [esp + 0x20]
0045d0e5  fxch       st(1)
0045d0e7  fst        dword ptr [esp + 4]
0045d0eb  fxch       st(1)

// block 0045d0ed
0045d0ed  fld        dword ptr [esp + 0x18]
0045d0f1  fld        qword ptr [0x4a3cf8]
0045d0f7  fmul       st(1), st(0)
0045d0f9  fxch       st(1)
0045d0fb  fstp       dword ptr [esp + 0x18]
0045d0ff  fmul       dword ptr [esp + 0x1c]
0045d103  fstp       dword ptr [esp + 0x1c]
0045d107  fld        dword ptr [esp + 4]
0045d10b  fld        dword ptr [esp + 0x18]
0045d10f  fld        st(0)
0045d111  fmulp      st(2)
0045d113  fld        dword ptr [esp + 0x20]
0045d117  fld        dword ptr [esp + 0x1c]
0045d11b  fld        st(0)
0045d11d  fmulp      st(2)
0045d11f  fxch       st(3)
0045d121  fsubrp     st(1)
0045d123  fld        dword ptr [esp + 0x10]
0045d127  fld        st(0)
0045d129  faddp      st(2)
0045d12b  fxch       st(1)
0045d12d  fstp       dword ptr [ecx]
0045d12f  fld        dword ptr [esp + 0x20]
0045d133  fmul       st(2)
0045d135  fld        dword ptr [esp + 4]
0045d139  fmul       st(4)
0045d13b  faddp      st(1)
0045d13d  fadd       dword ptr [esp + 0x14]
0045d141  fstp       dword ptr [ecx + 4]
0045d144  fxch       st(3)
0045d146  fst        dword ptr [ecx + 8]
0045d149  fld        dword ptr [esp + 4]
0045d14d  fmul       st(2)
0045d14f  fld        dword ptr [esp + 0x20]
0045d153  fmul       st(4)
0045d155  faddp      st(1)
0045d157  fsubp      st(4)
0045d159  fxch       st(3)
0045d15b  fstp       dword ptr [ecx + 0x14]
0045d15e  fld        dword ptr [esp + 4]
0045d162  fmul       st(2)
0045d164  fld        dword ptr [esp + 0x20]
0045d168  fmul       st(2)
0045d16a  fsubp      st(1)
0045d16c  fld        dword ptr [esp + 0x14]
0045d170  fld        st(0)
0045d172  faddp      st(2)
0045d174  fxch       st(1)
0045d176  fstp       dword ptr [ecx + 0x18]
0045d179  fxch       st(3)
0045d17b  fst        dword ptr [ecx + 0x1c]
0045d17e  fld        dword ptr [esp + 4]
0045d182  fmul       st(2)
0045d184  fld        dword ptr [esp + 0x20]
0045d188  fmul       st(4)
0045d18a  faddp      st(1)
0045d18c  fadd       dword ptr [esp + 0x10]
0045d190  fstp       dword ptr [ecx + 0x28]
0045d193  fld        dword ptr [esp + 0x20]
0045d197  fmul       st(2)
0045d199  fld        dword ptr [esp + 4]
0045d19d  fmul       st(4)
0045d19f  fsubp      st(1)
0045d1a1  fadd       st(4)
0045d1a3  fstp       dword ptr [ecx + 0x2c]
0045d1a6  fst        dword ptr [ecx + 0x30]
0045d1a9  fld        dword ptr [esp + 0x20]
0045d1ad  fmul       st(3)
0045d1af  fld        dword ptr [esp + 4]
0045d1b3  fmul       st(3)
0045d1b5  fsubp      st(1)
0045d1b7  fadd       dword ptr [esp + 0x10]
0045d1bb  fstp       dword ptr [ecx + 0x3c]
0045d1be  fld        dword ptr [esp + 0x20]
0045d1c2  fmulp      st(2)
0045d1c4  fld        dword ptr [esp + 4]
0045d1c8  fmulp      st(3)
0045d1ca  fxch       st(1)
0045d1cc  faddp      st(2)
0045d1ce  fxch       st(2)
0045d1d0  fsubrp     st(1)
0045d1d2  fstp       dword ptr [ecx + 0x40]
0045d1d5  mov        dword ptr [ecx + 0x38], esi
0045d1d8  mov        dword ptr [ecx + 0x10], esi
0045d1db  mov        esi, dword ptr [0x4ce8cc]
0045d1e1  fstp       dword ptr [ecx + 0x44]
0045d1e4  mov        dword ptr [ecx + 0x4c], edx
0045d1e7  fst        dword ptr [ecx + 0x48]
0045d1ea  mov        dword ptr [ecx + 0x24], edx
0045d1ed  fst        dword ptr [ecx + 0x34]
0045d1f0  fst        dword ptr [ecx + 0x20]
0045d1f3  fstp       dword ptr [ecx + 0xc]
0045d1f6  call       0x45a3c0

// block 0045d1fb
0045d1fb  mov        eax, dword ptr [0x4ce8f0]
0045d200  mov        ecx, dword ptr [eax]
0045d202  mov        edx, dword ptr [ecx + 0xe4]
0045d208  push       0
0045d20a  push       0xe
0045d20c  push       eax
0045d20d  call       edx

// block 0045d20f
0045d20f  mov        eax, dword ptr [0x4ce8cc]
0045d214  cmp        byte ptr [eax + 0x4b5647], 0
0045d21b  pop        esi
0045d21c  je         0x45d256

// block 0045d21e
0045d21e  mov        eax, dword ptr [0x4ce8f0]
0045d223  mov        ecx, dword ptr [eax]
0045d225  mov        edx, dword ptr [ecx + 0x10c]
0045d22b  push       3
0045d22d  push       4
0045d22f  push       0
0045d231  push       eax
0045d232  call       edx

// block 0045d234
0045d234  mov        eax, dword ptr [0x4ce8f0]
0045d239  mov        ecx, dword ptr [eax]
0045d23b  mov        edx, dword ptr [ecx + 0x10c]
0045d241  push       3
0045d243  push       1
0045d245  push       0
0045d247  push       eax
0045d248  call       edx

// block 0045d24a
0045d24a  mov        eax, dword ptr [0x4ce8cc]
0045d24f  mov        byte ptr [eax + 0x4b5647], 0

// block 0045d256
0045d256  cmp        byte ptr [edi + 0x4b5642], 1
0045d25d  je         0x45d292

// block 0045d25f
0045d25f  mov        eax, dword ptr [0x4ce8f0]
0045d264  mov        ecx, dword ptr [eax]
0045d266  mov        edx, dword ptr [ecx + 0x10c]
0045d26c  push       0
0045d26e  push       6
0045d270  push       0
0045d272  push       eax
0045d273  call       edx

// block 0045d275
0045d275  mov        eax, dword ptr [0x4ce8f0]
0045d27a  mov        ecx, dword ptr [eax]
0045d27c  mov        edx, dword ptr [ecx + 0x10c]
0045d282  push       0
0045d284  push       3
0045d286  push       0
0045d288  push       eax
0045d289  call       edx

// block 0045d28b
0045d28b  mov        byte ptr [edi + 0x4b5642], 1

// block 0045d292
0045d292  mov        eax, dword ptr [0x4ce8f0]
0045d297  mov        ecx, dword ptr [eax]
0045d299  mov        edx, dword ptr [ecx + 0x164]
0045d29f  push       0x44
0045d2a1  push       eax
0045d2a2  call       edx

// block 0045d2a4
0045d2a4  mov        edx, dword ptr [edi + 0x8856b0]
0045d2aa  mov        eax, dword ptr [0x4ce8f0]
0045d2af  mov        ecx, dword ptr [eax]
0045d2b1  push       0x14
0045d2b3  push       edx
0045d2b4  push       2
0045d2b6  push       5
0045d2b8  push       eax
0045d2b9  mov        eax, dword ptr [ecx + 0x14c]
0045d2bf  call       eax

// block 0045d2c1
0045d2c1  add        dword ptr [edi + 0x8856b0], 0x50
0045d2c8  inc        dword ptr [edi + 0xac]
0045d2ce  xor        eax, eax
0045d2d0  add        esp, 8
0045d2d3  ret        0x14
