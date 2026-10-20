
// block 00459360
00459360  push       ecx
00459361  fld        dword ptr [eax + 0x28]
00459364  fidiv      dword ptr [eax + 0x34]
00459367  mov        eax, dword ptr [eax + 0x38]
0045936a  dec        eax
0045936b  fstp       dword ptr [esp]
0045936e  fld        dword ptr [esp]
00459371  cmp        eax, 0xf
00459374  ja         0x45937f

// block 00459376
00459376  push       ecx
00459377  fstp       dword ptr [esp]
0045937a  call       0x464f80

// block 0045937f
0045937f  fstp       dword ptr [esp]
00459382  fld        dword ptr [esp]
00459385  pop        ecx
00459386  ret        
