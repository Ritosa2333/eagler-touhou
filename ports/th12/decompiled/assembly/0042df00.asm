
// block 0042df00
0042df00  sub        esp, 0x12c
0042df06  mov        eax, dword ptr [0x4ad138]
0042df0b  xor        eax, esp
0042df0d  mov        dword ptr [esp + 0x128], eax
0042df14  push       ebp
0042df15  xor        ebp, ebp
0042df17  push       edi
0042df18  mov        edi, ecx
0042df1a  cmp        dword ptr [esp + 0x144], ebp
0042df21  je         0x42df32

// block 0042df23
0042df23  cmp        dword ptr [edi + 0x44c], ebp
0042df29  je         0x42df32

// block 0042df2b
0042df2b  xor        eax, eax
0042df2d  jmp        0x42e3bf

// block 0042df32
0042df32  push       ebx
0042df33  push       esi
0042df34  push       0x100
0042df39  lea        eax, [esp + 0x3c]
0042df3d  push       ebp
0042df3e  push       eax
0042df3f  mov        dword ptr [esp + 0x30], ebp
0042df43  call       0x477420

// block 0042df48
0042df48  fld        dword ptr [esp + 0x150]
0042df4f  mov        ecx, dword ptr [edi + 0xf9c]
0042df55  fmul       st(0), st(0)
0042df57  add        esp, 0xc
0042df5a  cmp        dword ptr [edi + 0x470], ebp
0042df60  fstp       dword ptr [esp + 0x144]
0042df67  mov        dword ptr [esp + 0x14], ecx
0042df6b  jle        0x42e3b9

// block 0042df71
0042df71  mov        eax, dword ptr [ecx + 4]
0042df74  fld        dword ptr [0x4a3d78]
0042df7a  mov        edx, dword ptr [ecx]
0042df7c  mov        dword ptr [esp + 0x1c], eax
0042df80  mov        eax, dword ptr [esp + 0x140]
0042df87  fld        dword ptr [eax + 4]
0042df8a  mov        dword ptr [esp + 0x18], edx
0042df8e  fsub       dword ptr [esp + 0x1c]
0042df92  mov        edx, dword ptr [ecx + 8]
0042df95  fld        dword ptr [eax]
0042df97  mov        dword ptr [esp + 0x20], edx
0042df9b  fsub       dword ptr [esp + 0x18]
0042df9f  fmul       st(0), st(0)
0042dfa1  fld        st(1)
0042dfa3  fmulp      st(2)
0042dfa5  faddp      st(1)
0042dfa7  fstp       dword ptr [esp + 0x10]
0042dfab  fld        dword ptr [esp + 0x10]
0042dfaf  fld        dword ptr [esp + 0x144]
0042dfb6  fcompp     
0042dfb8  fnstsw     ax
0042dfba  test       ah, 5
0042dfbd  jnp        0x42e0eb

// block 0042dfc3
0042dfc3  mov        eax, 0x55555556
0042dfc8  imul       ebp
0042dfca  mov        eax, edx
0042dfcc  shr        eax, 0x1f
0042dfcf  add        eax, edx
0042dfd1  mov        ebx, 1
0042dfd6  add        dword ptr [esp + 0x24], ebx
0042dfda  lea        eax, [eax + eax*2]
0042dfdd  mov        edx, ebp
0042dfdf  sub        edx, eax
0042dfe1  mov        byte ptr [esp + ebp + 0x38], 1
0042dfe6  jne        0x42e0eb

// block 0042dfec
0042dfec  cmp        dword ptr [esp + 0x148], edx
0042dff3  je         0x42e02e

// block 0042dff5
0042dff5  sub        esp, 8
0042dff8  fst        dword ptr [esp + 4]
0042dffc  lea        ecx, [esp + 0x20]
0042e000  fstp       dword ptr [esp]
0042e003  call       0x42e820

// block 0042e008
0042e008  test       eax, eax
0042e00a  jne        0x42e030

// block 0042e00c
0042e00c  fld        dword ptr [0x4a41f4]
0042e012  sub        esp, 8
0042e015  fstp       dword ptr [esp + 4]
0042e019  mov        eax, ecx
0042e01b  fld        dword ptr [0x4a4060]
0042e021  fstp       dword ptr [esp]
0042e024  push       eax
0042e025  push       9
0042e027  call       0x4273f0

// block 0042e02c
0042e02c  jmp        0x42e030

// block 0042e02e
0042e02e  fstp       st(0)

