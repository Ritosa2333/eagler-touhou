
// block 00459300
00459300  push       ecx
00459301  fld        dword ptr [eax + 0x38]
00459304  fidiv      dword ptr [eax + 0x44]
00459307  mov        eax, dword ptr [eax + 0x48]
0045930a  dec        eax
0045930b  fstp       dword ptr [esp]
0045930e  fld        dword ptr [esp]
00459311  cmp        eax, 0xf
00459314  ja         0x45931f

// block 00459316
00459316  push       ecx
00459317  fstp       dword ptr [esp]
0045931a  call       0x464f80

// block 0045931f
0045931f  fstp       dword ptr [esp]
00459322  fld        dword ptr [esp]
00459325  pop        ecx
00459326  ret        
