
// block 004108e0
004108e0  push       ecx
004108e1  mov        dword ptr [esp], ecx
004108e4  mov        eax, dword ptr [esp]
004108e7  fld        dword ptr [esp + 8]
004108eb  fsincos    
004108ed  fmul       dword ptr [esp + 0xc]
004108f1  fstp       dword ptr [eax]
004108f3  fmul       dword ptr [esp + 0xc]
004108f7  fstp       dword ptr [eax + 4]
004108fa  pop        ecx
004108fb  ret        8
