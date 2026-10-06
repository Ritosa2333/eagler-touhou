
// block 0042e800
0042e800  fld        dword ptr [ecx]
0042e802  fld        dword ptr [esp + 4]
0042e806  fld        st(0)
0042e808  fmulp      st(2)
0042e80a  fxch       st(1)
0042e80c  fstp       dword ptr [eax]
0042e80e  fld        dword ptr [ecx + 4]
0042e811  fmul       st(1)
0042e813  fstp       dword ptr [eax + 4]
0042e816  fmul       dword ptr [ecx + 8]
0042e819  fstp       dword ptr [eax + 8]
0042e81c  ret        4
