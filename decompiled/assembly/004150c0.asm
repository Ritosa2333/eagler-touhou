
// block 004150c0
004150c0  mov        eax, dword ptr [0x4b43dc]
004150c5  mov        eax, dword ptr [eax + 0x68]
004150c8  test       eax, eax
004150ca  je         0x4151e9

// block 004150d0
004150d0  push       ebx
004150d1  push       ebp
004150d2  push       esi
004150d3  push       edi
004150d4  jmp        0x4150e0

// block 004150e0
004150e0  mov        esi, dword ptr [eax]
004150e2  fld        dword ptr [esp + 0x18]
004150e6  test       dword ptr [esi + 0x26f8], 0x800
004150f0  fld        dword ptr [0x4a4014]
004150f6  mov        ebx, dword ptr [eax + 4]
004150f9  je         0x4151d7

// block 004150ff
004150ff  fld        dword ptr [esi + 0x1074]
00415105  mov        edx, dword ptr [esp + 0x14]
00415109  fld        dword ptr [edx]
0041510b  lea        ecx, [esi + 0x1074]
00415111  fsub       st(3)
00415113  fcompp     
00415115  fnstsw     ax
00415117  test       ah, 0x41
0041511a  jp         0x4151d7

// block 00415120
00415120  fld        dword ptr [ecx]
00415122  fld        dword ptr [edx]
00415124  faddp      st(3)
00415126  fcomp      st(2)
00415128  fnstsw     ax
0041512a  fstp       st(1)
0041512c  test       ah, 0x41
0041512f  jp         0x4151d9

// block 00415135
00415135  fld        dword ptr [esi + 0x1078]
0041513b  fld        dword ptr [edx + 4]
0041513e  fcompp     
00415140  fnstsw     ax
00415142  test       ah, 0x41
00415145  jne        0x4151d9

// block 0041514b
0041514b  test       byte ptr [esi + 0x265c], 1
00415152  je         0x415194

// block 00415154
00415154  mov        edi, dword ptr [esi + 0x2658]
0041515a  add        dword ptr [esi + 0x2654], 0xfffe7961
00415164  mov        eax, dword ptr [esi + 0x2654]
0041516a  lea        edx, [edi*8]
00415171  sub        edx, edi
00415173  sub        eax, edx
00415175  mov        ebp, eax
00415177  mov        eax, 0x92492493
0041517c  imul       ebp
0041517e  add        edx, ebp
00415180  sar        edx, 2
00415183  mov        eax, edx
00415185  shr        eax, 0x1f
00415188  add        eax, edx
0041518a  add        eax, edi
0041518c  mov        dword ptr [esi + 0x2648], eax
00415192  jmp        0x41519e

// block 00415194
00415194  add        dword ptr [esi + 0x2648], 0xfffe7961

// block 0041519e
0041519e  sub        esp, 8
004151a1  fst        dword ptr [esp + 4]
004151a5  fstp       dword ptr [esp]
004151a8  call       0x41c780

// block 004151ad
004151ad  test       eax, eax
004151af  jne        0x4151db

// block 004151b1
004151b1  cmp        dword ptr [esp + 0x1c], eax
004151b5  je         0x4151db

// block 004151b7
004151b7  fld        dword ptr [0x4a41f4]
004151bd  sub        esp, 8
004151c0  fstp       dword ptr [esp + 4]
004151c4  fld        dword ptr [0x4a4060]
004151ca  fstp       dword ptr [esp]
004151cd  push       ecx
004151ce  push       9
004151d0  call       0x4273f0

// block 004151d5
004151d5  jmp        0x4151db

// block 004151d7
004151d7  fstp       st(1)

// block 004151d9
004151d9  fstp       st(0)

// block 004151db
004151db  mov        eax, ebx
004151dd  test       ebx, ebx
004151df  jne        0x4150e0

// block 004151e5
004151e5  pop        edi
004151e6  pop        esi
004151e7  pop        ebp
004151e8  pop        ebx

// block 004151e9
004151e9  ret        0xc
