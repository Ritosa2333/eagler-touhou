
// block 0040d470
0040d470  push       ecx
0040d471  fld        dword ptr [eax + 4]
0040d474  fld        dword ptr [eax]
0040d476  fmul       st(0), st(0)
0040d478  fld        st(1)
0040d47a  fmulp      st(2)
0040d47c  faddp      st(1)
0040d47e  fstp       dword ptr [esp]
0040d481  fld        dword ptr [esp]
0040d484  pop        ecx
0040d485  ret        
