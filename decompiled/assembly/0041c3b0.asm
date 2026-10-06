
// block 0041c3b0
0041c3b0  push       ecx
0041c3b1  fld        dword ptr [eax + 0x28]
0041c3b4  fidiv      dword ptr [eax + 0x34]
0041c3b7  mov        eax, dword ptr [eax + 0x38]
0041c3ba  dec        eax
0041c3bb  fstp       dword ptr [esp]
0041c3be  fld        dword ptr [esp]
0041c3c1  cmp        eax, 0xf
0041c3c4  ja         0x41c3cf

// block 0041c3c6
0041c3c6  push       ecx
0041c3c7  fstp       dword ptr [esp]
0041c3ca  call       0x464f80

// block 0041c3cf
0041c3cf  fstp       dword ptr [esp]
0041c3d2  fld        dword ptr [esp]
0041c3d5  pop        ecx
0041c3d6  ret        
