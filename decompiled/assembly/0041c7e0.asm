
// block 0041c7e0
0041c7e0  push       eax
0041c7e1  push       esi
0041c7e2  call       0x46c6bc

// block 0041c7e7
0041c7e7  fld        dword ptr [esi]
0041c7e9  fld        dword ptr [esp + 4]
0041c7ed  fld        st(0)
0041c7ef  fmulp      st(2)
0041c7f1  fxch       st(1)
0041c7f3  fstp       dword ptr [esi]
0041c7f5  fld        dword ptr [esi + 4]
0041c7f8  fmul       st(1)
0041c7fa  fstp       dword ptr [esi + 4]
0041c7fd  fmul       dword ptr [esi + 8]
0041c800  fstp       dword ptr [esi + 8]
0041c803  ret        4