// block 0042e030
0042e030  test       dword ptr [0x4cee78], 0x8000
0042e03a  movsx      ecx, word ptr [edi + 0x46e]
0042e041  mov        eax, dword ptr [0x4b44f4]
0042e046  mov        esi, dword ptr [eax + 0x488]
0042e04c  lea        edx, [ecx + ecx + 4]
0042e050  mov        dword ptr [esp + 0x10], edx
0042e054  je         0x42e067

// block 0042e056
0042e056  push       0x4cf1d0
0042e05b  call       dword ptr [0x498088]

// block 0042e061
0042e061  inc        byte ptr [0x4cf221]

// block 0042e067
0042e067  add        dword ptr [esi + 0x130], ebx
0042e06d  call       0x4621c0

// block 0042e072
0042e072  fld        dword ptr [esp + 0x18]
0042e076  fadd       qword ptr [0x4a3d70]
0042e07c  mov        ecx, dword ptr [esp + 0x10]
0042e080  mov        ebx, eax
0042e082  or         dword ptr [ebx + 0x480], 1
0042e089  fadd       qword ptr [0x4a3d28]
0042e08f  mov        dword ptr [ebx + 0x20], 0x17
0042e096  push       ecx
0042e097  push       ebx
0042e098  fstp       dword ptr [ebx + 0x430]
0042e09e  mov        ecx, esi
0042e0a0  fld        dword ptr [esp + 0x24]
0042e0a4  fadd       qword ptr [0x4a3d68]
0042e0aa  fstp       dword ptr [ebx + 0x434]
0042e0b0  fld        dword ptr [esp + 0x28]
0042e0b4  fstp       dword ptr [ebx + 0x438]
0042e0ba  call       0x454d10

// block 0042e0bf
0042e0bf  lea        eax, [esp + 0x30]
0042e0c3  call       0x461250

// block 0042e0c8
0042e0c8  test       dword ptr [0x4cee78], 0x8000
0042e0d2  je         0x42e0e5

// block 0042e0d4
0042e0d4  push       0x4cf1d0
0042e0d9  call       dword ptr [0x49808c]

// block 0042e0df
0042e0df  dec        byte ptr [0x4cf221]

// block 0042e0e5
0042e0e5  mov        ecx, dword ptr [esp + 0x14]
0042e0e9  jmp        0x42e0ed

// block 0042e0eb
0042e0eb  fstp       st(0)

// block 0042e0ed
0042e0ed  add        ecx, 0x14
0042e0f0  inc        ebp
0042e0f1  cmp        ebp, dword ptr [edi + 0x470]
0042e0f7  mov        dword ptr [esp + 0x14], ecx
0042e0fb  jl         0x42df71

// block 0042e101
0042e101  mov        eax, dword ptr [esp + 0x24]
0042e105  xor        ebx, ebx
0042e107  cmp        eax, ebx
0042e109  je         0x42e3b9

// block 0042e10f
0042e10f  cmp        eax, ebp
0042e111  jl         0x42e11c

// block 0042e113
0042e113  mov        byte ptr [edi + 0x7c], 1
0042e117  jmp        0x42e3bd

// block 0042e11c
0042e11c  xor        esi, esi
0042e11e  cmp        ebp, ebx
0042e120  jle        0x42e198

// block 0042e122
0042e122  cmp        byte ptr [esp + esi + 0x38], 0
0042e127  je         0x42e12e

// block 0042e129
0042e129  inc        esi
0042e12a  cmp        esi, ebp
0042e12c  jl         0x42e122

// block 0042e12e
0042e12e  cmp        esi, ebx
0042e130  je         0x42e198

// block 0042e132
0042e132  mov        edx, dword ptr [edi + 0x470]
0042e138  sub        edx, esi
0042e13a  mov        dword ptr [esp + 0x10], ebx
0042e13e  test       edx, edx
0042e140  jle        0x42e3b3

// block 0042e146
0042e146  lea        edx, [esi + esi*4]
0042e149  add        edx, edx
0042e14b  add        edx, edx
0042e14d  lea        ecx, [ecx]

// block 0042e150
0042e150  mov        ecx, dword ptr [edi + 0xf9c]
0042e156  mov        ebp, dword ptr [edx + ecx]
0042e159  lea        eax, [edx + ecx]
0042e15c  add        ecx, ebx
0042e15e  mov        dword ptr [ecx], ebp
0042e160  mov        ebp, dword ptr [eax + 4]
0042e163  mov        dword ptr [ecx + 4], ebp
0042e166  mov        ebp, dword ptr [eax + 8]
0042e169  mov        dword ptr [ecx + 8], ebp
0042e16c  mov        ebp, dword ptr [eax + 0xc]
0042e16f  mov        dword ptr [ecx + 0xc], ebp
0042e172  mov        eax, dword ptr [eax + 0x10]
0042e175  mov        dword ptr [ecx + 0x10], eax
0042e178  mov        eax, dword ptr [esp + 0x10]
0042e17c  mov        ecx, dword ptr [edi + 0x470]
0042e182  inc        eax
0042e183  sub        ecx, esi
0042e185  add        ebx, 0x14
0042e188  add        edx, 0x14
0042e18b  cmp        eax, ecx
0042e18d  mov        dword ptr [esp + 0x10], eax
0042e191  jl         0x42e150

