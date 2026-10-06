
// block 0041c3f0
0041c3f0  fld        dword ptr [ecx]
0041c3f2  fld        dword ptr [esp + 4]
0041c3f6  fld        st(0)
0041c3f8  fmulp      st(2)
0041c3fa  fxch       st(1)
0041c3fc  fstp       dword ptr [eax]
0041c3fe  fmul       dword ptr [ecx + 4]
0041c401  fstp       dword ptr [eax + 4]
0041c404  ret        4
