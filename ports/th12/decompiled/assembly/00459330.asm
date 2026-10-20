
// block 00459330
00459330  push       ecx
00459331  fld        dword ptr [eax + 0x18]
00459334  fidiv      dword ptr [eax + 0x24]
00459337  mov        eax, dword ptr [eax + 0x28]
0045933a  dec        eax
0045933b  fstp       dword ptr [esp]
0045933e  fld        dword ptr [esp]
00459341  cmp        eax, 0xf
00459344  ja         0x45934f

// block 00459346
00459346  push       ecx
00459347  fstp       dword ptr [esp]
0045934a  call       0x464f80

// block 0045934f
0045934f  fstp       dword ptr [esp]
00459352  fld        dword ptr [esp]
00459355  pop        ecx
00459356  ret        
