
// block 00422c10
00422c10  fld        dword ptr [ecx]
00422c12  fadd       qword ptr [0x4a3d70]
00422c18  fadd       qword ptr [0x4a3d28]
00422c1e  fstp       dword ptr [eax]
00422c20  fld        dword ptr [ecx + 4]
00422c23  fadd       qword ptr [0x4a3d68]
00422c29  fstp       dword ptr [eax + 4]
00422c2c  fld        dword ptr [ecx + 8]
00422c2f  fstp       dword ptr [eax + 8]
00422c32  ret        
