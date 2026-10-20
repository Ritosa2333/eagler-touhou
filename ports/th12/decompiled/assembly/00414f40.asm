
// block 00414f40
00414f40  mov        eax, dword ptr [0x4b43dc]
00414f45  mov        eax, dword ptr [eax + 0x68]
00414f48  sub        esp, 0x10
00414f4b  push       ebx
00414f4c  push       ebp
00414f4d  push       esi
00414f4e  push       edi
00414f4f  test       eax, eax
00414f51  je         0x4150a7

// block 00414f57
00414f57  fld        qword ptr [0x4a3d00]

// block 00414f5d
00414f5d  mov        ecx, dword ptr [eax]
00414f5f  test       dword ptr [ecx + 0x26f8], 0x800
00414f69  mov        ebx, dword ptr [eax + 4]
00414f6c  je         0x41509b

// block 00414f72
00414f72  mov        eax, dword ptr [esp + 0x24]
00414f76  fld        dword ptr [ecx + 0x1074]
00414f7c  fsub       dword ptr [eax]
00414f7e  lea        ebp, [ecx + 0x1074]
00414f84  fstp       dword ptr [esp + 0x14]
00414f88  fld        dword ptr [ebp + 4]
00414f8b  fsub       dword ptr [eax + 4]
00414f8e  fstp       dword ptr [esp + 0x18]
00414f92  fld        dword ptr [esp + 0x28]
00414f96  fadd       qword ptr [0x4a3d68]
00414f9c  fstp       dword ptr [esp + 0x10]
00414fa0  fld        dword ptr [esp + 0x18]
00414fa4  fld        dword ptr [esp + 0x14]
00414fa8  fld        dword ptr [esp + 0x10]
00414fac  fld        st(1)
00414fae  fmulp      st(2)
00414fb0  fld        st(2)
00414fb2  fmulp      st(3)
00414fb4  fxch       st(1)
00414fb6  faddp      st(2)
00414fb8  fxch       st(1)
00414fba  fstp       dword ptr [esp + 0x10]
00414fbe  fld        dword ptr [esp + 0x10]
00414fc2  fld        st(1)
00414fc4  fmulp      st(2)
00414fc6  fcompp     
00414fc8  fnstsw     ax
00414fca  test       ah, 0x41
00414fcd  jp         0x41509b

// block 00414fd3
00414fd3  test       byte ptr [ecx + 0x265c], 1
00414fda  je         0x41501c

// block 00414fdc
00414fdc  mov        esi, dword ptr [ecx + 0x2658]
00414fe2  add        dword ptr [ecx + 0x2654], 0xfffe7961
00414fec  mov        eax, dword ptr [ecx + 0x2654]
00414ff2  lea        edx, [esi*8]
00414ff9  sub        edx, esi
00414ffb  sub        eax, edx
00414ffd  mov        edi, eax
00414fff  mov        eax, 0x92492493
00415004  imul       edi
00415006  add        edx, edi
00415008  sar        edx, 2
0041500b  mov        eax, edx
0041500d  shr        eax, 0x1f
00415010  add        eax, edx
00415012  add        eax, esi
00415014  mov        dword ptr [ecx + 0x2648], eax
0041501a  jmp        0x415026

// block 0041501c
0041501c  add        dword ptr [ecx + 0x2648], 0xfffe7961

// block 00415026
00415026  fld        dword ptr [ebp]
00415029  fadd       st(1)
0041502b  fcomp      qword ptr [0x4a3d60]
00415031  fnstsw     ax
00415033  test       ah, 0x41
00415036  jnp        0x41509b

// block 00415038
00415038  fld        dword ptr [ebp]
0041503b  fsub       st(1)
0041503d  fcomp      qword ptr [0x4a3d28]
00415043  fnstsw     ax
00415045  test       ah, 1
00415048  je         0x41509b

// block 0041504a
0041504a  fld        dword ptr [ebp + 4]
0041504d  fadd       st(1)
0041504f  fcomp      qword ptr [0x4a3d58]
00415055  fnstsw     ax
00415057  test       ah, 0x41
0041505a  jnp        0x41509b

// block 0041505c
0041505c  fld        dword ptr [ebp + 4]
0041505f  fsub       st(1)
00415061  fcomp      qword ptr [0x4a3d50]
00415067  fnstsw     ax
00415069  test       ah, 1
0041506c  je         0x41509b

// block 0041506e
0041506e  cmp        dword ptr [esp + 0x2c], 0
00415073  je         0x41509b

// block 00415075
00415075  fstp       st(0)
00415077  sub        esp, 8
0041507a  fld        dword ptr [0x4a41f4]
00415080  fstp       dword ptr [esp + 4]
00415084  fld        dword ptr [0x4a4060]
0041508a  fstp       dword ptr [esp]
0041508d  push       ebp
0041508e  push       9
00415090  call       0x4273f0

// block 00415095
00415095  fld        qword ptr [0x4a3d00]

// block 0041509b
0041509b  mov        eax, ebx
0041509d  test       ebx, ebx
0041509f  jne        0x414f5d

// block 004150a5
004150a5  fstp       st(0)

// block 004150a7
004150a7  pop        edi
004150a8  pop        esi
004150a9  pop        ebp
004150aa  pop        ebx
004150ab  add        esp, 0x10
004150ae  ret        0xc
