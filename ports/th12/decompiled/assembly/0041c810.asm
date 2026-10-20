
// block 0041c810
0041c810  push       ecx
0041c811  fld        dword ptr [eax + 4]
0041c814  fld        dword ptr [eax]
0041c816  fmul       st(0), st(0)
0041c818  fld        st(1)
0041c81a  fmulp      st(2)
0041c81c  faddp      st(1)
0041c81e  fstp       dword ptr [esp]
0041c821  fld        dword ptr [esp]
0041c824  pop        ecx
0041c825  ret        
