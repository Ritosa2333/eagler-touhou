
// block 00465320
00465320  push       ebp
00465321  mov        ebp, esp
00465323  and        esp, 0xffffffc0
00465326  sub        esp, 0x40
00465329  fld        dword ptr [esi]
0046532b  sub        esp, 8
0046532e  fmul       qword ptr [0x4a3ce8]
00465334  fstp       dword ptr [esp + 0x44]
00465338  fld        dword ptr [esp + 0x44]
0046533c  fstp       qword ptr [esp]
0046533f  call       0x493290

// block 00465344
00465344  fstp       dword ptr [esp + 0x44]
00465348  fld        dword ptr [esp + 0x44]
0046534c  fld        qword ptr [0x4a3ce8]
00465352  fdiv       st(1), st(0)
00465354  fxch       st(1)
00465356  fstp       dword ptr [esi]
00465358  fmul       dword ptr [esi + 4]
0046535b  fstp       dword ptr [esp + 0x44]
0046535f  fld        dword ptr [esp + 0x44]
00465363  fstp       qword ptr [esp]
00465366  call       0x493290

// block 0046536b
0046536b  fstp       dword ptr [esp + 0x44]
0046536f  add        esp, 8
00465372  fld        dword ptr [esp + 0x3c]
00465376  fdiv       qword ptr [0x4a3ce8]
0046537c  fstp       dword ptr [esi + 4]
0046537f  mov        esp, ebp
00465381  pop        ebp
00465382  ret        
