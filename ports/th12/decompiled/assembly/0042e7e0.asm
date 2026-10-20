
// block 0042e7e0
0042e7e0  push       ecx
0042e7e1  fld        dword ptr [ecx + 4]
0042e7e4  fsub       dword ptr [eax + 4]
0042e7e7  fld        dword ptr [ecx]
0042e7e9  fsub       dword ptr [eax]
0042e7eb  fmul       st(0), st(0)
0042e7ed  fld        st(1)
0042e7ef  fmulp      st(2)
0042e7f1  faddp      st(1)
0042e7f3  fstp       dword ptr [esp]
0042e7f6  fld        dword ptr [esp]
0042e7f9  pop        ecx
0042e7fa  ret        
