
// block 00459390
00459390  push       ecx
00459391  fld        dword ptr [eax + 0x18]
00459394  fidiv      dword ptr [eax + 0x24]
00459397  mov        eax, dword ptr [eax + 0x28]
0045939a  dec        eax
0045939b  fstp       dword ptr [esp]
0045939e  fld        dword ptr [esp]
004593a1  cmp        eax, 0xf
004593a4  ja         0x4593af

// block 004593a6
004593a6  push       ecx
004593a7  fstp       dword ptr [esp]
004593aa  call       0x464f80

// block 004593af
004593af  fstp       dword ptr [esp]
004593b2  fld        dword ptr [esp]
004593b5  pop        ecx
004593b6  ret        
