
// block 0040cbe0
0040cbe0  push       ebx
0040cbe1  push       ebp
0040cbe2  mov        ebp, dword ptr [esp + 0xc]
0040cbe6  push       esi
0040cbe7  mov        esi, eax
0040cbe9  mov        eax, dword ptr [0x4b43cc]
0040cbee  add        esi, 0x64
0040cbf1  test       byte ptr [eax + 0x7c], 1
0040cbf5  push       edi
0040cbf6  je         0x40cc13

// block 0040cbf8
0040cbf8  mov        eax, dword ptr [eax + 0x78]
0040cbfb  cmp        eax, 0x60
0040cbfe  jl         0x40cc13

// block 0040cc00
0040cc00  cmp        eax, 0x63
0040cc03  jg         0x40cc13

// block 0040cc05
0040cc05  fld        dword ptr [esp + 0x18]
0040cc09  fdiv       qword ptr [0x4a3fd8]
0040cc0f  fstp       dword ptr [esp + 0x18]

// block 0040cc13
0040cc13  lea        edi, [esi + 0x4bc]
0040cc19  mov        ebx, 0x7d0
0040cc1e  mov        edi, edi

// block 0040cc20
0040cc20  movzx      eax, word ptr [edi + 0x76]
0040cc24  fld        dword ptr [esp + 0x18]
0040cc28  test       ax, ax
0040cc2b  je         0x40ccc0

// block 0040cc31
0040cc31  cmp        ax, 3
0040cc35  je         0x40ccc0

// block 0040cc3b
0040cc3b  cmp        dword ptr [esp + 0x20], 0
0040cc40  je         0x40cc4b

// block 0040cc42
0040cc42  cmp        dword ptr [edi - 0x4b8], 0
0040cc49  jne        0x40ccc0

// block 0040cc4b
0040cc4b  fld        dword ptr [edi]
0040cc4d  fld        dword ptr [ebp]
0040cc50  fsub       st(2)
0040cc52  fcompp     
0040cc54  fnstsw     ax
0040cc56  test       ah, 0x41
0040cc59  je         0x40ccc0

// block 0040cc5b
0040cc5b  fld        dword ptr [edi]
0040cc5d  fld        dword ptr [ebp]
0040cc60  faddp      st(2)
0040cc62  fcompp     
0040cc64  fnstsw     ax
0040cc66  test       ah, 0x41
0040cc69  je         0x40ccc2

// block 0040cc6b
0040cc6b  fld        dword ptr [edi + 4]
0040cc6e  fld        dword ptr [ebp + 4]
0040cc71  fcompp     
0040cc73  fnstsw     ax
0040cc75  test       ah, 5
0040cc78  jnp        0x40ccc2

// block 0040cc7a
0040cc7a  call       0x40c8b0

// block 0040cc7f
0040cc7f  fld        dword ptr [0x4a4014]
0040cc85  sub        esp, 8
0040cc88  fst        dword ptr [esp + 4]
0040cc8c  mov        ecx, edi
0040cc8e  fstp       dword ptr [esp]
0040cc91  call       0x40d560

// block 0040cc96
0040cc96  test       eax, eax
0040cc98  jne        0x40ccc2

// block 0040cc9a
0040cc9a  cmp        dword ptr [esp + 0x1c], eax
0040cc9e  je         0x40ccc2

// block 0040cca0
0040cca0  fld        dword ptr [0x4a41f4]
0040cca6  sub        esp, 8
0040cca9  fstp       dword ptr [esp + 4]
0040ccad  fld        dword ptr [0x4a4060]
0040ccb3  fstp       dword ptr [esp]
0040ccb6  push       edi
0040ccb7  push       9
0040ccb9  call       0x4273f0

// block 0040ccbe
0040ccbe  jmp        0x40ccc2

// block 0040ccc0
0040ccc0  fstp       st(0)

// block 0040ccc2
0040ccc2  add        esi, 0x9f8
0040ccc8  add        edi, 0x9f8
0040ccce  sub        ebx, 1
0040ccd1  jne        0x40cc20

// block 0040ccd7
0040ccd7  mov        eax, dword ptr [esp + 0x1c]
0040ccdb  fld        dword ptr [esp + 0x18]
0040ccdf  push       eax
0040cce0  push       ecx
0040cce1  fstp       dword ptr [esp]
0040cce4  push       ebp
0040cce5  call       0x4150c0

// block 0040ccea
0040ccea  pop        edi
0040cceb  pop        esi
0040ccec  pop        ebp
0040cced  xor        eax, eax
0040ccef  pop        ebx
0040ccf0  ret        0x10
