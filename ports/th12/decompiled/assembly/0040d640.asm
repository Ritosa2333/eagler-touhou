
// block 0040d640
0040d640  push       ecx
0040d641  mov        dword ptr [esp], ecx
0040d644  mov        eax, dword ptr [esp]
0040d647  fld        dword ptr [esp + 8]
0040d64b  fsincos    
0040d64d  fmul       dword ptr [esp + 0xc]
0040d651  fstp       dword ptr [eax]
0040d653  fmul       dword ptr [esp + 0xc]
0040d657  fstp       dword ptr [eax + 4]
0040d65a  pop        ecx
0040d65b  ret        8
