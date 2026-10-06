
// block 00466f00
00466f00  sub        esp, 0x10
00466f03  mov        edx, dword ptr [esp + 0x14]
00466f07  push       ebx
00466f08  push       ebp
00466f09  push       esi
00466f0a  mov        esi, eax
00466f0c  mov        eax, dword ptr [edi + 4]
00466f0f  mov        ecx, dword ptr [eax + 0x10]
00466f12  mov        eax, dword ptr [esi + 0x1008]
00466f18  lea        ebx, [ecx + edx*4 + 4]
00466f1c  xor        edx, edx
00466f1e  mov        dword ptr [esp + 0x14], edx
00466f22  mov        dword ptr [esp + 0x18], eax
00466f26  lea        ecx, [eax + 0xc]
00466f29  test       eax, eax
00466f2b  jne        0x466f4a

// block 00466f2d
00466f2d  mov        ecx, eax
00466f2f  lea        ebp, [ecx + 4]
00466f32  cmp        ebp, 0x1000
00466f38  jge        0x466f45

// block 00466f3a
00466f3a  mov        dword ptr [ecx + esi + 8], edx
00466f3e  add        dword ptr [esi + 0x1008], 4

// block 00466f45
00466f45  mov        ecx, 0x10

// block 00466f4a
00466f4a  mov        edx, dword ptr [edi + 4]
00466f4d  mov        ebp, dword ptr [esp + 0x20]
00466f51  movzx      edx, byte ptr [edx + 0xb]
00466f55  inc        ebp
00466f56  cmp        ebp, edx
00466f58  jge        0x46703e

// block 00466f5e
00466f5e  lea        eax, [ecx + esi + 8]
00466f62  lea        edx, [ebx + 4]
00466f65  mov        dword ptr [esp + 0x20], eax
00466f69  mov        dword ptr [esp + 0x10], edx
00466f6d  jmp        0x466f74

// block 00466f70
00466f70  mov        edx, dword ptr [esp + 0x10]

// block 00466f74
00466f74  mov        eax, dword ptr [edi + 4]
00466f77  mov        cl, byte ptr [eax + ebx + 0x10]
00466f7b  cmp        cl, 0x66
00466f7e  je         0x466fcd

// block 00466f80
00466f80  cmp        cl, 0x67
00466f83  je         0x466fcd

// block 00466f85
00466f85  shr        edx, 2
00466f88  mov        ecx, dword ptr [eax + edx*4 + 0x10]
00466f8c  movzx      edx, word ptr [eax + 8]
00466f90  mov        dword ptr [esp + 0xc], ecx
00466f94  mov        eax, 1
00466f99  mov        ecx, ebp
00466f9b  shl        eax, cl
00466f9d  test       eax, edx
00466f9f  je         0x466faf

// block 00466fa1
00466fa1  mov        ecx, dword ptr [esp + 0xc]
00466fa5  push       ecx
00466fa6  mov        ecx, edi
00466fa8  call       0x468f70

// block 00466fad
00466fad  jmp        0x466fb3

// block 00466faf
00466faf  mov        eax, dword ptr [esp + 0xc]

// block 00466fb3
00466fb3  mov        edx, dword ptr [edi + 4]
00466fb6  cmp        byte ptr [ebx + edx + 0x11], 0x66
00466fbb  mov        dword ptr [esp + 0xc], eax
00466fbf  jne        0x467017

// block 00466fc1
00466fc1  fild       dword ptr [esp + 0xc]
00466fc5  mov        eax, dword ptr [esp + 0x20]
00466fc9  fstp       dword ptr [eax]
00466fcb  jmp        0x46701d

// block 00466fcd
00466fcd  shr        edx, 2
00466fd0  fld        dword ptr [eax + edx*4 + 0x10]
00466fd4  movzx      edx, word ptr [eax + 8]
00466fd8  mov        eax, 1
00466fdd  fstp       dword ptr [esp + 0xc]
00466fe1  fld        dword ptr [esp + 0xc]
00466fe5  mov        ecx, ebp
00466fe7  shl        eax, cl
00466fe9  test       eax, edx
00466feb  je         0x466ff8

// block 00466fed
00466fed  push       ecx
00466fee  mov        eax, edi
00466ff0  fstp       dword ptr [esp]
00466ff3  call       0x468fe0

// block 00466ff8
00466ff8  mov        ecx, dword ptr [edi + 4]
00466ffb  fstp       dword ptr [esp + 0xc]
00466fff  cmp        byte ptr [ebx + ecx + 0x11], 0x66
00467004  fld        dword ptr [esp + 0xc]
00467008  jne        0x467012

// block 0046700a
0046700a  mov        edx, dword ptr [esp + 0x20]
0046700e  fstp       dword ptr [edx]
00467010  jmp        0x46701d

// block 00467012
00467012  call       0x4931e0

// block 00467017
00467017  mov        ecx, dword ptr [esp + 0x20]
0046701b  mov        dword ptr [ecx], eax

