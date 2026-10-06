
// block 0040d4b0
0040d4b0  push       ecx
0040d4b1  fld        dword ptr [eax + 4]
0040d4b4  fmul       dword ptr [ecx]
0040d4b6  fld        dword ptr [ecx + 4]
0040d4b9  fmul       dword ptr [eax]
0040d4bb  fsubp      st(1)
0040d4bd  fstp       dword ptr [esp]
0040d4c0  fld        dword ptr [esp]
0040d4c3  pop        ecx
0040d4c4  ret        
