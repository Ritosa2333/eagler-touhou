
// block 004082e0
004082e0  sub        esp, 8
004082e3  push       ebx
004082e4  push       ebp
004082e5  push       esi
004082e6  push       edi
004082e7  push       0x28
004082e9  call       0x406f60

// block 004082ee
004082ee  mov        ecx, dword ptr [esp + 0x1c]
004082f2  mov        eax, dword ptr [ecx + 0x18]
004082f5  cmp        eax, 0xc8
004082fa  mov        ebp, dword ptr [ecx + 0x500]
00408300  jl         0x408392

// block 00408306
00408306  mov        edi, ebp
00408308  mov        esi, 8
0040830d  lea        ecx, [ecx]

// block 00408310
00408310  call       0x408030

// block 00408315
00408315  add        edi, 0xb4
0040831b  sub        esi, 1
0040831e  jne        0x408310

// block 00408320
00408320  mov        esi, dword ptr [esp + 0x1c]
00408324  mov        eax, dword ptr [esi + 0x48]
00408327  push       eax
00408328  mov        ebx, 1
0040832d  call       0x461970

// block 00408332
00408332  mov        eax, dword ptr [esi + 0x500]
00408338  test       eax, eax
0040833a  je         0x40834f

// block 0040833c
0040833c  push       eax
0040833d  call       0x46c941

// block 00408342
00408342  add        esp, 4
00408345  mov        dword ptr [esi + 0x500], 0

// block 0040834f
0040834f  push       0x44
00408351  call       0x46c9ea

// block 00408356
00408356  mov        esi, eax
00408358  add        esp, 4
0040835b  test       esi, esi
0040835d  je         0x408385

// block 0040835f
0040835f  and        dword ptr [esi + 0x40], 0xfffffffe
00408363  push       0x44
00408365  push       0
00408367  push       esi
00408368  call       0x477420

// block 0040836d
0040836d  or         dword ptr [esi], 2
00408370  add        esp, 0xc
00408373  push       0x46
00408375  push       0
00408377  push       6
00408379  push       6
0040837b  push       8
0040837d  push       1
0040837f  push       esi
00408380  call       0x452a60

// block 00408385
00408385  pop        edi
00408386  pop        esi
00408387  pop        ebp
00408388  or         eax, 0xffffffff
0040838b  pop        ebx
0040838c  add        esp, 8
0040838f  ret        4

// block 00408392
00408392  mov        edx, eax
00408394  cmp        edx, dword ptr [ecx + 0x14]
00408397  je         0x408432

// block 0040839d
0040839d  test       eax, eax
0040839f  jne        0x408432

// block 004083a5
004083a5  fldz       
004083a7  push       ecx
004083a8  fstp       dword ptr [esp]
004083ab  call       0x4646e0

// block 004083b0
004083b0  fstp       dword ptr [esp + 0x10]
004083b4  xor        ebx, ebx
004083b6  lea        edi, [ebp + 0x34]
004083b9  lea        esp, [esp]

// block 004083c0
004083c0  mov        eax, dword ptr [0x4b4514]
004083c5  add        eax, 0x97c
004083ca  push       ebx
004083cb  lea        esi, [edi - 0x34]
004083ce  call       0x407ea0

// block 004083d3
004083d3  fldz       
004083d5  or         dword ptr [edi], 1
004083d8  fstp       dword ptr [edi - 0x10]
004083db  fld        dword ptr [esp + 0x10]
004083df  push       ecx
004083e0  fstp       dword ptr [esp]
004083e3  call       0x4646e0

// block 004083e8
004083e8  push       ecx
004083e9  fstp       dword ptr [esp]
004083ec  call       0x4646e0

// block 004083f1
004083f1  fstp       dword ptr [edi - 0x14]
004083f4  mov        eax, dword ptr [edi + 0x7c]
004083f7  fld        dword ptr [0x4a4448]
004083fd  push       ecx
004083fe  fstp       dword ptr [edi - 0xc]
00408401  mov        dword ptr [eax + 0x68], 0x12c
00408408  fld        dword ptr [esp + 0x14]
0040840c  fadd       qword ptr [0x4a3fc0]
00408412  fstp       dword ptr [esp + 0x18]
00408416  fld        dword ptr [esp + 0x18]
0040841a  fstp       dword ptr [esp]
0040841d  call       0x4646e0

// block 00408422
00408422  inc        ebx
00408423  fstp       dword ptr [esp + 0x10]
00408427  add        edi, 0xb4
0040842d  cmp        ebx, 8
00408430  jl         0x4083c0

// block 00408432
00408432  lea        ebx, [ebp + 4]
00408435  mov        dword ptr [esp + 0x10], 8
0040843d  mov        ebp, 0xfffffffe

// block 00408442
00408442  cmp        dword ptr [ebx + 0x80], 0
00408449  je         0x4084c0

// block 0040844b
0040844b  lea        edi, [ebx - 4]
0040844e  push       edi
0040844f  call       0x407b80

// block 00408454
00408454  mov        eax, dword ptr [ebx + 0xac]
0040845a  cmp        dword ptr [eax + 0x64], 0x12c
00408461  jl         0x4084af

// block 00408463
00408463  call       0x408030

// block 00408468
00408468  fld        dword ptr [ebx]
0040846a  push       ecx
0040846b  mov        eax, 0x1c
00408470  fstp       dword ptr [esp]
00408473  call       0x453e20

// block 00408478
00408478  push       0x44
0040847a  call       0x46c9ea

// block 0040847f
0040847f  mov        esi, eax
00408481  add        esp, 4
00408484  test       esi, esi
00408486  je         0x4084c0

// block 00408488
00408488  and        dword ptr [esi + 0x40], ebp
0040848b  push       0x44
0040848d  push       0
0040848f  push       esi
00408490  call       0x477420

// block 00408495
00408495  or         dword ptr [esi], 2
00408498  add        esp, 0xc
0040849b  push       0x46
0040849d  push       0
0040849f  push       6
004084a1  push       6
004084a3  push       8
004084a5  push       1
004084a7  push       esi
004084a8  call       0x452a60

// block 004084ad
004084ad  jmp        0x4084c0

// block 004084af
004084af  mov        ecx, dword ptr [ebx]
004084b1  mov        dword ptr [eax + 0x18], ecx
004084b4  mov        edx, dword ptr [ebx + 4]
004084b7  mov        dword ptr [eax + 0x1c], edx
004084ba  mov        ecx, dword ptr [ebx + 8]
004084bd  mov        dword ptr [eax + 0x20], ecx

// block 004084c0
004084c0  add        ebx, 0xb4
004084c6  sub        dword ptr [esp + 0x10], 1
004084cb  jne        0x408442

// block 004084d1
004084d1  mov        eax, dword ptr [esp + 0x1c]
004084d5  call       0x408510

// block 004084da
004084da  pop        edi
004084db  pop        esi
004084dc  pop        ebp
004084dd  xor        eax, eax
004084df  pop        ebx
004084e0  add        esp, 8
004084e3  ret        4