// block 0046701d
0046701d  mov        edx, dword ptr [edi + 4]
00467020  movzx      eax, byte ptr [edx + 0xb]
00467024  add        dword ptr [esp + 0x10], 8
00467029  add        dword ptr [esp + 0x20], 4
0046702e  inc        ebp
0046702f  add        ebx, 8
00467032  cmp        ebp, eax
00467034  jl         0x466f70

// block 0046703a
0046703a  mov        eax, dword ptr [esp + 0x18]

// block 0046703e
0046703e  mov        edx, dword ptr [esi + 0x1008]
00467044  mov        ebx, 4
00467049  test       eax, eax
0046704b  je         0x467072

// block 0046704d
0046704d  mov        ecx, edx
0046704f  add        ecx, -4
00467052  js         0x467062

// block 00467054
00467054  mov        dword ptr [esi + 0x1008], ecx
0046705a  mov        ecx, dword ptr [ecx + esi + 8]
0046705e  mov        dword ptr [esp + 0x14], ecx

// block 00467062
00467062  mov        ecx, dword ptr [esp + 0x14]
00467066  mov        dword ptr [esi + 0x1008], eax
0046706c  mov        dword ptr [eax + esi + 4], ecx
00467070  jmp        0x467078

// block 00467072
00467072  mov        dword ptr [esi + 0x1008], ebx

// block 00467078
00467078  mov        ecx, dword ptr [esi + 0x1008]
0046707e  lea        ebp, [ecx + 4]
00467081  cmp        ebp, 0x1000
00467087  jge        0x467093

// block 00467089
00467089  mov        dword ptr [ecx + esi + 8], edx
0046708d  add        dword ptr [esi + 0x1008], ebx

// block 00467093
00467093  test       eax, eax
00467095  mov        eax, dword ptr [esi + 0x1008]
0046709b  lea        edx, [eax + 4]
0046709e  je         0x4670ca

// block 004670a0
004670a0  cmp        edx, 0x1000
004670a6  jge        0x4670b4

// block 004670a8
004670a8  mov        ecx, dword ptr [edi]
004670aa  mov        dword ptr [eax + esi + 8], ecx
004670ae  add        dword ptr [esi + 0x1008], ebx

// block 004670b4
004670b4  mov        eax, dword ptr [esi + 0x1008]
004670ba  lea        edx, [eax + 4]
004670bd  cmp        edx, 0x1000
004670c3  jge        0x4670f9

// block 004670c5
004670c5  mov        ecx, dword ptr [edi + 4]
004670c8  jmp        0x4670ef

// block 004670ca
004670ca  xor        ecx, ecx
004670cc  cmp        edx, 0x1000
004670d2  jge        0x4670de

// block 004670d4
004670d4  mov        dword ptr [eax + esi + 8], ecx
004670d8  add        dword ptr [esi + 0x1008], ebx

// block 004670de
004670de  mov        eax, dword ptr [esi + 0x1008]
004670e4  lea        edx, [eax + 4]
004670e7  cmp        edx, 0x1000
004670ed  jge        0x4670f9

// block 004670ef
004670ef  mov        dword ptr [eax + esi + 8], ecx
004670f3  add        dword ptr [esi + 0x1008], ebx

// block 004670f9
004670f9  mov        eax, dword ptr [edi + 0x1014]
004670ff  mov        ebx, dword ptr [eax + 4]
00467102  mov        dword ptr [eax + 4], esi
00467105  mov        eax, dword ptr [edi + 4]
00467108  mov        esi, dword ptr [edi + 0x1014]
0046710e  add        eax, 0x14
00467111  push       eax
00467112  mov        eax, dword ptr [esi + 0x102c]
00467118  call       0x4694f0

// block 0046711d
0046711d  fldz       
0046711f  mov        ecx, dword ptr [esi + 4]
00467122  mov        dword ptr [ecx + 4], eax
00467125  mov        esi, dword ptr [esi + 4]
00467128  fstp       dword ptr [esi]
0046712a  cmp        dword ptr [esi + 4], 0
0046712e  jne        0x467152

// block 00467130
00467130  mov        edx, dword ptr [edi + 4]
00467133  add        edx, 0x14
00467136  push       edx
00467137  call       0x466ea0

// block 0046713c
0046713c  add        esp, 4
0046713f  pop        esi
00467140  pop        ebp
00467141  mov        dword ptr [edi + 4], 0
00467148  or         eax, 0xffffffff
0046714b  pop        ebx
0046714c  add        esp, 0x10
0046714f  ret        4

// block 00467152
00467152  mov        eax, dword ptr [edi + 0x1014]
00467158  pop        esi
00467159  mov        dword ptr [eax + 4], ebx
0046715c  pop        ebp
0046715d  xor        eax, eax
0046715f  pop        ebx
00467160  add        esp, 0x10
00467163  ret        4
