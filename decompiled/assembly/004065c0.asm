
// block 004065c0
004065c0  push       eax
004065c1  push       esi
004065c2  call       0x46c6bc

// block 004065c7
004065c7  fld        dword ptr [esi]
004065c9  fld        dword ptr [esp + 4]
004065cd  fld        st(0)
004065cf  fmulp      st(2)
004065d1  fxch       st(1)
004065d3  fstp       dword ptr [esi]
004065d5  fld        dword ptr [esi + 4]
004065d8  fmul       st(1)
004065da  fstp       dword ptr [esi + 4]
004065dd  fmul       dword ptr [esi + 8]
004065e0  fstp       dword ptr [esi + 8]
004065e3  ret        4
