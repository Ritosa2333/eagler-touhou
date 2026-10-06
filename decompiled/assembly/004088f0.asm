
// block 004088f0
004088f0  push       ebp
004088f1  mov        ebp, esp
004088f3  and        esp, 0xfffffff8
004088f6  sub        esp, 8
004088f9  fld        dword ptr [eax + 4]
004088fc  fld        dword ptr [eax]
004088fe  call       0x4937aa

// block 00408903
00408903  fstp       dword ptr [esp + 4]
00408907  fld        dword ptr [esp + 4]
0040890b  mov        esp, ebp
0040890d  pop        ebp
0040890e  ret        
