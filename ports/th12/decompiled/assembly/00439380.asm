
// block 00439380
00439380  push       ebp
00439381  mov        ebp, esp
00439383  and        esp, 0xfffffff8
00439386  sub        esp, 8
00439389  fld        dword ptr [eax + 4]
0043938c  fld        dword ptr [eax]
0043938e  call       0x4937aa

// block 00439393
00439393  fstp       dword ptr [esp + 4]
00439397  fld        dword ptr [esp + 4]
0043939b  mov        esp, ebp
0043939d  pop        ebp
0043939e  ret        
