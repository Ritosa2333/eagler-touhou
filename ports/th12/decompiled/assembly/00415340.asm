
// block 00415340
00415340  mov        eax, dword ptr [0x4b43dc]
00415345  mov        eax, dword ptr [eax + 0x68]
00415348  sub        esp, 0x10
0041534b  test       eax, eax
0041534d  je         0x4154ab

// block 00415353
00415353  fld        qword ptr [0x4a3d00]
00415359  push       ebx
0041535a  push       ebp
0041535b  push       esi
0041535c  push       edi

// block 0041535d
0041535d  mov        ecx, dword ptr [eax]
0041535f  test       dword ptr [ecx + 0x26f8], 0x800
00415369  mov        ebx, dword ptr [eax + 4]
0041536c  je         0x41549b

// block 00415372
00415372  mov        eax, dword ptr [esp + 0x24]
00415376  fld        dword ptr [ecx + 0x1074]
0041537c  fsub       dword ptr [eax]
0041537e  lea        ebp, [ecx + 0x1074]
00415384  fstp       dword ptr [esp + 0x14]
00415388  fld        dword ptr [ebp + 4]
0041538b  fsub       dword ptr [eax + 4]
0041538e  fstp       dword ptr [esp + 0x18]
00415392  fld        dword ptr [esp + 0x28]
00415396  fadd       qword ptr [0x4a3d68]
0041539c  fstp       dword ptr [esp + 0x10]
004153a0  fld        dword ptr [esp + 0x18]
004153a4  fld        dword ptr [esp + 0x14]
004153a8  fld        dword ptr [esp + 0x10]
004153ac  fld        st(1)
004153ae  fmulp      st(2)
004153b0  fld        st(2)
004153b2  fmulp      st(3)
004153b4  fxch       st(1)
004153b6  faddp      st(2)
004153b8  fxch       st(1)
004153ba  fstp       dword ptr [esp + 0x10]
004153be  fld        dword ptr [esp + 0x10]
004153c2  fld        st(1)
004153c4  fmulp      st(2)
004153c6  fcompp     
004153c8  fnstsw     ax
004153ca  test       ah, 0x41
004153cd  jp         0x41549b

// block 004153d3
004153d3  test       byte ptr [ecx + 0x265c], 1
004153da  je         0x41541c

// block 004153dc
004153dc  mov        esi, dword ptr [ecx + 0x2658]
004153e2  add        dword ptr [ecx + 0x2654], 0xfffe7961
004153ec  mov        eax, dword ptr [ecx + 0x2654]
004153f2  lea        edx, [esi*8]
004153f9  sub        edx, esi
004153fb  sub        eax, edx
004153fd  mov        edi, eax
004153ff  mov        eax, 0x92492493
00415404  imul       edi
00415406  add        edx, edi
00415408  sar        edx, 2
0041540b  mov        eax, edx
0041540d  shr        eax, 0x1f
00415410  add        eax, edx
00415412  add        eax, esi
00415414  mov        dword ptr [ecx + 0x2648], eax
0041541a  jmp        0x415426

// block 0041541c
0041541c  add        dword ptr [ecx + 0x2648], 0xfffe7961

// block 00415426
00415426  fld        dword ptr [ebp]
00415429  fadd       st(1)
0041542b  fcomp      qword ptr [0x4a3d60]
00415431  fnstsw     ax
00415433  test       ah, 0x41
00415436  jnp        0x41549b

// block 00415438
00415438  fld        dword ptr [ebp]
0041543b  fsub       st(1)
0041543d  fcomp      qword ptr [0x4a3d28]
00415443  fnstsw     ax
00415445  test       ah, 1
00415448  je         0x41549b

// block 0041544a
0041544a  fld        dword ptr [ebp + 4]
0041544d  fadd       st(1)
0041544f  fcomp      qword ptr [0x4a3d58]
00415455  fnstsw     ax
00415457  test       ah, 0x41
0041545a  jnp        0x41549b

// block 0041545c
0041545c  fld        dword ptr [ebp + 4]
0041545f  fsub       st(1)
00415461  fcomp      qword ptr [0x4a3d50]
00415467  fnstsw     ax
00415469  test       ah, 1
0041546c  je         0x41549b

// block 0041546e
0041546e  cmp        dword ptr [esp + 0x2c], 0
00415473  je         0x41549b

// block 00415475
00415475  fstp       st(0)
00415477  sub        esp, 8
0041547a  fld        dword ptr [0x4a41f4]
00415480  fstp       dword ptr [esp + 4]
00415484  fld        dword ptr [0x4a4060]
0041548a  fstp       dword ptr [esp]
0041548d  push       ebp
0041548e  push       9
00415490  call       0x4273f0

// block 00415495
00415495  fld        qword ptr [0x4a3d00]

// block 0041549b
0041549b  mov        eax, ebx
0041549d  test       ebx, ebx
0041549f  jne        0x41535d

// block 004154a5
004154a5  pop        edi
004154a6  fstp       st(0)
004154a8  pop        esi
004154a9  pop        ebp
004154aa  pop        ebx

// block 004154ab
004154ab  add        esp, 0x10
004154ae  ret        0xc
