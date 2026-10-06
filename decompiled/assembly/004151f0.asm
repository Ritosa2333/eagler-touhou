
// block 004151f0
004151f0  push       ecx
004151f1  mov        eax, dword ptr [0x4b43dc]
004151f6  mov        eax, dword ptr [eax + 0x68]
004151f9  test       eax, eax
004151fb  je         0x415337

// block 00415201
00415201  push       ebp
00415202  push       esi
00415203  push       edi

// block 00415204
00415204  mov        esi, dword ptr [eax]
00415206  fld        qword ptr [0x4a3cf8]
0041520c  test       dword ptr [esi + 0x26f8], 0x800
00415216  fld        dword ptr [0x4a4014]
0041521c  mov        edi, dword ptr [eax + 4]
0041521f  mov        dword ptr [esp + 0xc], edi
00415223  je         0x415326

// block 00415229
00415229  mov        edx, dword ptr [esp + 0x14]
0041522d  fld        dword ptr [edx]
0041522f  lea        ecx, [esi + 0x1074]
00415235  fmul       st(2)
00415237  fld        dword ptr [ecx]
00415239  fld        dword ptr [ebx]
0041523b  fsub       st(2)
0041523d  fcompp     
0041523f  fnstsw     ax
00415241  test       ah, 0x41
00415244  jne        0x415324

// block 0041524a
0041524a  fld        dword ptr [ecx]
0041524c  fld        dword ptr [ebx]
0041524e  faddp      st(2)
00415250  fcompp     
00415252  fnstsw     ax
00415254  test       ah, 0x41
00415257  jne        0x415326

// block 0041525d
0041525d  fld        dword ptr [edx + 4]
00415260  fmulp      st(2)
00415262  fld        dword ptr [esi + 0x1078]
00415268  fld        dword ptr [ebx + 4]
0041526b  fsub       st(3)
0041526d  fcompp     
0041526f  fnstsw     ax
00415271  test       ah, 0x41
00415274  jne        0x415326

// block 0041527a
0041527a  fld        dword ptr [esi + 0x1078]
00415280  fld        dword ptr [ebx + 4]
00415283  faddp      st(3)
00415285  fcomp      st(2)
00415287  fnstsw     ax
00415289  fstp       st(1)
0041528b  test       ah, 0x41
0041528e  jne        0x415328

// block 00415294
00415294  test       byte ptr [esi + 0x265c], 1
0041529b  je         0x4152e1

// block 0041529d
0041529d  mov        edi, dword ptr [esi + 0x2658]
004152a3  add        dword ptr [esi + 0x2654], 0xfffe7961
004152ad  mov        eax, dword ptr [esi + 0x2654]
004152b3  lea        edx, [edi*8]
004152ba  sub        edx, edi
004152bc  sub        eax, edx
004152be  mov        ebp, eax
004152c0  mov        eax, 0x92492493
004152c5  imul       ebp
004152c7  add        edx, ebp
004152c9  sar        edx, 2
004152cc  mov        eax, edx
004152ce  shr        eax, 0x1f
004152d1  add        eax, edx
004152d3  add        eax, edi
004152d5  mov        edi, dword ptr [esp + 0xc]
004152d9  mov        dword ptr [esi + 0x2648], eax
004152df  jmp        0x4152eb

// block 004152e1
004152e1  add        dword ptr [esi + 0x2648], 0xfffe7961

// block 004152eb
004152eb  sub        esp, 8
004152ee  fst        dword ptr [esp + 4]
004152f2  fstp       dword ptr [esp]
004152f5  call       0x41c780

// block 004152fa
004152fa  test       eax, eax
004152fc  jne        0x41532a

// block 004152fe
004152fe  cmp        dword ptr [esp + 0x18], eax
00415302  je         0x41532a

// block 00415304
00415304  fld        dword ptr [0x4a41f4]
0041530a  sub        esp, 8
0041530d  fstp       dword ptr [esp + 4]
00415311  fld        dword ptr [0x4a4060]
00415317  fstp       dword ptr [esp]
0041531a  push       ecx
0041531b  push       9
0041531d  call       0x4273f0

// block 00415322
00415322  jmp        0x41532a

// block 00415324
00415324  fstp       st(0)

// block 00415326
00415326  fstp       st(1)

// block 00415328
00415328  fstp       st(0)

// block 0041532a
0041532a  mov        eax, edi
0041532c  test       edi, edi
0041532e  jne        0x415204

// block 00415334
00415334  pop        edi
00415335  pop        esi
00415336  pop        ebp

// block 00415337
00415337  pop        ecx
00415338  ret        8
