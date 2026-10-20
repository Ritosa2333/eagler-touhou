
// block 0043ac90
0043ac90  push       ebp
0043ac91  mov        ebp, esp
0043ac93  and        esp, 0xfffffff8
0043ac96  sub        esp, 8
0043ac99  fld        dword ptr [eax + 4]
0043ac9c  fld        dword ptr [eax]
0043ac9e  call       0x4937aa

// block 0043aca3
0043aca3  fstp       dword ptr [esp + 4]
0043aca7  fld        dword ptr [esp + 4]
0043acab  mov        esp, ebp
0043acad  pop        ebp
0043acae  ret        