// block 0042e193
0042e193  jmp        0x42e3b3

// block 0042e198
0042e198  xor        ecx, ecx
0042e19a  cmp        ebp, ecx
0042e19c  jle        0x42e3b9

// block 0042e1a2
0042e1a2  cmp        byte ptr [esp + esi + 0x38], 0
0042e1a7  jne        0x42e1b4

// block 0042e1a9
0042e1a9  inc        esi
0042e1aa  inc        ebx
0042e1ab  cmp        esi, ebp
0042e1ad  jl         0x42e1a2

// block 0042e1af
0042e1af  jmp        0x42e3b9

// block 0042e1b4
0042e1b4  cmp        esi, ebp
0042e1b6  mov        dword ptr [esp + 0x14], ebx
0042e1ba  jge        0x42e3b9

// block 0042e1c0
0042e1c0  cmp        ebx, ecx
0042e1c2  je         0x42e3b9

// block 0042e1c8
0042e1c8  cmp        byte ptr [esp + esi + 0x38], 0
0042e1cd  je         0x42e1df

// block 0042e1cf
0042e1cf  inc        esi
0042e1d0  cmp        esi, ebp
0042e1d2  jl         0x42e1c8

// block 0042e1d4
0042e1d4  mov        dword ptr [edi + 0x470], ebx
0042e1da  jmp        0x42e3b9

// block 0042e1df
0042e1df  cmp        esi, ebp
0042e1e1  jl         0x42e1ee

// block 0042e1e3
0042e1e3  mov        dword ptr [edi + 0x470], ebx
0042e1e9  jmp        0x42e3b9

// block 0042e1ee
0042e1ee  cmp        ebx, ecx
0042e1f0  mov        dword ptr [esp + 0x28], ecx
0042e1f4  jle        0x42e34d

// block 0042e1fa
0042e1fa  mov        dword ptr [esp + 0x10], ecx
0042e1fe  mov        edi, edi

// block 0042e200
0042e200  mov        eax, dword ptr [edi + 0xf9c]
0042e206  add        eax, dword ptr [esp + 0x10]
0042e20a  mov        edx, dword ptr [eax]
0042e20c  mov        dword ptr [esp + 0x18], edx
0042e210  mov        edx, dword ptr [eax + 4]
0042e213  mov        eax, dword ptr [eax + 8]
0042e216  mov        dword ptr [esp + 0x20], eax
0042e21a  mov        dword ptr [esp + 0x1c], edx
0042e21e  mov        eax, 0x55555556
0042e223  imul       ecx
0042e225  mov        eax, edx
0042e227  shr        eax, 0x1f
0042e22a  add        eax, edx
0042e22c  lea        edx, [eax + eax*2]
0042e22f  mov        eax, ecx
0042e231  sub        eax, edx
0042e233  jne        0x42e33b

// block 0042e239
0042e239  cmp        dword ptr [esp + 0x148], eax
0042e240  je         0x42e27d

// block 0042e242
0042e242  fld        dword ptr [0x4a3d78]
0042e248  sub        esp, 8
0042e24b  fst        dword ptr [esp + 4]
0042e24f  lea        ecx, [esp + 0x20]
0042e253  fstp       dword ptr [esp]
0042e256  call       0x42e820

// block 0042e25b
0042e25b  test       eax, eax
0042e25d  jne        0x42e27d

// block 0042e25f
0042e25f  fld        dword ptr [0x4a41f4]
0042e265  sub        esp, 8
0042e268  fstp       dword ptr [esp + 4]
0042e26c  fld        dword ptr [0x4a4060]
0042e272  fstp       dword ptr [esp]
0042e275  push       ecx
0042e276  push       9
0042e278  call       0x4273f0

// block 0042e27d
0042e27d  test       dword ptr [0x4cee78], 0x8000
0042e287  movsx      edx, word ptr [edi + 0x46e]
0042e28e  mov        ecx, dword ptr [0x4b44f4]
0042e294  mov        ebp, dword ptr [ecx + 0x488]
0042e29a  lea        eax, [edx + edx + 4]
0042e29e  mov        dword ptr [esp + 0x2c], eax
0042e2a2  je         0x42e2b5

