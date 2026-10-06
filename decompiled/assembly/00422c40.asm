
// block 00422c40
00422c40  mov        ecx, 1
00422c45  test       byte ptr [0x4d50cc], cl
00422c4b  jne        0x422c53

// block 00422c4d
00422c4d  or         dword ptr [0x4d50cc], ecx

// block 00422c53
00422c53  fld        dword ptr [eax]
00422c55  fadd       qword ptr [0x4a3d70]
00422c5b  fadd       qword ptr [0x4a3d28]
00422c61  fstp       dword ptr [0x4d50c0]
00422c67  fld        dword ptr [eax + 4]
00422c6a  fadd       qword ptr [0x4a3d68]
00422c70  fstp       dword ptr [0x4d50c4]
00422c76  fld        dword ptr [eax + 8]
00422c79  mov        eax, 0x4d50c0
00422c7e  fstp       dword ptr [0x4d50c8]
00422c84  ret        
