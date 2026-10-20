
// block 00437600
00437600  cmp        dword ptr [eax + 0xa28], 2
00437607  je         0x43764e

// block 00437609
00437609  fld        dword ptr [eax + 0x97c]
0043760f  mov        ecx, dword ptr [0x4ce8cc]
00437615  fadd       qword ptr [0x4a3d70]
0043761b  add        eax, 0x14
0043761e  push       ecx
0043761f  fadd       qword ptr [0x4a3d28]
00437625  fstp       dword ptr [eax + 0x430]
0043762b  fld        dword ptr [eax + 0x96c]
00437631  fadd       qword ptr [0x4a3d68]
00437637  fstp       dword ptr [eax + 0x434]
0043763d  fld        dword ptr [eax + 0x970]
00437643  fstp       dword ptr [eax + 0x438]
00437649  call       0x45c900

// block 0043764e
0043764e  mov        eax, 1
00437653  ret        
