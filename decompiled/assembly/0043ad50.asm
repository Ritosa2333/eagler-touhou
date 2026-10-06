
// block 0043ad50
0043ad50  push       ebp
0043ad51  mov        ebp, esp
0043ad53  and        esp, 0xfffffff8
0043ad56  sub        esp, 8
0043ad59  fld        dword ptr [eax + 4]
0043ad5c  fsub       dword ptr [ecx + 4]
0043ad5f  fstp       dword ptr [esp + 4]
0043ad63  fld        dword ptr [esp + 4]
0043ad67  fld        dword ptr [eax]
0043ad69  fsub       dword ptr [ecx]
0043ad6b  fstp       dword ptr [esp + 4]
0043ad6f  fld        dword ptr [esp + 4]
0043ad73  call       0x4937aa

// block 0043ad78
0043ad78  fstp       dword ptr [esp + 4]
0043ad7c  fld        dword ptr [esp + 4]
0043ad80  mov        esp, ebp
0043ad82  pop        ebp
0043ad83  ret        
