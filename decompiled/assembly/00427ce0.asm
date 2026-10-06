
// block 00427ce0
00427ce0  push       ecx
00427ce1  fld        dword ptr [ecx + 4]
00427ce4  fsub       dword ptr [eax + 4]
00427ce7  fld        dword ptr [ecx]
00427ce9  fsub       dword ptr [eax]
00427ceb  fmul       st(0), st(0)
00427ced  fld        st(1)
00427cef  fmulp      st(2)
00427cf1  faddp      st(1)
00427cf3  fstp       dword ptr [esp]
00427cf6  fld        dword ptr [esp]
00427cf9  pop        ecx
00427cfa  ret        
