
// block 00406020
00406020  push       ecx
00406021  fld        dword ptr [eax + 0x38]
00406024  fidiv      dword ptr [eax + 0x44]
00406027  mov        eax, dword ptr [eax + 0x48]
0040602a  dec        eax
0040602b  fstp       dword ptr [esp]
0040602e  fld        dword ptr [esp]
00406031  cmp        eax, 0xf
00406034  ja         0x40603f

// block 00406036
00406036  push       ecx
00406037  fstp       dword ptr [esp]
0040603a  call       0x464f80

// block 0040603f
0040603f  fstp       dword ptr [esp]
00406042  fld        dword ptr [esp]
00406045  pop        ecx
00406046  ret        
