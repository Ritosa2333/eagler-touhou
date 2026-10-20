
// block 0041aba0
0041aba0  push       ebp
0041aba1  mov        ebp, esp
0041aba3  and        esp, 0xfffffff8
0041aba6  sub        esp, 0x20
0041aba9  fld        dword ptr [0x4a3e68]
0041abaf  mov        edx, dword ptr [0x4ce8cc]
0041abb5  push       ebx
0041abb6  fst        dword ptr [esp + 0x10]
0041abba  push       ebp
0041abbb  fstp       dword ptr [esp + 0x18]
0041abbf  push       esi
0041abc0  mov        esi, ecx
0041abc2  mov        eax, dword ptr [esi + 0xe0]
0041abc8  push       edi
0041abc9  push       eax
0041abca  xor        ebx, ebx
0041abcc  call       0x461920

// block 0041abd1
0041abd1  mov        edi, eax
0041abd3  cmp        edi, ebx
0041abd5  jne        0x41abdd

// block 0041abd7
0041abd7  mov        dword ptr [esi + 0xe0], ebx

// block 0041abdd
0041abdd  fild       dword ptr [esi + 0x1608]
0041abe3  push       ecx
0041abe4  fmul       qword ptr [0x4a42b0]
0041abea  fidiv      dword ptr [esi + 0x160c]
0041abf0  or         dword ptr [edi + 0x47c], 4
0041abf7  fadd       qword ptr [0x4a4290]
0041abfd  fstp       dword ptr [edi + 0x24]
0041ac00  fld        dword ptr [edi + 0x2c]
0041ac03  fld        dword ptr [edi + 0x24]
0041ac06  fmul       qword ptr [0x4a3cf8]
0041ac0c  fsubp      st(1)
0041ac0e  fld        dword ptr [edi + 0x24]
0041ac11  fmul       qword ptr [0x4a42a8]
0041ac17  faddp      st(1)
0041ac19  fstp       dword ptr [esp + 0x18]
0041ac1d  fld        dword ptr [esp + 0x18]
0041ac21  fstp       dword ptr [esp]
0041ac24  call       0x4646e0

// block 0041ac29
0041ac29  fstp       dword ptr [esp + 0x14]
0041ac2d  mov        ebp, 0x1f

// block 0041ac32
0041ac32  fld        dword ptr [edi + 0x44]
0041ac35  sub        esp, 8
0041ac38  fstp       dword ptr [esp + 4]
0041ac3c  lea        ecx, [esp + 0x2c]
0041ac40  fld        dword ptr [esp + 0x1c]
0041ac44  fstp       dword ptr [esp]
0041ac47  call       0x41c580

// block 0041ac4c
0041ac4c  fld        dword ptr [esi + 0x34]
0041ac4f  lea        ecx, [esp + 0x1c]
0041ac53  fadd       dword ptr [esp + 0x24]
0041ac57  push       ecx
0041ac58  lea        edx, [esp + 0x28]
0041ac5c  push       edx
0041ac5d  fstp       dword ptr [esp + 0x2c]
0041ac61  fld        dword ptr [esp + 0x30]
0041ac65  fadd       dword ptr [esi + 0x38]
0041ac68  fstp       dword ptr [esp + 0x30]
0041ac6c  fld        dword ptr [esi + 0x3c]
0041ac6f  fadd       dword ptr [esp + 0x34]
0041ac73  fstp       dword ptr [esp + 0x34]
0041ac77  call       0x439ed0

// block 0041ac7c
0041ac7c  fld        dword ptr [edi + 0x24]
0041ac7f  push       ecx
0041ac80  fmul       qword ptr [0x4a4188]
0041ac86  add        ebx, eax
0041ac88  fstp       dword ptr [esp + 0x1c]
0041ac8c  fld        dword ptr [esp + 0x1c]
0041ac90  fadd       dword ptr [esp + 0x18]
0041ac94  fstp       dword ptr [esp + 0x1c]
0041ac98  fld        dword ptr [esp + 0x1c]
0041ac9c  fstp       dword ptr [esp]
0041ac9f  call       0x4646e0

// block 0041aca4
0041aca4  sub        ebp, 1
0041aca7  fstp       dword ptr [esp + 0x14]
0041acab  jne        0x41ac32

// block 0041acad
0041acad  cmp        ebx, 0x1e
0041acb0  jl         0x41acf2

// block 0041acb2
0041acb2  lea        ecx, [ebx + ebx*2 - 0x5a]
0041acb6  mov        eax, 0x66666667
0041acbb  imul       ecx
0041acbd  sar        edx, 1
0041acbf  mov        eax, edx
0041acc1  shr        eax, 0x1f
0041acc4  lea        eax, [edx + eax + 0x1e]
0041acc8  cmp        eax, 0x46
0041accb  jl         0x41acf4

// block 0041accd
0041accd  lea        ecx, [eax - 0x32]
0041acd0  mov        eax, 0x66666667
0041acd5  imul       ecx
0041acd7  sar        edx, 1
0041acd9  mov        ecx, edx
0041acdb  shr        ecx, 0x1f
0041acde  lea        eax, [edx + ecx + 0x32]
0041ace2  cmp        eax, 0x50
0041ace5  jl         0x41acf4

// block 0041ace7
0041ace7  lea        eax, [ebp + 0x50]
0041acea  pop        edi
0041aceb  pop        esi
0041acec  pop        ebp
0041aced  pop        ebx
0041acee  mov        esp, ebp
0041acf0  pop        ebp
0041acf1  ret        

// block 0041acf2
0041acf2  mov        eax, ebx

// block 0041acf4
0041acf4  pop        edi
0041acf5  pop        esi
0041acf6  pop        ebp
0041acf7  pop        ebx
0041acf8  mov        esp, ebp
0041acfa  pop        ebp
0041acfb  ret        
