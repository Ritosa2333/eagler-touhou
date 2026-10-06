
// block 00409310
00409310  push       ecx
00409311  push       esi
00409312  mov        esi, 0x4ce568
00409317  call       0x464440

// block 0040931c
0040931c  mov        dword ptr [esp + 4], eax
00409320  fild       dword ptr [esp + 4]
00409324  test       eax, eax
00409326  jge        0x40932e

// block 00409328
00409328  fadd       dword ptr [0x4a3e10]

// block 0040932e
0040932e  fmul       qword ptr [0x4a3ea8]
00409334  pop        esi
00409335  fsub       qword ptr [0x4a3ce0]
0040933b  fstp       dword ptr [esp]
0040933e  fld        dword ptr [esp]
00409341  fmul       qword ptr [0x4a3cd8]
00409347  fstp       dword ptr [esp]
0040934a  fld        dword ptr [esp]
0040934d  pop        ecx
0040934e  ret        
