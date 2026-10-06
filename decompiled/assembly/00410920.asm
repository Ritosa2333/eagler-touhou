
// block 00410920
00410920  push       ecx
00410921  call       0x464440

// block 00410926
00410926  mov        dword ptr [esp], eax
00410929  fild       dword ptr [esp]
0041092c  test       eax, eax
0041092e  jge        0x410936

// block 00410930
00410930  fadd       dword ptr [0x4a3e10]

// block 00410936
00410936  fmul       qword ptr [0x4a3ea8]
0041093c  fsub       qword ptr [0x4a3ce0]
00410942  fstp       dword ptr [esp]
00410945  fld        dword ptr [esp]
00410948  fmul       dword ptr [esp + 8]
0041094c  fstp       dword ptr [esp + 8]
00410950  fld        dword ptr [esp + 8]
00410954  pop        ecx
00410955  ret        4