// block 0042e2a4
0042e2a4  push       0x4cf1d0
0042e2a9  call       dword ptr [0x498088]

// block 0042e2af
0042e2af  inc        byte ptr [0x4cf221]

// block 0042e2b5
0042e2b5  inc        dword ptr [ebp + 0x130]
0042e2bb  call       0x4621c0

// block 0042e2c0
0042e2c0  fld        dword ptr [esp + 0x18]
0042e2c4  fadd       qword ptr [0x4a3d70]
0042e2ca  mov        edx, dword ptr [esp + 0x2c]
0042e2ce  mov        ebx, eax
0042e2d0  or         dword ptr [ebx + 0x480], 1
0042e2d7  fadd       qword ptr [0x4a3d28]
0042e2dd  mov        dword ptr [ebx + 0x20], 0x17
0042e2e4  push       edx
0042e2e5  push       ebx
0042e2e6  fstp       dword ptr [ebx + 0x430]
0042e2ec  mov        ecx, ebp
0042e2ee  fld        dword ptr [esp + 0x24]
0042e2f2  fadd       qword ptr [0x4a3d68]
0042e2f8  fstp       dword ptr [ebx + 0x434]
0042e2fe  fld        dword ptr [esp + 0x28]
0042e302  fstp       dword ptr [ebx + 0x438]
0042e308  call       0x454d10

// block 0042e30d
0042e30d  lea        eax, [esp + 0x34]
0042e311  call       0x461250

// block 0042e316
0042e316  test       dword ptr [0x4cee78], 0x8000
0042e320  je         0x42e333

// block 0042e322
0042e322  push       0x4cf1d0
0042e327  call       dword ptr [0x49808c]

// block 0042e32d
0042e32d  dec        byte ptr [0x4cf221]

// block 0042e333
0042e333  mov        ecx, dword ptr [esp + 0x28]
0042e337  mov        ebx, dword ptr [esp + 0x14]

// block 0042e33b
0042e33b  add        dword ptr [esp + 0x10], 0x14
0042e340  inc        ecx
0042e341  cmp        ecx, ebx
0042e343  mov        dword ptr [esp + 0x28], ecx
0042e347  jl         0x42e200

// block 0042e34d
0042e34d  mov        eax, dword ptr [edi + 0x470]
0042e353  xor        ebx, ebx
0042e355  sub        eax, esi
0042e357  mov        dword ptr [esp + 0x10], ebx
0042e35b  test       eax, eax
0042e35d  jle        0x42e3b3

// block 0042e35f
0042e35f  lea        edx, [esi + esi*4]
0042e362  add        edx, edx
0042e364  add        edx, edx
0042e366  jmp        0x42e370

// block 0042e370
0042e370  mov        ecx, dword ptr [edi + 0xf9c]
0042e376  mov        ebp, dword ptr [edx + ecx]
0042e379  lea        eax, [edx + ecx]
0042e37c  add        ecx, ebx
0042e37e  mov        dword ptr [ecx], ebp
0042e380  mov        ebp, dword ptr [eax + 4]
0042e383  mov        dword ptr [ecx + 4], ebp
0042e386  mov        ebp, dword ptr [eax + 8]
0042e389  mov        dword ptr [ecx + 8], ebp
0042e38c  mov        ebp, dword ptr [eax + 0xc]
0042e38f  mov        dword ptr [ecx + 0xc], ebp
0042e392  mov        eax, dword ptr [eax + 0x10]
0042e395  mov        dword ptr [ecx + 0x10], eax
0042e398  mov        eax, dword ptr [esp + 0x10]
0042e39c  mov        ecx, dword ptr [edi + 0x470]
0042e3a2  inc        eax
0042e3a3  sub        ecx, esi
0042e3a5  add        ebx, 0x14
0042e3a8  add        edx, 0x14
0042e3ab  cmp        eax, ecx
0042e3ad  mov        dword ptr [esp + 0x10], eax
0042e3b1  jl         0x42e370

// block 0042e3b3
0042e3b3  sub        dword ptr [edi + 0x470], esi

// block 0042e3b9
0042e3b9  mov        eax, dword ptr [esp + 0x24]

// block 0042e3bd
0042e3bd  pop        esi
0042e3be  pop        ebx

// block 0042e3bf
0042e3bf  mov        ecx, dword ptr [esp + 0x130]
0042e3c6  pop        edi
0042e3c7  pop        ebp
0042e3c8  xor        ecx, esp
0042e3ca  call       0x46c932

// block 0042e3cf
0042e3cf  add        esp, 0x12c
0042e3d5  ret        0x10
