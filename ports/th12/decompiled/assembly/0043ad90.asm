
// block 0043ad90
0043ad90  push       ecx
0043ad91  fld        dword ptr [ecx + 4]
0043ad94  fsub       dword ptr [eax + 4]
0043ad97  fld        dword ptr [ecx]
0043ad99  fsub       dword ptr [eax]
0043ad9b  fmul       st(0), st(0)
0043ad9d  fld        st(1)
0043ad9f  fmulp      st(2)
0043ada1  faddp      st(1)
0043ada3  fstp       dword ptr [esp]
0043ada6  fld        dword ptr [esp]
0043ada9  pop        ecx
0043adaa  ret        
