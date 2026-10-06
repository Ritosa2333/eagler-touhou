
// block 00439420
00439420  push       ecx
00439421  fld        dword ptr [ecx + 4]
00439424  fsub       dword ptr [eax + 4]
00439427  fld        dword ptr [ecx]
00439429  fsub       dword ptr [eax]
0043942b  fmul       st(0), st(0)
0043942d  fld        st(1)
0043942f  fmulp      st(2)
00439431  faddp      st(1)
00439433  fstp       dword ptr [esp]
00439436  fld        dword ptr [esp]
00439439  pop        ecx
0043943a  ret        
