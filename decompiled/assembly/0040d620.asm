
// block 0040d620
0040d620  push       ecx
0040d621  fld        dword ptr [ecx + 4]
0040d624  fsub       dword ptr [eax + 4]
0040d627  fld        dword ptr [ecx]
0040d629  fsub       dword ptr [eax]
0040d62b  fmul       st(0), st(0)
0040d62d  fld        st(1)
0040d62f  fmulp      st(2)
0040d631  faddp      st(1)
0040d633  fstp       dword ptr [esp]
0040d636  fld        dword ptr [esp]
0040d639  pop        ecx
0040d63a  ret        
