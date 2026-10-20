
// block 0041c490
0041c490  push       ecx
0041c491  fld        dword ptr [ecx + 4]
0041c494  fsub       dword ptr [eax + 4]
0041c497  fld        dword ptr [ecx]
0041c499  fsub       dword ptr [eax]
0041c49b  fmul       st(0), st(0)
0041c49d  fld        st(1)
0041c49f  fmulp      st(2)
0041c4a1  faddp      st(1)
0041c4a3  fstp       dword ptr [esp]
0041c4a6  fld        dword ptr [esp]
0041c4a9  pop        ecx
0041c4aa  ret        
