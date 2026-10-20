
// block 0040d490
0040d490  push       ecx
0040d491  fld        dword ptr [ecx + 4]
0040d494  fmul       dword ptr [eax + 4]
0040d497  fld        dword ptr [ecx]
0040d499  fmul       dword ptr [eax]
0040d49b  faddp      st(1)
0040d49d  fstp       dword ptr [esp]
0040d4a0  fld        dword ptr [esp]
0040d4a3  pop        ecx
0040d4a4  ret        
