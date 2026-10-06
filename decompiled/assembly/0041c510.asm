
// block 0041c510
0041c510  push       ebp
0041c511  mov        ebp, esp
0041c513  and        esp, 0xfffffff8
0041c516  sub        esp, 8
0041c519  fld        dword ptr [eax + 4]
0041c51c  fld        dword ptr [eax]
0041c51e  call       0x4937aa

// block 0041c523
0041c523  fstp       dword ptr [esp + 4]
0041c527  fld        dword ptr [esp + 4]
0041c52b  mov        esp, ebp
0041c52d  pop        ebp
0041c52e  ret        
